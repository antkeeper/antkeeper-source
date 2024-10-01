// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_SOLID_ANGLE_HPP
#define ANTKEEPER_GEOM_SOLID_ANGLE_HPP

#include <engine/math/constants.hpp>
#include <cmath>

namespace geom {

/// Solid angle functions.
namespace solid_angle {

/// Calculates the solid angle of a cone.
/// @tparam T Real type.
/// @param theta Apex angle of the cone, in radians.
/// @return Solid angle of the cone, in steradians.
template <class T>
[[nodiscard]] T cone(T theta)
{
	return math::two_pi<T> * (T(1) - std::cos(theta));
}

} // namespace solid_angle
} // namespace geom

#endif // ANTKEEPER_GEOM_SOLID_ANGLE_HPP
