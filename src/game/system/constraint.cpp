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

#include "game/system/constraint.hpp"
#include "game/component/constraint-stack.hpp"
#include "math/quaternion-operators.hpp"
#include "math/transform-operators.hpp"

namespace game {
namespace system {

constraint::constraint(entity::registry& registry):
	updatable(registry)
{
	registry.on_construct<component::constraint_stack>().connect<&constraint::on_constraint_stack_update>(this);
	registry.on_update<component::constraint_stack>().connect<&constraint::on_constraint_stack_update>(this);
	registry.on_destroy<component::constraint_stack>().connect<&constraint::on_constraint_stack_update>(this);
}

constraint::~constraint()
{
	registry.on_construct<component::constraint_stack>().disconnect<&constraint::on_constraint_stack_update>(this);
	registry.on_update<component::constraint_stack>().disconnect<&constraint::on_constraint_stack_update>(this);
	registry.on_destroy<component::constraint_stack>().disconnect<&constraint::on_constraint_stack_update>(this);
}

void constraint::update(double t, double dt)
{
	// For each entity with transform and constraint stack components
	registry.view<component::transform, component::constraint_stack>().each
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
				const component::constraint_stack_node* node = registry.try_get<component::constraint_stack_node>(constraint_eid);
				
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

void constraint::evaluate(entity::id entity_id)
{
	if (!registry.valid(entity_id))
		return;
	
	// Get transform and constraint stack components of the entity
	const auto [transform, stack] = registry.try_get<component::transform, component::constraint_stack>(entity_id);
	
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
		const component::constraint_stack_node* node = registry.try_get<component::constraint_stack_node>(constraint_eid);
		
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

void constraint::on_constraint_stack_update(entity::registry& registry, entity::id constraint_stack_eid)
{
	registry.sort<component::constraint_stack>
	(
		[](const auto& lhs, const auto& rhs)
		{
			return lhs.priority < rhs.priority;
		}
	);
}

void constraint::handle_constraint(component::transform& transform, entity::id constraint_eid, float dt)
{
	if (auto constraint = registry.try_get<component::constraint::copy_translation>(constraint_eid); constraint)
		handle_copy_translation_constraint(transform, *constraint);
	else if (auto constraint = registry.try_get<component::constraint::copy_rotation>(constraint_eid); constraint)
		handle_copy_rotation_constraint(transform, *constraint);
	else if (auto constraint = registry.try_get<component::constraint::copy_scale>(constraint_eid); constraint)
		handle_copy_scale_constraint(transform, *constraint);
	else if (auto constraint = registry.try_get<component::constraint::copy_transform>(constraint_eid); constraint)
		handle_copy_transform_constraint(transform, *constraint);
	else if (auto constraint = registry.try_get<component::constraint::track_to>(constraint_eid); constraint)
		handle_track_to_constraint(transform, *constraint);
	else if (auto constraint = registry.try_get<component::constraint::three_dof>(constraint_eid); constraint)
		handle_three_dof_constraint(transform, *constraint);
	else if (auto constraint = registry.try_get<component::constraint::pivot>(constraint_eid); constraint)
		handle_pivot_constraint(transform, *constraint);
	else if (auto constraint = registry.try_get<component::constraint::child_of>(constraint_eid); constraint)
		handle_child_of_constraint(transform, *constraint);
	else if (auto constraint = registry.try_get<component::constraint::spring_to>(constraint_eid); constraint)
		handle_spring_to_constraint(transform, *constraint, dt);
	else if (auto constraint = registry.try_get<component::constraint::spring_translation>(constraint_eid); constraint)
		handle_spring_translation_constraint(transform, *constraint, dt);
	else if (auto constraint = registry.try_get<component::constraint::spring_rotation>(constraint_eid); constraint)
		handle_spring_rotation_constraint(transform, *constraint, dt);
	else if (auto constraint = registry.try_get<component::constraint::ease_to>(constraint_eid); constraint)
		handle_ease_to_constraint(transform, *constraint, dt);
}

void constraint::handle_child_of_constraint(component::transform& transform, const component::constraint::child_of& constraint)
{
	if (registry.valid(constraint.target))
	{
		const component::transform* target_transform = registry.try_get<component::transform>(constraint.target);
		if (target_transform)
		{
			transform.world = target_transform->world * transform.world;
		}
	}
}

void constraint::handle_copy_rotation_constraint(component::transform& transform, const component::constraint::copy_rotation& constraint)
{
	if (registry.valid(constraint.target))
	{
		const component::transform* target_transform = registry.try_get<component::transform>(constraint.target);
		if (target_transform)
		{
			transform.world.rotation = target_transform->world.rotation;
		}
	}
}

void constraint::handle_copy_scale_constraint(component::transform& transform, const component::constraint::copy_scale& constraint)
{
	if (registry.valid(constraint.target))
	{
		const component::transform* target_transform = registry.try_get<component::transform>(constraint.target);
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

void constraint::handle_copy_transform_constraint(component::transform& transform, const component::constraint::copy_transform& constraint)
{
	if (registry.valid(constraint.target))
	{
		const component::transform* target_transform = registry.try_get<component::transform>(constraint.target);
		if (target_transform)
		{
			transform.world = target_transform->world;
		}
	}
}

void constraint::handle_copy_translation_constraint(component::transform& transform, const component::constraint::copy_translation& constraint)
{
	if (registry.valid(constraint.target))
	{
		const component::transform* target_transform = registry.try_get<component::transform>(constraint.target);
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

void constraint::handle_ease_to_constraint(component::transform& transform, component::constraint::ease_to& constraint, float dt)
{
	if (constraint.function && registry.valid(constraint.target))
	{
		const component::transform* target_transform = registry.try_get<component::transform>(constraint.target);
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

void constraint::handle_pivot_constraint(component::transform& transform, const component::constraint::pivot& constraint)
{
	if (registry.valid(constraint.target))
	{
		const component::transform* target_transform = registry.try_get<component::transform>(constraint.target);
		if (target_transform)
		{
			// Get pivot center point
			const float3 pivot_center = target_transform->world.translation + constraint.offset;
			
			// Pivot translation
			transform.world.translation = pivot_center + transform.world.rotation * (transform.world.translation - pivot_center);
		}
	}
}

void constraint::handle_spring_rotation_constraint(component::transform& transform, component::constraint::spring_rotation& constraint, float dt)
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

void constraint::handle_spring_to_constraint(component::transform& transform, component::constraint::spring_to& constraint, float dt)
{
	if (registry.valid(constraint.target))
	{
		const component::transform* target_transform = registry.try_get<component::transform>(constraint.target);
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
				constraint.rotation.x1 =
				{
					target_transform->world.rotation.w,
					target_transform->world.rotation.x,
					target_transform->world.rotation.y,
					target_transform->world.rotation.z
				};
				
				// Solve rotation spring
				solve_numeric_spring<float4, float>(constraint.rotation, dt);
				
				// Update transform rotation
				transform.world.rotation = math::normalize(math::quaternion<float>{constraint.rotation.x0[0], constraint.rotation.x0[1], constraint.rotation.x0[2], constraint.rotation.x0[3]});
			}
		}
	}
}

void constraint::handle_spring_translation_constraint(component::transform& transform, component::constraint::spring_translation& constraint, float dt)
{
	// Solve translation spring
	solve_numeric_spring<float3, float>(constraint.spring, dt);
	
	// Update transform translation
	transform.world.translation = constraint.spring.x0;
}

void constraint::handle_three_dof_constraint(component::transform& transform, const component::constraint::three_dof& constraint)
{
	const math::quaternion<float> yaw = math::angle_axis(constraint.yaw, {0.0f, 1.0f, 0.0f});
	const math::quaternion<float> pitch = math::angle_axis(constraint.pitch, {-1.0f, 0.0f, 0.0f});
	const math::quaternion<float> roll = math::angle_axis(constraint.roll, {0.0f, 0.0f, -1.0f});
	transform.world.rotation = math::normalize(yaw * pitch * roll);
}

void constraint::handle_track_to_constraint(component::transform& transform, const component::constraint::track_to& constraint)
{
	if (registry.valid(constraint.target))
	{
		const component::transform* target_transform = registry.try_get<component::transform>(constraint.target);
		if (target_transform)
		{
			transform.world.rotation = math::look_rotation(math::normalize(math::sub(target_transform->world.translation, transform.world.translation)), constraint.up);
		}
	}
}

} // namespace system
} // namespace game
