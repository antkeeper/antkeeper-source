// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_EASE_TO_CONSTRAINT_HPP
#define ANTKEEPER_GAME_EASE_TO_CONSTRAINT_HPP

#include <engine/entity/id.hpp>
#include <engine/math/vector.hpp>


/**
 * Eases toward a target entity.
 */
struct ease_to_constraint
{
	/// Target entity ID.
	entity::id target;
	
	/// Start position.
	math::fvec3 start;
	
	/// Total duration of the ease.
	float duration;
	
	/// Elapsed time since ease began.
	float t;
	
	/// Pointer to the interpolation function.
	math::fvec3 (*function)(const math::fvec3&, const math::fvec3&, float);
};


#endif // ANTKEEPER_GAME_EASE_TO_CONSTRAINT_HPP
