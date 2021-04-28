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

#ifndef ANTKEEPER_MATH_INTERPOLATION_HPP
#define ANTKEEPER_MATH_INTERPOLATION_HPP

#include "math/constants.hpp"
#include "math/matrix-type.hpp"
#include "math/quaternion-type.hpp"
#include "math/transform-type.hpp"
#include <cmath>
#include <type_traits>

namespace math {

/**
 * Linearly interpolates between @p x and @p y.
 *
 * Requires the following operators to be defined:
 *
 *     T operator+(const T&, const T&);
 *     T operator-(const T&, const T&);
 *     T operator*(const T&, S);
 *
 * @tparam T Value type.
 * @tparam S Scalar type.
 */
template <typename T, typename S = float>
T lerp(const T& x, const T& y, S a);

/**
 * Linearly interpolates between two angles, @p x and @p y.
 *
 * @tparam T Value type.
 * @param x Start angle, in radians.
 * @param y End angle, in radians.
 * @param a Interpolation ratio.
 * @return Interpolated angle, in radians.
 */
template <typename T>
T lerp_angle(T x, T y, T a);

/**
 * Logarithmically interpolates between @p x and @p y.
 *
 * @warning Undefined behavior when @p x is zero.
 *
 * @tparam T Value type.
 * @tparam S Scalar type.
 */
template <typename T, typename S = float>
T log_lerp(const T& x, const T& y, S a);

template <typename T, typename S>
inline T lerp(const T& x, const T& y, S a)
{
	return x + (y - x) * a;
}

template <typename T>
T lerp_angle(T x, T y, T a)
{
	T shortest_angle = std::remainder((y - x), two_pi<T>);
	return std::remainder(x + shortest_angle * a, two_pi<T>);
}

template <typename T, typename S>
inline T log_lerp(const T& x, const T& y, S a)
{
	//return std::exp(linear(std::log(x), std::log(y), a));
	return x * std::pow(y / x, a);
}

} // namespace math

#endif // ANTKEEPER_MATH_INTERPOLATION_HPP
