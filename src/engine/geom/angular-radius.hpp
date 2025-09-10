// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/functions.hpp>

namespace engine::geom
{
	/// Finds the angular radius of a celestial object, given its radius and distance.
	/// @param radius Radius of the celestial object.
	/// @param distance Distance to the celestial object.
	/// @return Angular radius, in radians.
	template <class T>
	[[nodiscard]] inline T angular_radius(T radius, T distance)
	{
		return math::asin(radius / distance);
	}
}
