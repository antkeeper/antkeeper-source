// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_SPRING_TO_CONSTRAINT_HPP
#define ANTKEEPER_GAME_SPRING_TO_CONSTRAINT_HPP

#include <engine/physics/spring.hpp>
#include <engine/math/vector.hpp>
#include <engine/entity/id.hpp>

using namespace engine;

/// Springs to a target entity.
struct spring_to_constraint
{
	/// Target entity ID.
	entity::id target;
	
	/// Translation spring.
	physics::numeric_spring<math::fvec3, float> translation;
	
	/// Rotation spring.
	physics::numeric_spring<math::fvec4, float> rotation;
	
	/// Spring translation.
	bool spring_translation;
	
	/// Spring rotation.
	bool spring_rotation;
};

#endif // ANTKEEPER_GAME_SPRING_TO_CONSTRAINT_HPP
