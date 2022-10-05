/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "constraint.hpp"
#include "game/component/constraint-stack.hpp"
#include "game/component/constraints/constraints.hpp"
#include "game/component/transform.hpp"

namespace game {
namespace system {

constraint::constraint(entity::registry& registry):
	updatable(registry)
{}

void constraint::update(double t, double dt)
{
	// For each entity with transform and constraint stack components
	registry.view<component::transform, component::constraint_stack>().each(
		[&](entity::id transform_eid, auto& transform, auto& stack)
		{
			// Get entity ID of first constraint
			entity::id constraint_eid = stack.head;
			
			// Consecutively apply constraints
			while (constraint_eid != entt::null)
			{
				// Invalid constraint
				if (!registry.has<component::constraint_stack_node>(constraint_eid))
					break;
				
				// Get constraint stack node of this constraint
				const auto& node = registry.get<component::constraint_stack_node>(constraint_eid);
				
				// Apply constraint if enabled
				if (node.active)
					handle_constraint(transform, constraint_eid, static_cast<float>(dt));
				
				// Get entity ID of next constraint
				constraint_eid = node.next;
			}
		});
}

void constraint::handle_constraint(component::transform& transform, entity::id constraint_eid, float dt)
{
	if (registry.has<component::constraint::copy_translation>(constraint_eid))
		handle_copy_translation_constraint(transform, constraint_eid);
	else if (registry.has<component::constraint::copy_rotation>(constraint_eid))
		handle_copy_rotation_constraint(transform, constraint_eid);
	else if (registry.has<component::constraint::copy_scale>(constraint_eid))
		handle_copy_scale_constraint(transform, constraint_eid);
	else if (registry.has<component::constraint::copy_transform>(constraint_eid))
		handle_copy_transform_constraint(transform, constraint_eid);
	else if (registry.has<component::constraint::track_to>(constraint_eid))
		handle_track_to_constraint(transform, constraint_eid);
	else if (registry.has<component::constraint::three_dof>(constraint_eid))
		handle_three_dof_constraint(transform, constraint_eid);
	else if (registry.has<component::constraint::spring_to>(constraint_eid))
		handle_spring_to_constraint(transform, constraint_eid, dt);
}

void constraint::handle_copy_translation_constraint(component::transform& transform, entity::id constraint_eid)
{
	const auto& params = registry.get<component::constraint::copy_translation>(constraint_eid);
	
	if (this->registry.has<component::transform>(params.target))
	{
		const auto& target_translation = registry.get<component::transform>(params.target).world.translation;
		
		if (params.offset)
		{
			if (params.copy_x)
				transform.world.translation.x += (params.invert_x) ? -target_translation.x : target_translation.x;
			if (params.copy_y)
				transform.world.translation.y += (params.invert_y) ? -target_translation.y : target_translation.y;
			if (params.copy_z)
				transform.world.translation.z += (params.invert_z) ? -target_translation.z : target_translation.z;
		}
		else
		{
			if (params.copy_x)
				transform.world.translation.x = (params.invert_x) ? -target_translation.x : target_translation.x;
			if (params.copy_y)
				transform.world.translation.y = (params.invert_y) ? -target_translation.y : target_translation.y;
			if (params.copy_z)
				transform.world.translation.z = (params.invert_z) ? -target_translation.z : target_translation.z;
		}
		
	}
}

void constraint::handle_copy_rotation_constraint(component::transform& transform, entity::id constraint_eid)
{
	const auto& params = registry.get<component::constraint::copy_rotation>(constraint_eid);
	
	if (this->registry.has<component::transform>(params.target))
	{
		const auto& target_rotation = registry.get<component::transform>(params.target).world.rotation;
		transform.world.rotation = target_rotation;
	}
}

void constraint::handle_copy_scale_constraint(component::transform& transform, entity::id constraint_eid)
{
	const auto& params = registry.get<component::constraint::copy_scale>(constraint_eid);
	
	if (this->registry.has<component::transform>(params.target))
	{
		const auto& target_scale = registry.get<component::transform>(params.target).world.scale;
		
		if (params.copy_x)
			transform.world.scale.x = target_scale.x;
		if (params.copy_y)
			transform.world.scale.y = target_scale.y;
		if (params.copy_z)
			transform.world.scale.z = target_scale.z;
	}
}

void constraint::handle_copy_transform_constraint(component::transform& transform, entity::id constraint_eid)
{
	const auto& params = registry.get<component::constraint::copy_transform>(constraint_eid);
	
	if (this->registry.has<component::transform>(params.target))
	{
		const auto& target_transform = registry.get<component::transform>(params.target).world;
		transform.world = target_transform;
	}
}

void constraint::handle_track_to_constraint(component::transform& transform, entity::id constraint_eid)
{
	const auto& params = registry.get<component::constraint::track_to>(constraint_eid);
	
	if (this->registry.has<component::transform>(params.target))
	{
		const auto& target_transform = registry.get<component::transform>(params.target).world;
		transform.world.rotation = math::look_rotation(math::normalize(math::sub(target_transform.translation, transform.world.translation)), params.up);
	}
}

void constraint::handle_three_dof_constraint(component::transform& transform, entity::id constraint_eid)
{
	const auto& params = registry.get<component::constraint::three_dof>(constraint_eid);
	
	const math::quaternion<float> yaw = math::angle_axis(params.yaw, {0.0f, 1.0f, 0.0f});
	const math::quaternion<float> pitch = math::angle_axis(params.pitch, {-1.0f, 0.0f, 0.0f});
	const math::quaternion<float> roll = math::angle_axis(params.roll, {0.0f, 0.0f, -1.0f});
	transform.local.rotation = math::normalize(yaw * pitch * roll);
}

void constraint::handle_spring_to_constraint(component::transform& transform, entity::id constraint_eid, float dt)
{
	auto& params = registry.get<component::constraint::spring_to>(constraint_eid);
	
	if (this->registry.has<component::transform>(params.target))
	{
		// Get transform of target entity
		const auto& target_transform = registry.get<component::transform>(params.target).world;
		
		// Spring translation
		if (params.spring_translation)
		{
			// Update translation spring target
			params.translation.x1 = target_transform.translation;
			
			// Solve translation spring
			solve_numeric_spring<float3, float>(params.translation, dt);
			
			// Update transform translation
			transform.world.translation = params.translation.x0;
		}
		
		// Spring rotation
		if (params.spring_rotation)
		{
			// Update rotation spring target
			params.rotation.x1 = {target_transform.rotation.w, target_transform.rotation.x, target_transform.rotation.y, target_transform.rotation.z};
			
			// Solve rotation spring
			solve_numeric_spring<float4, float>(params.rotation, dt);
			
			// Update transform rotation
			transform.world.rotation = math::normalize(math::quaternion<float>{params.rotation.x0[0], params.rotation.x0[1], params.rotation.x0[2], params.rotation.x0[3]});
		}
	}
}

} // namespace system
} // namespace game
