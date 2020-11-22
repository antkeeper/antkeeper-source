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

#include <cstdlib>
#include <iterator>

namespace dna
{

/**
 * Performs a genetic crossover on a range of interleaved allele blocks defined by `[first1, last1)`.
 *
 * @param[in] first1,last1 First range of interleaved allele blocks to crossover. 
 * @param[in] first2 Beginning of the the second range of interleaved allele blocks to crossover.
 * @param[out] d_first Beginning of the destination range.
 * @param[in,out] g A uniform random bit generator.
 * @return Output iterator to the block past the last block crossed over. 
 */
template <class InputIt1, class InputIt2, class OutputIt, class URBG>
OutputIt crossover(InputIt1 first1, InputIt1 last1, InputIt2 first2, OutputIt d_first, URBG&& g)
{
	typedef typename std::iterator_traits<OutputIt>::value_type block_t;
	static constexpr std::size_t bits_per_block = sizeof(block_t) << 3;
	
	while (first1 != last1)
	{
		block_t block = 0;
		
		for (std::size_t i = 0; i < bits_per_block;)
		{
			block_t allele1 = ((*first1) >> (i + (g() % 2))) & 1;
			block_t allele2 = ((*first2) >> (i + (g() % 2))) & 1;
			
			block |= allele1 << i++;
			block |= allele2 << i++;
		}
		
		*d_first++ = block;
		++first1;
		++first2;
	}
	
	return d_first;
}

/**
 * Performs a genetic mutation by flipping the bit of a single allele in a range of interleaved allele blocks defined by `[first1, last1)`.
 *
 * @param[in,out] first,last Range of interleaved allele blocks to mutate. 
 * @param[in,out] g A uniform random bit generator.
 * @return Locus of the mutated allele.
 */
template <class RandomIt, class URBG>
typename std::iterator_traits<RandomIt>::difference_type mutate(RandomIt first, RandomIt last, URBG&& g)
{
	typedef typename std::iterator_traits<RandomIt>::value_type block_t;
	typedef typename std::iterator_traits<RandomIt>::difference_type diff_t;
	static constexpr diff_t bits_per_block = sizeof(block_t) << 3;
	
	diff_t i = g() % ((last - first) * bits_per_block);
	first[i / bits_per_block] ^= block_t(1) << (i % bits_per_block);
	
	return i;
}

} // namespace dna

#endif // ANTKEEPER_GENETIC_OPERATORS_HPP
