// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_GRAVITY_COMPONENT_HPP
#define ANTKEEPER_GAME_GRAVITY_COMPONENT_HPP

import engine.math.vector;

using namespace engine;

struct gravity_component
{
	/// Gravitational force vector.
	math::fvec3 force{};
};

#endif // ANTKEEPER_GAME_GRAVITY_COMPONENT_HPP
