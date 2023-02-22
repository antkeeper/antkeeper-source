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

#include "game/systems/constraint-system.hpp"
#include "game/components/constraint-stack-component.hpp"
#include <engine/math/quaternion.hpp>
#include <engine/math/transform-operators.hpp>

constraint_system::constraint_system(entity::registry& registry):
	updatable_system(registry)
{
	registry.on_construct<constraint_stack_component>().connect<&constraint_system::on_constraint_stack_update>(this);
	registry.on_update<constraint_stack_component>().connect<&constraint_system::on_constraint_stack_update>(this);
	registry.on_destroy<constraint_stack_component>().connect<&constraint_system::on_constraint_stack_update>(this);
}

constraint_system::~constraint_system()
{
	registry.on_construct<constraint_stack_component>().disconnect<&constraint_system::on_constraint_stack_update>(this);
	registry.on_update<constraint_stack_component>().disconnect<&constraint_system::on_constraint_stack_update>(this);
	registry.on_destroy<constraint_stack_component>().disconnect<&constraint_system::on_constraint_stack_update>(this);
}

void constraint_system::update(double t, double dt)
{
	// For each entity with transform and constraint stack components
	registry.view<transform_component, constraint_stack_component>().each
	(
		[&](entity::id transform_eid, auto& transform, auto& stack)
		{
			// Init world-space transform
			transform.world = transform.local;
			
			// Get entity ID of first constraint
			entity::id constraint_eid = stack.head;
			
			// Consecutively apply constraints
			while (registry.valid(constraint_eid))
			{
				// Get constraint stack node of the constraint
				const constraint_stack_node_component* node = registry.try_get<constraint_stack_node_component>(constraint_eid);
				
				// Abort if constraint is missing a constraint stack node
				if (!node)
					break;
				
				// Apply constraint if enabled
				if (node->active)
					handle_constraint(transform, constraint_eid, static_cast<float>(dt));
				
				// Get entity ID of next constraint in the stack
				constraint_eid = node->next;
			}
		}
	);
}

void constraint_system::evaluate(entity::id entity_id)
{
	if (!registry.valid(entity_id))
		return;
	
	// Get transform and constraint stack components of the entity
	const auto [transform, stack] = registry.try_get<transform_component, constraint_stack_component>(entity_id);
	
	if (!transform || !stack)
		return;
	
	// Init world-space transform
	transform->world = transform->local;
	
	// Get entity ID of first constraint
	entity::id constraint_eid = stack->head;
	
	// Consecutively apply constraints
	while (registry.valid(constraint_eid))
	{
		// Get constraint stack node of the constraint
		const constraint_stack_node_component* node = registry.try_get<constraint_stack_node_component>(constraint_eid);
		
		// Abort if constraint is missing a constraint stack node
		if (!node)
			break;
		
		// Apply constraint if enabled
		if (node->active)
			handle_constraint(*transform, constraint_eid, 0.0f);
		
		// Get entity ID of next constraint in the stack
		constraint_eid = node->next;
	}
}

void constraint_system::on_constraint_stack_update(entity::registry& registry, entity::id constraint_stack_eid)
{
	registry.sort<constraint_stack_component>
	(
		[](const auto& lhs, const auto& rhs)
		{
			return lhs.priority < rhs.priority;
		}
	);
}

