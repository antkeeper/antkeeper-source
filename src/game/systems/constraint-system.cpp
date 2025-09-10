// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <entt/entt.hpp>
#include "game/systems/constraint-system.hpp"
#include "game/components/constraint-stack-component.hpp"
#include <engine/math/quaternion.hpp>

constraint_system::constraint_system(entity::registry& registry):
	m_registry(registry)
{
	m_registry.on_construct<constraint_stack_component>().connect<&constraint_system::on_constraint_stack_update>(this);
	m_registry.on_update<constraint_stack_component>().connect<&constraint_system::on_constraint_stack_update>(this);
	m_registry.on_destroy<constraint_stack_component>().connect<&constraint_system::on_constraint_stack_update>(this);
}

constraint_system::~constraint_system()
{
	m_registry.on_construct<constraint_stack_component>().disconnect<&constraint_system::on_constraint_stack_update>(this);
	m_registry.on_update<constraint_stack_component>().disconnect<&constraint_system::on_constraint_stack_update>(this);
	m_registry.on_destroy<constraint_stack_component>().disconnect<&constraint_system::on_constraint_stack_update>(this);
}

void constraint_system::fixed_update(entity::registry& registry, float, float dt)
{
	// For each entity with transform and constraint stack components
	registry.view<transform_component, constraint_stack_component>().each
	(
		[&](entity::id, auto& transform, auto& stack)
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
					handle_constraint(transform, constraint_eid, dt);
				
				// Get entity ID of next constraint in the stack
				constraint_eid = node->next;
			}
		}
	);
}

