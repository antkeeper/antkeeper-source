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

#ifndef ANTKEEPER_GAME_CONSTRAINT_STACK_COMPONENT_HPP
#define ANTKEEPER_GAME_CONSTRAINT_STACK_COMPONENT_HPP

#include <engine/entity/id.hpp>


/**
 * Causes an ordered stack of constraints to be applied to an entity.
 */
struct constraint_stack_component
{
	/// Priority number, with lower priorities evaluated first.
	int priority;
	
	/// ID of the entity containing the first constraint stack node.
	entity::id head;
};

/**
 * Single node in a constraint stack. Allows toggling and weighing of constraints and links to the following constraint stack node (if any).
 */
struct constraint_stack_node_component
{
	/// Enables or disables the constraint.
	bool active;
	
	/// Controls the amount of influence the constraint has on the final result.
	float weight;
	
	/// ID of the entity containing the next constraint in the constraint stack.
	entity::id next;
};


#endif // ANTKEEPER_GAME_CONSTRAINT_STACK_COMPONENT_HPP
