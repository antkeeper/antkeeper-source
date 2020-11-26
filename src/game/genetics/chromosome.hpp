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

#ifndef ANTKEEPER_CHROMOSOME_HPP
#define ANTKEEPER_CHROMOSOME_HPP

#include "utility/bit-math.hpp"
#include <array>

namespace dna
{

/**
 * Segregates the odd and even bits of a value.
 *
 * @param x Value to segregate.
 * @return Value with even bits of @p x in the lower half, and odd bits in the upper half.
 */
template <class T>
T segregate(T x)
{
	T odd = bit::compress(x);
	T even = bit::compress(x >> 1);
	
	return odd | (even << (sizeof(T) << 2));
}

/**
 * Interleaves bits of the lower and upper halves of a value.
 *
 * @param x Value to desegregate.
 * @return Value with bits from the upper half of @p x interleaved with bits from the lower half.
 */
template <class T>
T desegregate(T x)
{
	return bit::interleave<T>(x, x >> (sizeof(T) << 2));
}

/**
 * Replicates each bit in the lower half of a value.
 *
 * @param x Value to replicate.
 * @return Value of @p x interleaved with itself.
 */
template <class T>
T replicate(T x)
{
	x = bit::expand(x);
	return x | (x << 1);
}

/**
 * Generates four daughter chromosomes from a 2n/2c chromosome pair.
 *
 * @param x Homologous pair of chromosomes.
 * @param mask Bit mask with set bits marking crossover points.
 * @return Array of four 1n, 1c chromosomes.
 */
template <class T, class U = T>	
std::array<U, 4> meiosis(T x, U mask)
{
	U a = bit::compress(x);
	U b = bit::compress(x >> 1);
	U c = crossover_n(a, b, mask);
	U d = crossover_n(b, a, mask);
	
	return {a, c, d, b};
}

/**
 * 
 *
 * @param a 2c chromosome of first parent.
 * @param b 2c chromosome of second parent.
 * @param g Uniform random bit generator. `g()` will be called three times.
 * @return 2c chromosome of child.
 */
template <class T, class URBG>
T reproduce(T a, T b, URBG&& g)
{
	auto gametes_a = meiosis(a, static_cast<T>(g()));
	auto gametes_b = meiosis(b, static_cast<T>(g()));
	
	T i = static_cast<T>(g());
	T ca = gametes_a[i & 3];
	T cb = gametes_b[(i >> 2) & 3];
	
	return interleave(ca, cb);
}

/**
 * Performs a single-point crossover between two values.
 *
 * @param a First value.
 * @param b Second value.
 * @param i Index of the crossover point.
 * @return Crossed over value.
 */
template <class T>
constexpr T crossover(T a, T b, int i) noexcept;

/**
 * Performs an n-point crossover between two values.
 *
 * @param a First value.
 * @param b Second value.
 * @param mask Bit mask with set bits marking crossover points.
 * @return Crossed over value.
 */
template <class T>
constexpr T crossover_n(T a, T b, T mask) noexcept;

/**
 * Mutates a value by flipping a single bit.
 *
 * @param x Value to mutate.
 * @param i Index of the bit to flip.
 * @return Mutated copy of @p x.
 */
template <class T>
T mutate(T x, int i);

template <class T>
inline constexpr T crossover(T a, T b, int i) noexcept
{
	T mask = (T(1) << i) - 1;
	return bit_merge(b, a, mask);
}

template <class T>
constexpr T crossover_n(T a, T b, T mask) noexcept
{
	T merge = ~T(0) * parity(mask);
	
	while (mask)
	{
		merge ^= (mask ^ (mask - 1)) >> 1;
		mask &= mask - 1;
	}
	
	return bit_merge(a, b, merge);
}

template <class T>
inline T mutate(T x, int i)
{
	return x ^ (T(1) << i);
}

} // namespace dna

#endif // ANTKEEPER_CHROMOSOME_HPP
