// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/locomotion-system.hpp"
#include "game/components/pose-component.hpp"
#include "game/components/legged-locomotion-component.hpp"
#include "game/components/winged-locomotion-component.hpp"
#include "game/components/navmesh-agent-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include <engine/entity/id.hpp>
#include <engine/animation/skeleton.hpp>
#include <engine/debug/log.hpp>
#include <engine/math/functions.hpp>
#include <engine/math/functions.hpp>
#include <engine/ai/navmesh.hpp>

void locomotion_system::fixed_update(entity::registry& registry, float t, float dt)
{
	update_legged(registry, t, dt);
	update_winged(registry, t, dt);
}

void locomotion_system::update_legged(entity::registry& registry, float, float dt)
{
	auto legged_group = registry.group<legged_locomotion_component>(entt::get<navmesh_agent_component, rigid_body_component, pose_component>);
	for (auto entity_id: legged_group)
	{
		auto& locomotion = legged_group.get<legged_locomotion_component>(entity_id);
		
		float cos_target_direction{};
		if (locomotion.speed != 0.0f)
		{
			auto& rigid_body = *legged_group.get<rigid_body_component>(entity_id).body;
			
			const auto max_steering_angle = locomotion.max_angular_frequency * dt;
			
			const auto current_direction = rigid_body.get_orientation() * math::fvec3{0, 0, 1};
			
			math::fquat steering_rotation;
			cos_target_direction = math::dot(current_direction, locomotion.target_direction);
			if (cos_target_direction < -0.999f)
			{
				steering_rotation = math::angle_axis(max_steering_angle, rigid_body.get_orientation() * math::fvec3{0, 1, 0});
			}
			else
			{
				steering_rotation = math::rotate_towards(current_direction, locomotion.target_direction, max_steering_angle);
			}
			
			rigid_body.set_orientation(math::normalize(steering_rotation * rigid_body.get_orientation()));
		}
		
		auto& navmesh_agent = legged_group.get<navmesh_agent_component>(entity_id);

		geom::brep_face** face = std::get_if<geom::brep_face*>(&navmesh_agent.feature);
		
		// Traverse navmesh
		if (locomotion.speed != 0.0f/* && cos_target_direction >= 0.0f*/ && face)
		{
			// Get agent rigid body
			auto& agent_rigid_body = *legged_group.get<rigid_body_component>(entity_id).body;
			const auto& agent_transform = agent_rigid_body.get_transform();
			
			// Get navmesh rigid body
			auto& navmesh_rigid_body = *registry.get<rigid_body_component>(navmesh_agent.navmesh_eid).body;
			const auto& navmesh_transform = navmesh_rigid_body.get_transform();
			
			// Determine start and end points of traversal
			const auto traversal_direction = agent_transform.rotation * math::fvec3{0, 0, 1};
			auto traversal_start = agent_transform.translation;
			auto traversal_end = agent_transform.translation + traversal_direction * (locomotion.speed * dt);
			
			// Transform traversal segment from world-space to navmesh-space
			traversal_start = ((traversal_start - navmesh_transform.translation) * navmesh_transform.rotation) / navmesh_transform.scale;
			traversal_end = ((traversal_end - navmesh_transform.translation) * navmesh_transform.rotation) / navmesh_transform.scale;
			
			// Traverse navmesh
			// NOTE: if the navmesh has a nonuniform scale, the traversal will be skewed
			auto traversal = ai::traverse_navmesh(*navmesh_agent.mesh, *face, traversal_start, traversal_end);
			
			// Transform traversal end point from navmesh-space world-space
			traversal.closest_point = navmesh_transform.translation + (navmesh_transform.rotation * (navmesh_transform.scale * traversal.closest_point));
			
			// Update navmesh agent face
			navmesh_agent.feature = traversal.feature;

			face = std::get_if<geom::brep_face*>(&navmesh_agent.feature);
			if (face)
			{
				// Interpolate navmesh vertex normals
				const auto& vertex_normals = navmesh_agent.mesh->vertices().attributes().at<math::fvec3>("normal");
				auto loop = (*face)->loops().begin();
				const auto& na = vertex_normals[loop->vertex()->index()];
				const auto& nb = vertex_normals[(++loop)->vertex()->index()];
				const auto& nc = vertex_normals[(++loop)->vertex()->index()];
				const auto& uvw = traversal.barycentric;
				navmesh_agent.surface_normal = math::normalize(na * uvw.x() + nb * uvw.y() + nc * uvw.z());
				
				// Transform surface normal from navmesh-space to world-space
				navmesh_agent.surface_normal = math::normalize(navmesh_transform.rotation * (navmesh_agent.surface_normal / navmesh_transform.scale));
			}
			
			// const auto& face_normals = navmesh_agent.mesh->faces().attributes().at<math::fvec3>("normal");
			// navmesh_agent.surface_normal = face_normals[navmesh_agent.face->index()];
			
			// Update agent rigid body
			agent_rigid_body.set_position(traversal.closest_point);
			// agent_rigid_body.set_position(traversal_ray.extrapolate(locomotion.speed * dt));
			// agent_rigid_body.set_orientation(math::normalize(math::rotation(rigid_body_transform.rotation * math::fvec3{0, 1, 0}, navmesh_agent.surface_normal) * rigid_body_transform.rotation));
			agent_rigid_body.set_orientation(math::normalize(math::rotation(agent_transform.rotation * math::fvec3{0, 1, 0}, navmesh_agent.surface_normal) * agent_transform.rotation));
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
				const skeleton_pose* pose_a;
				const skeleton_pose* pose_b;
				float t;
				if (step_phase < 0.0f)
				{
					pose_b = locomotion.touchdown_pose.get();
					pose_a = locomotion.liftoff_pose.get();
					t = std::abs(step_phase);
				}
				else
				{
					if (step_phase < 0.5f)
					{
						pose_a = locomotion.liftoff_pose.get();
						pose_b = locomotion.midswing_pose.get();
						t = step_phase * 2.0f;
					}
					else
					{
						pose_a = locomotion.midswing_pose.get();
						pose_b = locomotion.touchdown_pose.get();
						t = (step_phase - 0.5f) * 2.0f;
					}
				}
				
				// Update leg bones
				auto bone_index = locomotion.tip_bones[i];
				for (std::uint8_t j = 0; j < locomotion.leg_bone_count; ++j)
				{
					if (j)
					{
						bone_index = pose_component.current_pose.get_skeleton()->bones()[bone_index].parent()->index();
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
		}
		
		// Apply locomotive force
		//auto& body = *(legged_group.get<rigid_body_component>(entity_id).body);
		//body.apply_central_force(locomotion.force);
	}
}

void locomotion_system::update_winged(entity::registry& registry, float, float)
{
	auto winged_group = registry.group<winged_locomotion_component>(entt::get<rigid_body_component>);
	for (auto entity_id: winged_group)
	{
		const auto& locomotion = winged_group.get<winged_locomotion_component>(entity_id);
		auto& body = *(winged_group.get<rigid_body_component>(entity_id).body);
		
		const math::fvec3 gravity{0.0f, 9.80665f * 10.0f, 0.0f};
		//const math::fvec3 gravity{0.0f, 0.0f, 0.0f};
		
		// Apply locomotive force
		body.apply_central_force(locomotion.force + gravity * body.get_mass());
	}
}