void constraint_system::evaluate(entity::id entity_id)
{
	if (!m_registry.valid(entity_id))
		return;
	
	// Get transform and constraint stack components of the entity
	const auto [transform, stack] = m_registry.try_get<transform_component, constraint_stack_component>(entity_id);
	
	if (!transform || !stack)
		return;
	
	// Init world-space transform
	transform->world = transform->local;
	
	// Get entity ID of first constraint
	entity::id constraint_eid = stack->head;
	
	// Consecutively apply constraints
	while (m_registry.valid(constraint_eid))
	{
		// Get constraint stack node of the constraint
		const constraint_stack_node_component* node = m_registry.try_get<constraint_stack_node_component>(constraint_eid);
		
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

void constraint_system::on_constraint_stack_update(entity::registry& registry, entity::id)
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
	if (auto copy_translation_constraint_ptr = m_registry.try_get<copy_translation_constraint>(constraint_eid))
		handle_copy_translation_constraint(transform, *copy_translation_constraint_ptr);
	else if (auto copy_rotation_constraint_ptr = m_registry.try_get<copy_rotation_constraint>(constraint_eid))
		handle_copy_rotation_constraint(transform, *copy_rotation_constraint_ptr);
	else if (auto copy_scale_constraint_ptr = m_registry.try_get<copy_scale_constraint>(constraint_eid))
		handle_copy_scale_constraint(transform, *copy_scale_constraint_ptr);
	else if (auto copy_transform_constraint_ptr = m_registry.try_get<copy_transform_constraint>(constraint_eid))
		handle_copy_transform_constraint(transform, *copy_transform_constraint_ptr);
	else if (auto track_to_constraint_ptr = m_registry.try_get<track_to_constraint>(constraint_eid))
		handle_track_to_constraint(transform, *track_to_constraint_ptr);
	else if (auto three_dof_constraint_ptr = m_registry.try_get<three_dof_constraint>(constraint_eid))
		handle_three_dof_constraint(transform, *three_dof_constraint_ptr);
	else if (auto pivot_constraint_ptr = m_registry.try_get<pivot_constraint>(constraint_eid))
		handle_pivot_constraint(transform, *pivot_constraint_ptr);
	else if (auto child_of_constraint_ptr = m_registry.try_get<child_of_constraint>(constraint_eid))
		handle_child_of_constraint(transform, *child_of_constraint_ptr);
	else if (auto spring_to_constraint_ptr = m_registry.try_get<spring_to_constraint>(constraint_eid))
		handle_spring_to_constraint(transform, *spring_to_constraint_ptr, dt);
	else if (auto spring_translation_constraint_ptr = m_registry.try_get<spring_translation_constraint>(constraint_eid))
		handle_spring_translation_constraint(transform, *spring_translation_constraint_ptr, dt);
	else if (auto spring_rotation_constraint_ptr = m_registry.try_get<spring_rotation_constraint>(constraint_eid))
		handle_spring_rotation_constraint(transform, *spring_rotation_constraint_ptr, dt);
	else if (auto ease_to_constraint_ptr = m_registry.try_get<ease_to_constraint>(constraint_eid))
		handle_ease_to_constraint(transform, *ease_to_constraint_ptr, dt);
}

void constraint_system::handle_child_of_constraint(transform_component& transform, const child_of_constraint& constraint)
{
	if (m_registry.valid(constraint.target))
	{
		const transform_component* target_transform = m_registry.try_get<transform_component>(constraint.target);
		if (target_transform)
		{
			transform.world = target_transform->world * transform.world;
		}
	}
}

void constraint_system::handle_copy_rotation_constraint(transform_component& transform, const copy_rotation_constraint& constraint)
{
	if (m_registry.valid(constraint.target))
	{
		const transform_component* target_transform = m_registry.try_get<transform_component>(constraint.target);
		if (target_transform)
		{
			transform.world.rotation = target_transform->world.rotation;
		}
	}
}

void constraint_system::handle_copy_scale_constraint(transform_component& transform, const copy_scale_constraint& constraint)
{
	if (m_registry.valid(constraint.target))
	{
		const transform_component* target_transform = m_registry.try_get<transform_component>(constraint.target);
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
	if (m_registry.valid(constraint.target))
	{
		const transform_component* target_transform = m_registry.try_get<transform_component>(constraint.target);
		if (target_transform)
		{
			transform.world = target_transform->world;
		}
	}
}

void constraint_system::handle_copy_translation_constraint(transform_component& transform, const copy_translation_constraint& constraint)
{
	if (m_registry.valid(constraint.target))
	{
		const transform_component* target_transform = m_registry.try_get<transform_component>(constraint.target);
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
	if (constraint.function && m_registry.valid(constraint.target))
	{
		const transform_component* target_transform = m_registry.try_get<transform_component>(constraint.target);
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
	if (m_registry.valid(constraint.target))
	{
		const transform_component* target_transform = m_registry.try_get<transform_component>(constraint.target);
		if (target_transform)
		{
			// Get pivot center point
			const math::fvec3 pivot_center = target_transform->world.translation + constraint.offset;
			
			// Pivot translation
			transform.world.translation = pivot_center + transform.world.rotation * (transform.world.translation - pivot_center);
		}
	}
}

void constraint_system::handle_spring_rotation_constraint(transform_component& transform, spring_rotation_constraint& constraint, float dt)
{
	// Solve yaw, pitch, and roll angle spring
	constraint.spring.solve(dt);
	
	// Build yaw, pitch, and roll quaternions
	const math::fquat yaw = math::angle_axis(constraint.spring.get_value()[0], {0.0f, 1.0f, 0.0f});
	const math::fquat pitch = math::angle_axis(constraint.spring.get_value()[1], {-1.0f, 0.0f, 0.0f});
	const math::fquat roll = math::angle_axis(constraint.spring.get_value()[2], {0.0f, 0.0f, -1.0f});
	
	// Update transform rotation
	transform.world.rotation = math::normalize(yaw * pitch * roll);
}

void constraint_system::handle_spring_to_constraint(transform_component& transform, spring_to_constraint& constraint, float dt)
{
	if (m_registry.valid(constraint.target))
	{
		const transform_component* target_transform = m_registry.try_get<transform_component>(constraint.target);
		if (target_transform)
		{
			// Spring translation
			if (constraint.spring_translation)
			{
				// Update translation spring target
				constraint.translation.set_target_value(target_transform->world.translation);
				
				// Solve translation spring
				constraint.translation.solve(dt);
				
				// Update transform translation
				transform.world.translation = constraint.translation.get_value();
			}
			
			// Spring rotation
			if (constraint.spring_rotation)
			{
				// Update rotation spring target
				const auto& r = target_transform->world.rotation;
				constraint.rotation.set_target_value(math::fvec4{r.w(), r.x(), r.y(), r.z()});
				
				// Solve rotation spring
				constraint.rotation.solve(dt);
				
				// Update transform rotation
				transform.world.rotation = math::normalize(math::fquat{constraint.rotation.get_value()[0], constraint.rotation.get_value()[1], constraint.rotation.get_value()[2], constraint.rotation.get_value()[3]});
			}
		}
	}
}

void constraint_system::handle_spring_translation_constraint(transform_component& transform, spring_translation_constraint& constraint, float dt)
{
	// Solve translation spring
	constraint.spring.solve(dt);
	
	// Update transform translation
	transform.world.translation = constraint.spring.get_value();
}

void constraint_system::handle_three_dof_constraint(transform_component& transform, const three_dof_constraint& constraint)
{
	const math::fquat yaw = math::angle_axis(constraint.yaw, {0.0f, 1.0f, 0.0f});
	const math::fquat pitch = math::angle_axis(constraint.pitch, {-1.0f, 0.0f, 0.0f});
	const math::fquat roll = math::angle_axis(constraint.roll, {0.0f, 0.0f, -1.0f});
	transform.world.rotation = math::normalize(yaw * pitch * roll);
}

void constraint_system::handle_track_to_constraint(transform_component& transform, const track_to_constraint& constraint)
{
	if (m_registry.valid(constraint.target))
	{
		const transform_component* target_transform = m_registry.try_get<transform_component>(constraint.target);
		if (target_transform)
		{
			transform.world.rotation = math::look_rotation(math::normalize(target_transform->world.translation - transform.world.translation), constraint.up);
		}
	}
}
