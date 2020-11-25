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

#ifndef ANTKEEPER_ALLELE_HPP
#define ANTKEEPER_ALLELE_HPP

namespace dna
{

/**
 * Tests the two least significant bits of a value for equality
 *
 * @param x Value to test.
 * @return `true` if the two least significant bits are equal, `false` otherwise.
 */
template <class T>
constexpr bool homozygous(T x) noexcept;

/**
 * Tests the two least significant bits of a value for inequality.
 *
 * @param x Value to test.
 * @return `true` if the two least significant bits are inequal, `false` otherwise.
 */
template <class T>
constexpr bool heterozygous(T x) noexcept;

template <class T>
inline constexpr bool homozygous<T>(T x) noexcept
{
	return (x & 1) == ((x >> 1) & 1);
}

template <class T>
inline constexpr bool heterozygous<T>(T x) noexcept
{
	return (x & 1) != ((x >> 1) & 1);
}

} // namespace dna

#endif // ANTKEEPER_ALLELE_HPP
