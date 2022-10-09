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

#ifndef ANTKEEPER_GAME_SYSTEM_CONSTRAINT_HPP
#define ANTKEEPER_GAME_SYSTEM_CONSTRAINT_HPP

#include "game/system/updatable.hpp"
#include "game/component/transform.hpp"
#include "game/component/constraint/constraint.hpp"
#include "entity/id.hpp"

namespace game {
namespace system {

/**
 * Applies constraint stacks to transform components.
 *
 * @see game::component::constraint_stack
 * @see game::component::constraint_stack_node
 * @see game::component::constraint
 */
class constraint:
	public updatable
{
public:
	constraint(entity::registry& registry);
	~constraint();
	
	virtual void update(double t, double dt);
	
private:
	void on_constraint_stack_update(entity::registry& registry, entity::id constraint_stack_eid);
	
	void handle_constraint(component::transform& transform, entity::id constraint_eid, float dt);
	void handle_child_of_constraint(component::transform& transform, const component::constraint::child_of& constraint);
	void handle_copy_rotation_constraint(component::transform& transform, const component::constraint::copy_rotation& constraint);
	void handle_copy_scale_constraint(component::transform& transform, const component::constraint::copy_scale& constraint);
	void handle_copy_transform_constraint(component::transform& transform, const component::constraint::copy_transform& constraint);
	void handle_copy_translation_constraint(component::transform& transform, const component::constraint::copy_translation& constraint);
	void handle_pivot_constraint(component::transform& transform, const component::constraint::pivot& constraint);
	void handle_spring_rotation_constraint(component::transform& transform, component::constraint::spring_rotation& constraint, float dt);
	void handle_spring_to_constraint(component::transform& transform, component::constraint::spring_to& constraint, float dt);
	void handle_spring_translation_constraint(component::transform& transform, component::constraint::spring_translation& constraint, float dt);
	void handle_three_dof_constraint(component::transform& transform, const component::constraint::three_dof& constraint);
	void handle_track_to_constraint(component::transform& transform, const component::constraint::track_to& constraint);
};

} // namespace system
} // namespace game

#endif // ANTKEEPER_GAME_SYSTEM_CONSTRAINT_HPP
