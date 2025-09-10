// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_PIVOT_CONSTRAINT_HPP
#define ANTKEEPER_GAME_PIVOT_CONSTRAINT_HPP

#include <engine/math/vector.hpp>
#include <engine/entity/id.hpp>

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
