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

#ifndef ANTKEEPER_GENETIC_OPERATORS_HPP
#define ANTKEEPER_GENETIC_OPERATORS_HPP

namespace dna
{

/**
 * Performs a genetic crossover on two blocks of interleaved alleles.
 *
 * @param a Interleaved allele block of the first parent.
 * @param b Interleaved allele block of the second parent.
 * @param g Uniform random bit generator.
 * @return Interleaved allele block of the new offspring.
 */
template <class T, class URBG>
T crossover(T a, T b, URBG&& g)
{
	T c = 0, i = 1;
	
	do
	{
		c |= (a >> (g() % 2)) & i;
		i <<= 1;
		c |= (b << (g() % 2)) & i;
		i <<= 1;
	}
	while (i);
	
	return c;
}

/**
 * Mutates a single allele in an interleaved allele block.
 *
 * @param x Interleaved allele block to mutate.
 * @param g Uniform random bit generator.
 * @return Mutated copy of @p x.
 */
template <class T, class URBG>
T mutate(T x, URBG&& g)
{
	return x ^ (T(1) << (g() % (sizeof(T) << 3)));
}

} // namespace dna

#endif // ANTKEEPER_GENETIC_OPERATORS_HPP
