// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_COPY_ROTATION_CONSTRAINT_HPP
#define ANTKEEPER_GAME_COPY_ROTATION_CONSTRAINT_HPP

#include <engine/entity/id.hpp>

/// Copies the rotation of a target entity.
struct copy_rotation_constraint
{
	/// Target entity ID.
	entity::id target;
};

#endif // ANTKEEPER_GAME_COPY_ROTATION_CONSTRAINT_HPP
