// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_WINGED_LOCOMOTION_COMPONENT_HPP
#define ANTKEEPER_GAME_WINGED_LOCOMOTION_COMPONENT_HPP

#include <engine/math/vector.hpp>

/**
 * Winged aerial locomotion.
 */
struct winged_locomotion_component
{
	/// Force vector.
	math::fvec3 force{0.0f, 0.0f, 0.0f};
};

#endif // ANTKEEPER_GAME_WINGED_LOCOMOTION_COMPONENT_HPP
