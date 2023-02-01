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
constexpr T map(T x, T from_min, T from_max, T to_min, T to_max) noexcept
{
	return to_min + (x - from_min) * (to_max - to_min) / (from_max - from_min);
}

} // namespace math

#endif // ANTKEEPER_MATH_MAP_HPP
