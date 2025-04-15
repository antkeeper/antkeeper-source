// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_WINGED_LOCOMOTION_COMPONENT_HPP
#define ANTKEEPER_GAME_WINGED_LOCOMOTION_COMPONENT_HPP

import engine.math.vector;

using namespace engine;

/// Winged aerial locomotion.
struct winged_locomotion_component
{
	/// Force vector.
	math::fvec3 force{0.0f, 0.0f, 0.0f};
};

#endif // ANTKEEPER_GAME_WINGED_LOCOMOTION_COMPONENT_HPP
