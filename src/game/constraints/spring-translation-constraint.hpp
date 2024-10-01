// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_SPRING_TRANSLATION_CONSTRAINT_HPP
#define ANTKEEPER_GAME_SPRING_TRANSLATION_CONSTRAINT_HPP

#include <engine/physics/spring.hpp>
#include <engine/math/vector.hpp>

struct spring_translation_constraint
{
	/// Translation spring.
	physics::numeric_spring<math::fvec3, float> spring;
};

#endif // ANTKEEPER_GAME_SPRING_TRANSLATION_CONSTRAINT_HPP
