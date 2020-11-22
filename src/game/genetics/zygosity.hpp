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

#ifndef ANTKEEPER_ZYGOSITY_HPP
#define ANTKEEPER_ZYGOSITY_HPP

namespace dna
{
	
/**
 * Tests a gene for heterozygosity.
 *
 * @param x Gene with alleles encoded in the two least significant bits.
 * @return `true` if the gene is heterozygous, `false` otherwise.
 */
template <class T>
bool is_heterozygous(T x);

/**
 * Tests a gene for homozygosity.
 *
 * @param x Gene with alleles encoded in the two least significant bits.
 * @return `true` if the gene is homozygous, `false` otherwise.
 */
template <class T>
bool is_homozygous(T x);

template <class T>
inline bool is_heterozygous<T>(T x)
{
	return x & 1 != (x >> 1) & 1;
}

template <class T>
inline bool is_homozygous<T>(T x)
{
	return x & 1 == (x >> 1) & 1;
}

} // namespace dna

#endif // ANTKEEPER_ZYGOSITY_HPP
