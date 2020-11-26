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

#ifndef ANTKEEPER_BIT_MATH_HPP
#define ANTKEEPER_BIT_MATH_HPP

namespace bit
{

/**
 * Reads bits from the least significant bits of a value and returns them in the positions marked by a mask.
 *
 * @param x Value from which bits should be read.
 * @param mask Bit mask indicating where bits should be deposited.
 * @return Bits from the least significant bits of @p x in the positions marked by @p mask.
 */
template <class T>
constexpr T deposit(T x, T mask) noexcept;

/**
 * Reads bits from a value in the positions marked by a mask and returns them in the least significant bits.
 *
 * @param x Value from which bits should be read.
 * @param mask Bit mask indicating which bits to extract.
 * @return Bits of @p x from the positions marked by @p mask in the least significant bits.
 */
template <class T>
constexpr T extract(T x, T mask) noexcept;

/**
 * Returns the number of set bits in a value, known as a *population count* or *Hamming weight*.
 *
 * @param x Value to count.
 * @return Number of set bits in @p x.
 */
template <class T>
constexpr int count(T x) noexcept;

/**
 * Returns the number of differing bits between two values, known as *Hamming distance*.
 *
 * @param x First value.
 * @param y Second value.
 * @return Hamming distance between @px and @p y.
 */	
template <class T>
constexpr int difference(T x, T y) noexcept;

template <class T>
constexpr T merge(T a, T b, T mask) noexcept
{
	return a ^ ((a ^ b) & mask);
}

template <class T>
constexpr T expand(T x) noexcept
{
	x &= (1 << (sizeof(T) << 2)) - 1;
	
	if constexpr(sizeof(T) >= 8)
		x = (x ^ (x << 16)) & T(0x0000ffff0000ffff);
	if constexpr(sizeof(T) >= 4)
		x = (x ^ (x <<  8)) & T(0x00ff00ff00ff00ff);
	if constexpr(sizeof(T) >= 2)
		x = (x ^ (x <<  4)) & T(0x0f0f0f0f0f0f0f0f);
	
	x = (x ^ (x << 2)) & T(0x3333333333333333);
	x = (x ^ (x << 1)) & T(0x5555555555555555);
	
	return x;
}

template <class T, class U = T>
inline constexpr U interleave(T a, T b) noexcept
{
	return bit_expand<U>(a) | (bit_expand<U>(b) << 1);
}

/**
 * Returns the parity of a value.
 *
 * @param x Value to test.
 * @return `1` if an odd number of bits are set, `0` otherwise.
 */
template <class T>
constexpr T parity(T x) noexcept
{
	if constexpr(sizeof(T) >= 8)
		x ^= x >> 32;
	if constexpr(sizeof(T) >= 4)
		x ^= x >> 16;
	if constexpr(sizeof(T) >= 2)
		x ^= x >> 8;
	
	x ^= x >> 4;
	
	return (0x6996 >> (x & 0xf)) & 1;
}

template <class T>
constexpr T compress(T x) noexcept
{
	x &= T(0x5555555555555555);
	
	x = (x ^ (x >> 1)) & T(0x3333333333333333);
	x = (x ^ (x >> 2)) & T(0x0f0f0f0f0f0f0f0f);
	
	if constexpr(sizeof(T) >= 2)
		x = (x ^ (x >>  4)) & T(0x00ff00ff00ff00ff);
	if constexpr(sizeof(T) >= 4)
		x = (x ^ (x >>  8)) & T(0x0000ffff0000ffff);
	if constexpr(sizeof(T) >= 8)
		x = (x ^ (x >> 16)) & T(0x00000000ffffffff);
	
	return x;
}

template <class T>
constexpr T swap_adjacent(T x) noexcept
{
	return ((x & T(0xaaaaaaaaaaaaaaaa)) >> 1) | ((x & T(0x5555555555555555)) << 1);
}

/**
 * For an n-bit number with r set bits, there are `n! / ((n - r)! * r!)` permutations.
 */
template <class T>
constexpr T next_permutation(T x) noexcept
{
	T y = (x | (x - 1)) + 1;
	return y | ((((y & -y) / (x & -x)) >> 1) - 1);  
}

template <class T>
constexpr T bit_deposit(T x, T mask) noexcept
{
	T result = 0;
	
	for (T i = 1; mask; i <<= 1)
	{
		if (x & i)
			result |= mask & -mask;
		mask &= mask - 1;
	}
	
	return result;
}

template <class T>
constexpr T extract(T x, T mask) noexcept
{
	T result = 0;
	
	for (T i = 1; mask; i <<= 1)
	{
		if (x & mask & -mask)
			result |= i;
		mask &= mask - 1;
	}
	
	return result;
}

template <class T>
constexpr int count(T x) noexcept
{
	int n = 0;
	for (; x; ++n)
		x &= x - 1;
	return n;
}

template <class T>
inline constexpr int difference(T x, T y) noexcept
{
	return count(x ^ y);
}

} // namespace bit

#endif // ANTKEEPER_BIT_MATH_HPP
