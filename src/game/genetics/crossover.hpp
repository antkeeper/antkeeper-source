/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_CROSSOVER_HPP
#define ANTKEEPER_CROSSOVER_HPP

#include "bit-math.hpp"

namespace dna
{

/**
 * Performs a single-point crossover between two values.
 *
 * @param a First value.
 * @param b Second value.
 * @parma pos Position of the crossover point.
 */
template <class T>
constexpr T crossover(T a, T b, int pos) noexcept;

/**
 * Performs an n-point crossover between two values.
 *
 * @param a First value.
 * @param b Second value.
 * @param mask Bit mask with set bits marking crossover points.
 */
template <class T>
constexpr T crossover_n(T a, T b, T mask) noexcept;

template <class T>
inline constexpr T crossover(T a, T b, int pos) noexcept
{
	T mask = (T(1) << pos) - 1;
	return bit_merge(b, a, mask);
}

template <class T>
constexpr T crossover_n(T a, T b, T mask) noexcept
{
	T merge = 0, i = 0;
	
	while (mask)
	{
		merge ^= (mask ^ (mask - 1)) >> 1;
		mask &= mask - 1;
		i = !i;
	}
	
	merge ^= ~T(0) * i;
	
	return bit_merge<T>(a, b, merge);
}

} // namespace dna

#endif // ANTKEEPER_CROSSOVER_HPP
