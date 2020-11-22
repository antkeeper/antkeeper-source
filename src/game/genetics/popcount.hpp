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

#ifndef ANTKEEPER_POPCOUNT_HPP
#define ANTKEEPER_POPCOUNT_HPP

namespace dna
{

/// Returns the number of set bits (*population count*) in @p x. Also known as the *Hamming weight*.
template <typename T>
constexpr int popcount(T x) noexcept;

template <typename T>
inline constexpr int popcount(T x) noexcept
{
	int n;
	for (n = 0; x; ++n)
		x &= x - 1;
	return n;
}

} // namespace dna

#endif // ANTKEEPER_POPCOUNT_HPP
