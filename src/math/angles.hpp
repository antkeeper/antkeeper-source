/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "math/constants.hpp"
#include <cmath>

namespace math {

/// @addtogroup utility
/// @{

/**
 * Converts an angle given in radians to degrees.
 *
 * @param radians Angle in radians.
 * @return Angle in degrees.
 */
template <class T>
T degrees(T radians);

/**
 * Converts an angle given in degrees to radians.
 *
 * @param radians Angle in radians.
 * @return Angle in degrees.
 */
template <class T>
T radians(T degrees);

/**
 * Wraps an angle to [-180, 180].
 *
 * @param degrees Angle in degrees.
 * @return Wrapped angle.
 */
template <class T>
T wrap_degrees(T degrees);

/**
 * Wraps an angle to [-pi, pi].
 *
 * @param radians Angle in radians.
 * @return Wrapped angle.
 */
template <class T>
T wrap_radians(T radians);

template <class T>
inline T degrees(T radians)
{
	return radians * T(180) / pi<T>;
}

template <class T>
inline T radians(T degrees)
{
	return degrees * pi<T> / T(180);
}

template <class T>
inline T wrap_degrees(T degrees)
{
	return std::remainder(degrees, T(360));
}

template <class T>
inline T wrap_radians(T radians)
{
	return std::remainder(radians, two_pi<T>);
}

/// @}

} // namespace math

#endif // ANTKEEPER_MATH_ANGLES_HPP
