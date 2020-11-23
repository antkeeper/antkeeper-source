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

namespace dna
{

/**
 * Reads bits from the least significant bits of a value and returns them in the positions marked by a mask.
 *
 * @param x Value from which bits should be read.
 * @param mask Bit mask indicating where bits should be deposited.
 * @return Bits from the least significant bits of @p x in the positions marked by @p mask.
 */
template <class T>
constexpr T bit_deposit(T x, T mask) noexcept;

/**
 * Reads bits from a value in the positions marked by a mask and returns them in the least significant bits.
 *
 * @param x Value from which bits should be read.
 * @param mask Bit mask indicating which bits to extract.
 * @return Bits of @p x from the positions marked by @p mask in the least significant bits.
 */
template <class T>
constexpr T bit_extract(T x, T mask) noexcept;

/**
 * Returns the number of set bits in a value, known as a *population count* or *Hamming weight*.
 *
 * @param x Value to count.
 * @return Number of set bits in @p x.
 */
template <typename T>
constexpr int popcount(T x) noexcept;

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
constexpr T bit_extract(T x, T mask) noexcept
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

template <typename T>
constexpr int popcount(T x) noexcept
{
	int n = 0;
	for (; x; ++n)
		x &= x - 1;
	return n;
}

} // namespace dna

#endif // ANTKEEPER_BIT_MATH_HPP
