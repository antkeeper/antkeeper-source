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

#ifndef ANTKEEPER_MATH_RANDOM_HPP
#define ANTKEEPER_MATH_RANDOM_HPP

#include <cstdlib>
#include <type_traits>

namespace math {

/// @addtogroup utility
/// @{

/**
 * Generates a pseudo-random floating point number on `[start, end)` using std::rand().
 *
 * @warning Don't forget to seed with std::srand() before using!
 *
 * @param start Start of the range (inclusive).
 * @param end End of the range (exclusive).
 * @return Pseudo-random floating point number.
 */
template <typename T = float>
T random(T start, T end);

template <typename T>
inline T random(T start, T end)
{
	static_assert(std::is_floating_point<T>::value);
	constexpr T rand_max_inverse = T(1) / static_cast<T>(RAND_MAX);
    return static_cast<T>(std::rand()) * rand_max_inverse * (end - start) + start;
}

/// @}

} // namespace math

#endif // ANTKEEPER_MATH_RANDOM_HPP
