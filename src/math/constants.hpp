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

/// Pi.
template <class T>
constexpr T pi = T(3.1415926535897932384626433832795);

/// Pi * 2.
template <class T>
constexpr T two_pi = pi<T> * T(2);

/// Pi * 4.
template <class T>
constexpr T four_pi = pi<T> * T(4);

/// Pi / 2.
template <class T>
constexpr T half_pi = pi<T> * T(0.5);

/// 1 / Pi.
template <class T>
constexpr T inverse_pi = T(1) / pi<T>;

} // namespace math

#endif // ANTKEEPER_MATH_CONSTANTS_HPP
