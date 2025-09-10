// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/constants.hpp>
#include <engine/math/functions.hpp>

/// Solid angle functions.
namespace engine::geom::solid_angle
{
	/// Calculates the solid angle of a cone.
	/// @tparam T Real type.
	/// @param theta Apex angle of the cone, in radians.
	/// @return Solid angle of the cone, in steradians.
	template <class T>
	[[nodiscard]] T cone(T theta)
	{
		return math::two_pi<T> * (T{1} - math::cos(theta));
	}
}
