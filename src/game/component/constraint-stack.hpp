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

#ifndef ANTKEEPER_GAME_COMPONENT_CONSTRAINT_STACK_HPP
#define ANTKEEPER_GAME_COMPONENT_CONSTRAINT_STACK_HPP

#include "entity/id.hpp"

namespace game {
namespace component {

/**
 * Causes an ordered stack of constraints to be applied to an entity.
 *
 * @see game::component::constraint_stack_node
 * @see game::component::constraint
 */
struct constraint_stack
{
	/// ID of the entity containing the first constraint stack node.
	entity::id head;
};

/**
 * Single node in a constraint stack. Allows toggling and weighing of constraints and links to the following constraint stack node (if any).
 *
 * @see game::component::constraint_stack
 * @see game::component::constraint
 */
struct constraint_stack_node
{
	/// Enables or disables the constraint.
	bool active;
	
	/// Controls the amount of influence the constraint has on the final result.
	float weight;
	
	/// ID of the entity containing the next constraint in the constraint stack.
	entity::id next;
};

} // namespace component
} // namespace game

#endif // ANTKEEPER_GAME_COMPONENT_CONSTRAINT_STACK_HPP
