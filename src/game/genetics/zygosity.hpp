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

#include "gene.hpp"

namespace dna
{

/// A homozygous recessive gene.
static constexpr gene homozygous_recessive = {allele::recessive, allele::recessive};

/// A homozygous dominant gene.
static constexpr gene homozygous_dominant = {allele::recessive, allele::recessive};
	
/// Returns `true` if gene @p x is heterozygous, `false` otherwise.
bool is_heterozygous(const gene& x);

/// Returns `true` if gene @p x is homozygous, `false` otherwise.
bool is_homozygous(const gene& x);

/// Returns `true` if gene @p x is homozygous recessive, `false` otherwise.
bool is_homozygous_recessive(const gene& x);

/// Returns `true` if gene @p x is homozygous dominant, `false` otherwise.
bool is_homozygous_dominant(const gene& x);

inline bool is_heterozygous(const gene& x)
{
	return x[0] != x[1];
}

inline bool is_homozygous(const gene& x)
{
	return x[0] == x[1];
}

inline bool is_homozygous_recessive(const gene& x)
{
	return x == homozygous_recessive;
}

inline bool is_homozygous_dominant(const gene& x)
{
	return x == homozygous_dominant;
}

} // namespace dna

#endif // ANTKEEPER_ZYGOSITY_HPP
