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

#ifndef ANTKEEPER_MATH_COMPILE_HPP
#define ANTKEEPER_MATH_COMPILE_HPP

#include <concepts>

namespace math {

/// Compile-time mathematical functions.
namespace compile {

/**
 * Compile-time `pow` for unsigned integrals.
 *
 * @param x Base value.
 * @param e Integral exponent.
 *
 * @return `x^e`.
 */
template <std::unsigned_integral T>
consteval T pow(T x, T e) noexcept
{
	return (e == 0) ? T(1) : (x * pow<T>(x, e - 1));
}

/**
 * Compile-time `ceil(log2(x))` for unsigned integrals.
 *
 * @param x Input value.
 *
 * @return `ceil(log2(x))`.
 */
template <std::unsigned_integral T>
consteval T ceil_log2(T x) noexcept
{
	return (x <= T(1)) ? T(0) : ceil_log2((x + T(1)) / T(2)) + T(1);
}

} // namespace compile

} // namespace math

#endif // ANTKEEPER_MATH_COMPILE_HPP
