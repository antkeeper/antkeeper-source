// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
