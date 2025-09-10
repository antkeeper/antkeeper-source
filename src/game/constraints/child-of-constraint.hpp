// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_CHILD_OF_CONSTRAINT_HPP
#define ANTKEEPER_GAME_CHILD_OF_CONSTRAINT_HPP

#include <engine/entity/id.hpp>

using namespace engine;

/// Makes the entity a child of the target entity.
struct child_of_constraint
{
	/// Target entity ID.
	entity::id target;
};

#endif // ANTKEEPER_GAME_CHILD_OF_CONSTRAINT_HPP
