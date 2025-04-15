// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_PIVOT_CONSTRAINT_HPP
#define ANTKEEPER_GAME_PIVOT_CONSTRAINT_HPP

import engine.math.vector;
import engine.entity.id;

using namespace engine;

/// Pivots around a target entity.
struct pivot_constraint
{
	/// Target entity ID.
	entity::id target;
	
	/// Pivot point offset.
	math::fvec3 offset;
};

#endif // ANTKEEPER_GAME_PIVOT_CONSTRAINT_HPP
