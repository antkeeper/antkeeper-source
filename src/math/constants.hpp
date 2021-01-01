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

#ifndef ANTKEEPER_MATH_CONSTANTS_HPP
#define ANTKEEPER_MATH_CONSTANTS_HPP

#include "math/matrix-type.hpp"
#include "math/quaternion-type.hpp"
#include "math/transform-type.hpp"

namespace math {

/// @addtogroup utility
/// @{

/**
 * Pi.
 */
template <class T>
constexpr T pi = T(3.14159265358979323846);

/**
 * Pi / 2.
 */
template <class T>
constexpr T half_pi = pi<T> * T(0.5);

/**
 * Pi * 2.
 */
template <class T>
constexpr T two_pi = pi<T> * T(2);

/**
 * 2x2 identity matrix.
 */
template <class T>
constexpr matrix<T, 2, 2> identity2x2 =
{{
	{1, 0},
	{0, 1}
}};

/**
 * 3x3 identity matrix.
 */
template <class T>
constexpr matrix<T, 3, 3> identity3x3 =
{{
	{1, 0, 0},
	{0, 1, 0},
	{0, 0, 1}
}};

/**
 * 4x4 identity matrix.
 */
template <class T>
constexpr matrix<T, 4, 4> identity4x4 =
{{
	{1, 0, 0, 0},
	{0, 1, 0, 0},
	{0, 0, 1, 0},
	{0, 0, 0, 1}
}};

/**
 * Unit quaternion.
 */
template <class T>
constexpr quaternion<T> identity_quaternion = {T(1), T(0), T(0), T(0)};

/**
 * Identity transform.
 */
template <class T>
constexpr transform<T> identity_transform =
{
	{T(0), T(0), T(0)},
	{T(1), T(0), T(0), T(0)},
	{T(1), T(1), T(1)}
};

/// @}

} // namespace math

#endif // ANTKEEPER_MATH_CONSTANTS_HPP
