// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_COPY_TRANSFORM_CONSTRAINT_HPP
#define ANTKEEPER_GAME_COPY_TRANSFORM_CONSTRAINT_HPP

import engine.entity.id;

using namespace engine;

/// Copies the transform of a target entity.
struct copy_transform_constraint
{
	/// Target entity ID.
	entity::id target;
};

#endif // ANTKEEPER_GAME_COPY_TRANSFORM_CONSTRAINT_HPP
