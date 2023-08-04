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
#include "game/components/legged-locomotion-component.hpp"
#include "game/components/winged-locomotion-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include "game/components/scene-component.hpp"
#include <engine/entity/id.hpp>
#include <engine/animation/skeleton.hpp>
#include <engine/debug/log.hpp>
#include <algorithm>
#include <execution>

locomotion_system::locomotion_system(entity::registry& registry):
	updatable_system(registry)
{}

void locomotion_system::update(float t, float dt)
{
	// Legged locomotion
	// auto legged_group = registry.group<legged_locomotion_component>(entt::get<rigid_body_component>);
	auto legged_group = registry.group<legged_locomotion_component>(entt::get<scene_component>);
	std::for_each
	(
		std::execution::par_unseq,
		legged_group.begin(),
		legged_group.end(),
		[&](auto entity_id)
		{
			const auto& locomotion = legged_group.get<legged_locomotion_component>(entity_id);
			if (!locomotion.moving)
			{
				return;
			}
			
			//auto& body = *(legged_group.get<rigid_body_component>(entity_id).body);
			
			// Apply locomotive force
			//body.apply_central_force(locomotion.force);
			
			// Animate legs
			if (locomotion.current_pose)
			{
				// Get gait phase
				float gait_phase = locomotion.gait->phase(t);
				
				// For each leg
				for (std::size_t i = 0; i < locomotion.tip_bones.size(); ++i)
				{
					// Get step phase
					float step_phase = locomotion.gait->steps[i].phase(gait_phase);
					
					// Determine leg pose
					const pose* pose_a;
					const pose* pose_b;
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
							bone_index = locomotion.current_pose->get_skeleton()->get_bone_parent(bone_index);
						}
						
						const auto rotation_a = pose_a->get_relative_transform(bone_index).rotation;
						const auto rotation_b = pose_b->get_relative_transform(bone_index).rotation;
						
						auto transform = pose_a->get_relative_transform(bone_index);
						transform.rotation = math::nlerp(rotation_a, rotation_b, t);
						
						locomotion.current_pose->set_relative_transform(bone_index, transform);
					}
					
					// Update subset of pose containing the leg bones
					locomotion.current_pose->update(bone_index, locomotion.leg_bone_count);
				}
			}
		}
	);
	
	// Winged locomotion
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