void constraint_system::handle_constraint(transform_component& transform, entity::id constraint_eid, float dt)
{
	if (auto constraint = registry.try_get<copy_translation_constraint>(constraint_eid); constraint)
		handle_copy_translation_constraint(transform, *constraint);
	else if (auto constraint = registry.try_get<copy_rotation_constraint>(constraint_eid); constraint)
		handle_copy_rotation_constraint(transform, *constraint);
	else if (auto constraint = registry.try_get<copy_scale_constraint>(constraint_eid); constraint)
		handle_copy_scale_constraint(transform, *constraint);
	else if (auto constraint = registry.try_get<copy_transform_constraint>(constraint_eid); constraint)
		handle_copy_transform_constraint(transform, *constraint);
	else if (auto constraint = registry.try_get<track_to_constraint>(constraint_eid); constraint)
		handle_track_to_constraint(transform, *constraint);
	else if (auto constraint = registry.try_get<three_dof_constraint>(constraint_eid); constraint)
		handle_three_dof_constraint(transform, *constraint);
	else if (auto constraint = registry.try_get<pivot_constraint>(constraint_eid); constraint)
		handle_pivot_constraint(transform, *constraint);
	else if (auto constraint = registry.try_get<child_of_constraint>(constraint_eid); constraint)
		handle_child_of_constraint(transform, *constraint);
	else if (auto constraint = registry.try_get<spring_to_constraint>(constraint_eid); constraint)
		handle_spring_to_constraint(transform, *constraint, dt);
	else if (auto constraint = registry.try_get<spring_translation_constraint>(constraint_eid); constraint)
		handle_spring_translation_constraint(transform, *constraint, dt);
	else if (auto constraint = registry.try_get<spring_rotation_constraint>(constraint_eid); constraint)
		handle_spring_rotation_constraint(transform, *constraint, dt);
	else if (auto constraint = registry.try_get<ease_to_constraint>(constraint_eid); constraint)
		handle_ease_to_constraint(transform, *constraint, dt);
}

void constraint_system::handle_child_of_constraint(transform_component& transform, const child_of_constraint& constraint)
{
	if (registry.valid(constraint.target))
	{
		const transform_component* target_transform = registry.try_get<transform_component>(constraint.target);
		if (target_transform)
		{
			transform.world = target_transform->world * transform.world;
		}
	}
}

void constraint_system::handle_copy_rotation_constraint(transform_component& transform, const copy_rotation_constraint& constraint)
{
	if (registry.valid(constraint.target))
	{
		const transform_component* target_transform = registry.try_get<transform_component>(constraint.target);
		if (target_transform)
		{
			transform.world.rotation = target_transform->world.rotation;
		}
	}
}

void constraint_system::handle_copy_scale_constraint(transform_component& transform, const copy_scale_constraint& constraint)
{
	if (registry.valid(constraint.target))
	{
		const transform_component* target_transform = registry.try_get<transform_component>(constraint.target);
		if (target_transform)
		{
			const auto& target_scale = target_transform->world.scale;
			
			if (constraint.copy_x)
				transform.world.scale.x() = target_scale.x();
			if (constraint.copy_y)
				transform.world.scale.y() = target_scale.y();
			if (constraint.copy_z)
				transform.world.scale.z() = target_scale.z();
		}
	}
}

void constraint_system::handle_copy_transform_constraint(transform_component& transform, const copy_transform_constraint& constraint)
{
	if (registry.valid(constraint.target))
	{
		const transform_component* target_transform = registry.try_get<transform_component>(constraint.target);
		if (target_transform)
		{
			transform.world = target_transform->world;
		}
	}
}

void constraint_system::handle_copy_translation_constraint(transform_component& transform, const copy_translation_constraint& constraint)
{
	if (registry.valid(constraint.target))
	{
		const transform_component* target_transform = registry.try_get<transform_component>(constraint.target);
		if (target_transform)
		{
			const auto& target_translation = target_transform->world.translation;
			
			if (constraint.offset)
			{
				if (constraint.copy_x)
					transform.world.translation.x() += (constraint.invert_x) ? -target_translation.x() : target_translation.x();
				if (constraint.copy_y)
					transform.world.translation.y() += (constraint.invert_y) ? -target_translation.y() : target_translation.y();
				if (constraint.copy_z)
					transform.world.translation.z() += (constraint.invert_z) ? -target_translation.z() : target_translation.z();
			}
			else
			{
				if (constraint.copy_x)
					transform.world.translation.x() = (constraint.invert_x) ? -target_translation.x() : target_translation.x();
				if (constraint.copy_y)
					transform.world.translation.y() = (constraint.invert_y) ? -target_translation.y() : target_translation.y();
				if (constraint.copy_z)
					transform.world.translation.z() = (constraint.invert_z) ? -target_translation.z() : target_translation.z();
			}
		}
	}
}

