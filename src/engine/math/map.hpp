// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_MAP_HPP
#define ANTKEEPER_MATH_MAP_HPP

namespace math {

/**
 * Remaps a number from one range to another.
 *
 * @param x Value to remap.
 * @param from_min Start of the first range.
 * @param from_max End of the first range.
 * @param to_min Start of the second range.
 * @param to_max End of the second range.
 *
 * @return Unclamped remapped value.
 */
template <class T>
[[nodiscard]] constexpr T map(T x, T from_min, T from_max, T to_min, T to_max) noexcept
{
	return to_min + (x - from_min) * (to_max - to_min) / (from_max - from_min);
}

} // namespace math

#endif // ANTKEEPER_MATH_MAP_HPP
