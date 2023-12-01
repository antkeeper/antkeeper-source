// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_FRACT_HPP
#define ANTKEEPER_MATH_FRACT_HPP

#include <cmath>
#include <concepts>

namespace math {

/**
 * Returns the fractional part of a floating-point number.
 *
 * @tparam T Floating-point type.
 *
 * @param x Floating-point number.
 *
 * @return Fractional part of @p x.
 */
template <std::floating_point T>
[[nodiscard]] inline T fract(T x)
{
	T i;
	return std::modf(x, &i);
}

} // namespace math

#endif // ANTKEEPER_MATH_FRACT_HPP
