/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

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