void constraint_system::handle_ease_to_constraint(transform_component& transform, ease_to_constraint& constraint, float dt)
{
	if (constraint.function && registry.valid(constraint.target))
	{
		const transform_component* target_transform = registry.try_get<transform_component>(constraint.target);
		if (target_transform)
		{
			if (constraint.t < constraint.duration)
			{
				const float a = constraint.t / constraint.duration;
				transform.world.translation = constraint.function(constraint.start, target_transform->world.translation, a);
			}
			else
			{
				transform.world.translation = target_transform->world.translation;
			}
			
			constraint.t += dt;
		}
	}
}

void constraint_system::handle_pivot_constraint(transform_component& transform, const pivot_constraint& constraint)
{
	if (registry.valid(constraint.target))
	{
		const transform_component* target_transform = registry.try_get<transform_component>(constraint.target);
		if (target_transform)
		{
			// Get pivot center point
			const float3 pivot_center = target_transform->world.translation + constraint.offset;
			
			// Pivot translation
			transform.world.translation = pivot_center + transform.world.rotation * (transform.world.translation - pivot_center);
		}
	}
}

void constraint_system::handle_spring_rotation_constraint(transform_component& transform, spring_rotation_constraint& constraint, float dt)
{
	// Solve yaw, pitch, and roll angle spring
	solve_numeric_spring<float3, float>(constraint.spring, dt);
	
	// Build yaw, pitch, and roll quaternions
	const math::quaternion<float> yaw = math::angle_axis(constraint.spring.x0[0], {0.0f, 1.0f, 0.0f});
	const math::quaternion<float> pitch = math::angle_axis(constraint.spring.x0[1], {-1.0f, 0.0f, 0.0f});
	const math::quaternion<float> roll = math::angle_axis(constraint.spring.x0[2], {0.0f, 0.0f, -1.0f});
	
	// Update transform rotation
	transform.world.rotation = math::normalize(yaw * pitch * roll);
}

void constraint_system::handle_spring_to_constraint(transform_component& transform, spring_to_constraint& constraint, float dt)
{
	if (registry.valid(constraint.target))
	{
		const transform_component* target_transform = registry.try_get<transform_component>(constraint.target);
		if (target_transform)
		{
			// Spring translation
			if (constraint.spring_translation)
			{
				// Update translation spring target
				constraint.translation.x1 = target_transform->world.translation;
				
				// Solve translation spring
				solve_numeric_spring<float3, float>(constraint.translation, dt);
				
				// Update transform translation
				transform.world.translation = constraint.translation.x0;
			}
			
			// Spring rotation
			if (constraint.spring_rotation)
			{
				// Update rotation spring target
				constraint.rotation.x1 = float4(target_transform->world.rotation);
				
				// Solve rotation spring
				solve_numeric_spring<float4, float>(constraint.rotation, dt);
				
				// Update transform rotation
				transform.world.rotation = math::normalize(math::quaternion<float>{constraint.rotation.x0[0], constraint.rotation.x0[1], constraint.rotation.x0[2], constraint.rotation.x0[3]});
			}
		}
	}
}

void constraint_system::handle_spring_translation_constraint(transform_component& transform, spring_translation_constraint& constraint, float dt)
{
	// Solve translation spring
	solve_numeric_spring<float3, float>(constraint.spring, dt);
	
	// Update transform translation
	transform.world.translation = constraint.spring.x0;
}

void constraint_system::handle_three_dof_constraint(transform_component& transform, const three_dof_constraint& constraint)
{
	const math::quaternion<float> yaw = math::angle_axis(constraint.yaw, {0.0f, 1.0f, 0.0f});
	const math::quaternion<float> pitch = math::angle_axis(constraint.pitch, {-1.0f, 0.0f, 0.0f});
	const math::quaternion<float> roll = math::angle_axis(constraint.roll, {0.0f, 0.0f, -1.0f});
	transform.world.rotation = math::normalize(yaw * pitch * roll);
}

void constraint_system::handle_track_to_constraint(transform_component& transform, const track_to_constraint& constraint)
{
	if (registry.valid(constraint.target))
	{
		const transform_component* target_transform = registry.try_get<transform_component>(constraint.target);
		if (target_transform)
		{
			transform.world.rotation = math::look_rotation(math::normalize(math::sub(target_transform->world.translation, transform.world.translation)), constraint.up);
		}
	}
}
