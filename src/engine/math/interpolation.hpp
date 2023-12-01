// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_INTERPOLATION_HPP
#define ANTKEEPER_MATH_INTERPOLATION_HPP

#include <engine/math/angles.hpp>
#include <cmath>

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
[[nodiscard]] constexpr T lerp(const T& x, const T& y, S a) noexcept
{
	return x + (y - x) * a;
}

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
[[nodiscard]] constexpr T lerp_angle(T x, T y, T a)
{
	return wrap_radians(x + wrap_radians(y - x) * a);
}

/**
 * Logarithmically interpolates between @p x and @p y.
 *
 * @warning Undefined behavior when @p x is zero.
 *
 * @tparam T Value type.
 * @tparam S Scalar type.
 */
template <typename T, typename S = float>
[[nodiscard]] T log_lerp(const T& x, const T& y, S a)
{
	//return std::exp(linear(std::log(x), std::log(y), a));
	return x * std::pow(y / x, a);
}

} // namespace math

#endif // ANTKEEPER_MATH_INTERPOLATION_HPP
