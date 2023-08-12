/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "game/systems/locomotion-system.hpp"
#include "game/components/pose-component.hpp"
#include "game/components/legged-locomotion-component.hpp"
#include "game/components/winged-locomotion-component.hpp"
#include "game/components/navmesh-agent-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include <engine/entity/id.hpp>
#include <engine/animation/skeleton.hpp>
#include <engine/debug/log.hpp>
#include <engine/math/fract.hpp>
#include <engine/ai/navmesh.hpp>
#include <algorithm>
#include <execution>

locomotion_system::locomotion_system(entity::registry& registry):
	updatable_system(registry)
{}

void locomotion_system::update(float t, float dt)
{
	update_legged(t, dt);
	update_winged(t, dt);
}

void locomotion_system::update_legged(float t, float dt)
{
	auto legged_group = registry.group<legged_locomotion_component>(entt::get<navmesh_agent_component, rigid_body_component, pose_component>);
	std::for_each
	(
		std::execution::par_unseq,
		legged_group.begin(),
		legged_group.end(),
		[&](auto entity_id)
		{
			auto& locomotion = legged_group.get<legged_locomotion_component>(entity_id);
			
			if (locomotion.angular_velocity != 0.0f)
			{
				auto& rigid_body = *legged_group.get<rigid_body_component>(entity_id).body;
				const auto up = rigid_body.get_orientation() * math::fvec3{0, 1, 0};
				const auto rotation = math::angle_axis(locomotion.angular_velocity * dt, up);
				rigid_body.set_orientation(math::normalize(rotation * rigid_body.get_orientation()));
			}
			
			// Traverse navmesh
			auto& navmesh_agent = legged_group.get<navmesh_agent_component>(entity_id);
			if (locomotion.speed != 0.0f && navmesh_agent.face)
			{
				// Get rigid body
				auto& rigid_body = *legged_group.get<rigid_body_component>(entity_id).body;
				const auto& rigid_body_transform = rigid_body.get_transform();
				
				// Construct ray with origin at agent position and forward-facing direction 
				geom::ray<float, 3> traversal_ray;
				traversal_ray.origin = rigid_body_transform.translation;
				traversal_ray.direction = rigid_body_transform.rotation * math::fvec3{0, 0, 1};
				
				// Traverse navmesh along ray
				const auto traversal = ai::traverse_navmesh(*navmesh_agent.mesh, navmesh_agent.face, traversal_ray, locomotion.speed * dt);
				
				// Update navmesh agent face
				navmesh_agent.face = traversal.face;
				
				// Interpolate navmesh vertex normals
				const auto& vertex_normals = navmesh_agent.mesh->vertices().attributes().at<math::fvec3>("normal");
				auto loop = navmesh_agent.face->loops().begin();
				const auto& na = vertex_normals[loop->vertex()->index()];
				const auto& nb = vertex_normals[(++loop)->vertex()->index()];
				const auto& nc = vertex_normals[(++loop)->vertex()->index()];
				const auto& uvw = traversal.barycentric;
				navmesh_agent.surface_normal = math::normalize(na * uvw.x() + nb * uvw.y() + nc * uvw.z());
				
				// const auto& face_normals = navmesh_agent.mesh->faces().attributes().at<math::fvec3>("normal");
				// navmesh_agent.surface_normal = face_normals[navmesh_agent.face->index()];
				
				// Update rigid body
				rigid_body.set_position(traversal.closest_point);
				// rigid_body.set_position(traversal_ray.extrapolate(locomotion.speed * dt));
				// rigid_body.set_orientation(math::normalize(math::rotation(rigid_body_transform.rotation * math::fvec3{0, 1, 0}, navmesh_agent.surface_normal) * rigid_body_transform.rotation));
				rigid_body.set_orientation(math::normalize(math::rotation(rigid_body_transform.rotation * math::fvec3{0, 1, 0}, navmesh_agent.surface_normal) * rigid_body_transform.rotation));
			}
			
			// Animate legs
			{
				// Get pose component
				auto& pose_component = legged_group.get<::pose_component>(entity_id);
				
				// Update gait phase
				locomotion.gait_phase = math::fract(locomotion.gait_phase + locomotion.speed * dt / locomotion.stride_length);
				
				// Update previous pose of body bone
				pose_component.previous_pose.set_relative_transform(locomotion.body_bone, pose_component.current_pose.get_relative_transform(locomotion.body_bone));
				
				// Update current pose of body bone
				auto body_xf = locomotion.midstance_pose->get_relative_transform(locomotion.body_bone);
				// body_xf.translation.x() += -std::cos(locomotion.gait_phase * math::two_pi<float>) * 0.01f;
				body_xf.translation.y() += locomotion.standing_height;// - std::sin(locomotion.gait_phase * math::four_pi<float>) * 0.025f;
				pose_component.current_pose.set_relative_transform(locomotion.body_bone, body_xf);
				
				// For each leg
				for (std::size_t i = 0; i < locomotion.tip_bones.size(); ++i)
				{
					// Determine step phase
					float step_phase = locomotion.gait->steps[i].phase(locomotion.gait_phase);
					
					// Determine leg pose
					const ::pose* pose_a;
					const ::pose* pose_b;
					float t;
					if (step_phase < 0.0f)
					{
						pose_b = locomotion.touchdown_pose;
						pose_a = locomotion.liftoff_pose;
						t = std::abs(step_phase);
					}
					else
					{
						if (step_phase < 0.5f)
						{
							pose_a = locomotion.liftoff_pose;
							pose_b = locomotion.midswing_pose;
							t = step_phase * 2.0f;
						}
						else
						{
							pose_a = locomotion.midswing_pose;
							pose_b = locomotion.touchdown_pose;
							t = (step_phase - 0.5f) * 2.0f;
						}
					}
					
					// Update leg bones
					bone_index_type bone_index = locomotion.tip_bones[i];
					for (std::uint8_t j = 0; j < locomotion.leg_bone_count; ++j)
					{
						if (j)
						{
							bone_index = pose_component.current_pose.get_skeleton()->get_bone_parent(bone_index);
						}
						
						// Update previous pose of leg bone
						pose_component.previous_pose.set_relative_transform(bone_index, pose_component.current_pose.get_relative_transform(bone_index));
						
						// Update current pose of leg bone
						auto transform = pose_a->get_relative_transform(bone_index);
						const auto rotation_a = pose_a->get_relative_transform(bone_index).rotation;
						const auto rotation_b = pose_b->get_relative_transform(bone_index).rotation;
						transform.rotation = math::nlerp(rotation_a, rotation_b, t);
						pose_component.current_pose.set_relative_transform(bone_index, transform);
					}
				}
				
				pose_component.current_pose.update();
			}
			
			// Apply locomotive force
			//auto& body = *(legged_group.get<rigid_body_component>(entity_id).body);
			//body.apply_central_force(locomotion.force);
		}
	);
}

void locomotion_system::update_winged(float t, float dt)
{
	auto winged_group = registry.group<winged_locomotion_component>(entt::get<rigid_body_component>);
	std::for_each
	(
		std::execution::par_unseq,
		winged_group.begin(),
		winged_group.end(),
		[&](auto entity_id)
		{
			const auto& locomotion = winged_group.get<winged_locomotion_component>(entity_id);
			auto& body = *(winged_group.get<rigid_body_component>(entity_id).body);
			
			const math::fvec3 gravity{0.0f, 9.80665f * 10.0f, 0.0f};
			//const math::fvec3 gravity{0.0f, 0.0f, 0.0f};
			
			// Apply locomotive force
			body.apply_central_force(locomotion.force + gravity * body.get_mass());
		}
	);
}
