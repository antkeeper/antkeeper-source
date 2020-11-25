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

#ifndef ANTKEEPER_GENE_HPP
#define ANTKEEPER_GENE_HPP

namespace dna
{

/**
 * Isolates a pair of bits and returns them in the two least significant bits.
 *
 * @param x Diploid chromosome with interleaved alleles.
 * @param i Index of the pair to isolate.
 * @return Isolated pair of bits in the two least significant bits.
 */
template <class T>
T isolate(T x, int i)
{
	return (x >> (i << 1)) & 0b11;
}

/**
 * Isolates a sequence of genes and returns their alleles in the least significant bits.
 *
 * @param x Diploid chromosome with interleaed alleles.
 * @param i Index of the first gene in the sequence.
 * @param n Length of the sequence, in genes.
 * @return Alleles of the sequeuence in the least significant bits.
 */
template <class T>
T isolate_n(T x, int i, int n)
{
	T mask = (T(1) << (n << 1)) - 1;
	return (x >> (i << 1)) & mask;
}

} // namespace dna

#endif // ANTKEEPER_GENE_HPP
