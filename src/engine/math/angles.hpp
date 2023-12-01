// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_ANGLES_HPP
#define ANTKEEPER_MATH_ANGLES_HPP

#include <engine/math/numbers.hpp>
#include <cmath>

namespace math {

/**
 * Converts an angle from radians to degrees.
 *
 * @param radians Angle in radians.
 *
 * @return Angle in degrees.
 */
template <class T>
[[nodiscard]] inline constexpr T degrees(T radians) noexcept
{
	return radians * rad2deg<T>;
}

/**
 * Converts an angle given in degrees to radians.
 *
 * @param radians Angle in radians.
 * @return Angle in degrees.
 */
template <class T>
[[nodiscard]] inline constexpr T radians(T degrees) noexcept
{
	return degrees * deg2rad<T>;
}

/**
 * Wraps an angle to [-180, 180].
 *
 * @param degrees Angle in degrees.
 * @return Wrapped angle.
 */
template <class T>
[[nodiscard]] inline constexpr T wrap_degrees(T degrees)
{
	return std::remainder(degrees, T(360));
}

/**
 * Wraps an angle to [-pi, pi].
 *
 * @param radians Angle in radians.
 * @return Wrapped angle.
 */
template <class T>
[[nodiscard]] inline constexpr T wrap_radians(T radians)
{
	return std::remainder(radians, two_pi<T>);
}

} // namespace math

#endif // ANTKEEPER_MATH_ANGLES_HPP
