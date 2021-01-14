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

#ifndef ANTKEEPER_BIT_MATH_HPP
#define ANTKEEPER_BIT_MATH_HPP

/// Bitwise math
namespace bit {

/**
 * Compresses the even bits of a value into the lower half, then clears the upper half.
 *
 * @param x Value to compress.
 * @return Compressed value.
 */
template <class T>
constexpr T compress(T x) noexcept;

/**
 * Returns the number of set bits in a value, known as a *population count* or *Hamming weight*.
 *
 * @param x Value to count.
 * @return Number of set bits in @p x.
 */
template <class T>
constexpr int count(T x) noexcept;

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
 * Reads bits from the least significant bits of a value and returns them in the positions marked by a mask.
 *
 * @param x Value from which bits should be read.
 * @param mask Bit mask indicating where bits should be deposited.
 * @return Bits from the least significant bits of @p x in the positions marked by @p mask.
 */
template <class T>
constexpr T deposit(T x, T mask) noexcept;

/**
 * Interleaves bits of the lower and upper halves of a value.
 *
 * @param x Value to desegregate.
 * @return Value with bits from the upper half of @p x interleaved with bits from the lower half.
 */
template <class T>
constexpr T desegregate(T x) noexcept;

/**
 * Returns the number of differing bits between two values, known as *Hamming distance*.
 *
 * @param x First value.
 * @param y Second value.
 * @return Hamming distance between @px and @p y.
 */	
template <class T>
constexpr int difference(T x, T y) noexcept;

/**
 * Moves bits from the lower half of a value to occupy all even bits, and clears all odd bits.
 *
 * @param x Value to expand.
 * @return Expanded value.
 */
template <class T>
constexpr T expand(T x) noexcept;

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
 * Flips a single bit in a value.
 *
 * @param x Value to modify.
 * @param i Index of the bit to flip.
 * @return Copy of @p x with one bit flipped.
 */
template <class T>
constexpr T flip(T x, int i) noexcept;

/**
 * Returns a value with even bits containing the first value's lower half, and odd bits containing the second value's lower half.
 *
 * @param a First value.
 * @param b Second value.
 * @return Value with bits from the lower halves of @p a and @p b interleaved.
 */
template <class T, class U = T>
constexpr U interleave(T a, T b) noexcept;

/**
 * Merges the bits of two values using a bit mask.
 *
 * @param a First value.
 * @param b Second value.
 * @param mask Bit mask with zeros where bits from @p a should be used, and ones where bits from @p b should be used.
 * @return Merged value.
 */
template <class T>
constexpr T merge(T a, T b, T mask) noexcept;

/**
 * Returns the parity of a value.
 *
 * @param x Value to test.
 * @return `1` if an odd number of bits are set, `0` otherwise.
 */
template <class T>
constexpr T parity(T x) noexcept;

/**
 * Segregates the odd and even bits of a value.
 *
 * @param x Value to segregate.
 * @return Value with even bits of @p x in the lower half, and odd bits in the upper half.
 */
template <class T>
constexpr T segregate(T x) noexcept;

/**
 * Swaps the each odd bit with its following even bit.
 *
 * @param x Value in which adjacent bits should be swap.
 * @return Copy of @p x with adjacent bits swapped.
 */
template <class T>
constexpr T swap_adjacent(T x) noexcept;


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
constexpr int count(T x) noexcept
{
	int n = 0;
	for (; x; ++n)
		x &= x - 1;
	return n;
}

template <class T>
inline constexpr T crossover(T a, T b, int i) noexcept
{
	T mask = (T(1) << i) - 1;
	return merge(b, a, mask);
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
	
	return merge(a, b, merge);
}

template <class T>
inline constexpr T desegregate(T x) noexcept
{
	return interleave<T>(x, x >> (sizeof(T) << 2));
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

template <class T>
constexpr T deposit(T x, T mask) noexcept
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
inline constexpr int difference(T x, T y) noexcept
{
	return count(x ^ y);
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
inline constexpr T flip(T x, int i) noexcept
{
	return x ^ (T(1) << i);
}

template <class T, class U>
inline constexpr U interleave(T a, T b) noexcept
{
	return expand<U>(a) | (expand<U>(b) << 1);
}

template <class T>
inline constexpr T merge(T a, T b, T mask) noexcept
{
	return a ^ ((a ^ b) & mask);
}

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
constexpr T segregate(T x) noexcept
{
	T odd = compress(x);
	T even = compress(x >> 1);
	
	return odd | (even << (sizeof(T) << 2));
}

template <class T>
constexpr T swap_adjacent(T x) noexcept
{
	return ((x & T(0xaaaaaaaaaaaaaaaa)) >> 1) | ((x & T(0x5555555555555555)) << 1);
}

} // namespace bit

#endif // ANTKEEPER_BIT_MATH_HPP
