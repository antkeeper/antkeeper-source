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

#ifndef ANTKEEPER_DNA_CROSSOVER_HPP
#define ANTKEEPER_DNA_CROSSOVER_HPP

#include <algorithm>
#include <iterator>
#include <random>

namespace dna {

/**
 * Exchanges elements between two ranges, starting at a random offset.
 *
 * @param first1,last1 First range of elements to crossover.
 * @param first2 Beginning of the second range of elements to crossover.
 * @param g Uniform random bit generator.
 * @return Iterator to the start of the crossover in the second range.
 */
template <class ForwardIt1, class ForwardIt2, class URBG>
ForwardIt2 crossover(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2, URBG&& g);

/**
 * Exchanges elements between two ranges multiple times, starting at a random offset each time.
 *
 * @param first1,last1 First range of elements to crossover.
 * @param first2 Beginning of the second range of elements to crossover.
 * @param count Number of times to crossover.
 * @param g Uniform random bit generator.
 */
template <class ForwardIt1, class ForwardIt2, class Size, class URBG>
void crossover_n(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2, Size count, URBG&& g);

template <class ForwardIt1, class ForwardIt2, class URBG>
ForwardIt2 crossover(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2, URBG&& g)
{
	typedef typename std::iterator_traits<ForwardIt1>::difference_type difference_t;
	std::uniform_int_distribution<difference_t> distribution(0, std::distance(first1, last1) - 1);
	difference_t pos = distribution(g);
	std::advance(first1, pos);
	std::advance(first2, pos);
	std::swap_ranges(first1, last1, first2);
	return first2;
}

template <class ForwardIt1, class ForwardIt2, class Size, class URBG>
void crossover_n(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2, Size count, URBG&& g)
{
	typedef typename std::iterator_traits<ForwardIt1>::difference_type difference_t;
	
	std::uniform_int_distribution<difference_t> distribution(0, std::distance(first1, last1) - 1);
	ForwardIt1 crossover1, crossover2;
	
	while (count)
	{
		crossover1 = first1;
		crossover2 = first2;
		
		difference_t pos = distribution(g);
		std::advance(crossover1, pos);
		std::advance(crossover2, pos);
		std::swap_ranges(crossover1, last1, crossover2);
		
		--count;
	}
}

} // namespace dna

#endif // ANTKEEPER_DNA_CROSSOVER_HPP
