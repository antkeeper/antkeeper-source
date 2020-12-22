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

#ifndef ANTKEEPER_DNA_MUTATE_HPP
#define ANTKEEPER_DNA_MUTATE_HPP

#include <algorithm>
#include <iterator>
#include <random>

namespace dna {

/**
 * Applies the given function to a randomly selected element in a range.
 *
 * @param first,last Range of elements to mutate.
 * @param unary_op Unary operation function object that will be applied.
 * @param g Uniform random bit generator.
 * @return Iterator to the mutated element.
 */
template <class ForwardIt, class UnaryOperation, class URBG>
ForwardIt mutate(ForwardIt first, ForwardIt last, UnaryOperation unary_op, URBG&& g);

/**
 * Applies the given function to a random selection of elements in a range.
 *
 * @param first,last Range of elements to mutate.
 * @param count Number of elements to mutate.
 * @param unary_op Unary operation function object that will be applied.
 * @param g Uniform random bit generator.
 */
template <class ForwardIt, class Size, class UnaryOperation, class URBG>
void mutate_n(ForwardIt first, ForwardIt last, Size count, UnaryOperation unary_op, URBG&& g);

template <class ForwardIt, class UnaryOperation, class URBG>
ForwardIt mutate(ForwardIt first, ForwardIt last, UnaryOperation unary_op, URBG&& g)
{
	typedef typename std::iterator_traits<ForwardIt>::difference_type difference_t;
	
	std::uniform_int_distribution<difference_t> distribution(0, std::distance(first, last) - 1);
	std::advance(first, distribution(g));
	*first = unary_op(*first);
	
	return first;
}

template <class ForwardIt, class Size, class UnaryOperation, class URBG>
void mutate_n(ForwardIt first, ForwardIt last, Size count, UnaryOperation unary_op, URBG&& g)
{
	typedef typename std::iterator_traits<ForwardIt>::difference_type difference_t;
	
	std::uniform_int_distribution<difference_t> distribution(0, std::distance(first, last) - 1);
	ForwardIt mutation;
	
	while (count)
	{
		mutation = first;
		std::advance(mutation, distribution(g));
		*mutation = unary_op(*mutation);
		--count;
	}
}

} // namespace dna

#endif // ANTKEEPER_DNA_MUTATE_HPP
