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

#ifndef ANTKEEPER_ENTITY_SYSTEM_CONSTRAINT_HPP
#define ANTKEEPER_ENTITY_SYSTEM_CONSTRAINT_HPP

#include "entity/systems/updatable.hpp"
#include "entity/components/transform.hpp"
#include "entity/id.hpp"

namespace entity {
namespace system {

/**
 * Applies constraint stacks to transform components.
 *
 * @see entity::component::constraint_stack
 * @see entity::component::constraint_stack_node
 * @see entity::component::constraint
 */
class constraint:
	public updatable
{
public:
	constraint(entity::registry& registry);
	virtual void update(double t, double dt);
	
private:
	void handle_constraint(component::transform& transform, entity::id constraint_eid, float dt);
	
	void handle_copy_translation_constraint(component::transform& transform, entity::id constraint_eid);
	void handle_copy_rotation_constraint(component::transform& transform, entity::id constraint_eid);
	void handle_copy_scale_constraint(component::transform& transform, entity::id constraint_eid);
	void handle_copy_transform_constraint(component::transform& transform, entity::id constraint_eid);
	void handle_track_to_constraint(component::transform& transform, entity::id constraint_eid);
	void handle_three_dof_constraint(component::transform& transform, entity::id constraint_eid);
	void handle_spring_to_constraint(component::transform& transform, entity::id constraint_eid, float dt);
};

} // namespace system
} // namespace entity

#endif // ANTKEEPER_ENTITY_SYSTEM_CONSTRAINT_HPP
