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

#ifndef ANTKEEPER_MATH_CONSTANTS_HPP
#define ANTKEEPER_MATH_CONSTANTS_HPP

#include <limits>

namespace math {

/// Positive infinity.
template <class T>
constexpr T inf{std::numeric_limits<T>::infinity()};

/// Pi.
template <class T>
constexpr T pi = T{3.1415926535897932384626433832795};

/// Pi * 2.
template <class T>
constexpr T two_pi = pi<T> * T{2};

/// Pi * 4.
template <class T>
constexpr T four_pi = pi<T> * T{4};

/// Pi / 2.
template <class T>
constexpr T half_pi = pi<T> / T{2};

/// 1 / Pi.
template <class T>
constexpr T inverse_pi = T{1} / pi<T>;

/// sqrt(0.5)
template <class T>
constexpr T sqrt_half = T{0.70710678118654752440084436210485};

/// sqrt(2)
template <class T>
constexpr T sqrt_2 = T{1.4142135623730950488016887242097};

/// sqrt(3)
template <class T>
constexpr T sqrt_3 = T{1.7320508075688772935274463415059};

/// sqrt(5)
template <class T>
constexpr T sqrt_5 = T{2.2360679774997896964091736687313};

} // namespace math

#endif // ANTKEEPER_MATH_CONSTANTS_HPP
