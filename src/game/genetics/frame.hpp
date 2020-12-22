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

#ifndef ANTKEEPER_DNA_FRAME_HPP
#define ANTKEEPER_DNA_FRAME_HPP

#include <iterator>

namespace dna {

/**
 * Finds the first start codon in a sequence.
 *
 * @param first,last Range of elements to search.
 * @param n Number of elements per codon.
 * @param p Binary predicate which returns `true` if a subrange of length @p n is a start codon.
 * @return Iterator to the first element in the start codon, or @p last if no start codon was found.
 */
template <class InputIt, class Size, class BinaryPredicate>
InputIt find_start(InputIt first, InputIt last, Size n, BinaryPredicate p)
{
	auto length = std::distance(first, last);
	
	if (length >= n)
	{
		InputIt next = first;
		std::advance(next, n);
		
		do
		{
			if (p(first, next))
				return first;
			
			++first;
			++next;
			--length;
		}
		while (length >= n);
	}
	
	return last;
}

/**
 * Searches the range `[first, last)` for a sequence of @p n elements which satifies predicate @p p.
 *
 * @param first,last Range of elements to search.
 * @param n Number of elements in the sequence.
 * @param stride Number of elements between searches.
 * @param p
 */
template <class InputIt, class Size, class BinaryPredicate>
InputIt find_sequence(InputIt first, InputIt last, Size n, Size stride, BinaryPredicate p)
{
	if (auto length = std::distance(first, last); length >= n)
	{
		Size offset = n + stride;
		InputIt next = first;
		std::advance(next, n);
		
		do
		{
			if (p(first, next))
				return first;
			
			if (length < offset)
				break;
			
			std::advance(first, stride);
			std::advance(next, stride);
			length -= offset;
		}
		while (1);
	}
	
	return last;
}

/**
 * Finds the first stop codon in a sequence.
 *
 * @param first,last Range of elements to search.
 * @param n Number of elements per codon.
 * @param p Binary predicate which returns `true` if a subrange of length @p n is a stop codon.
 * @return Iterator to the first element in the stop codon, or @p last if no stop codon was found.
 */
template <class InputIt, class Size, class BinaryPredicate>
InputIt find_stop(InputIt first, InputIt last, Size n, BinaryPredicate p)
{
	for (auto length = std::distance(first, last); length >= n; length -= n)
	{
		InputIt next = first;
		std::advance(next, n);
		if (p(first, next))
			return first;
		first = next;
	}
	
	return last;
}

/**
 * Finds the first open reading frame (ORF) in a range of elements.
 *
 * @param[in,out] first Iterator to the beginning of the sequence, which will point to th
 *
 * @param start_p Binary predicate which returns `true` if a subrange of length @p n is a start codon.
 * @param stop_p Binary predicate which returns `true` if a subrange of length @p n is a stop codon.
 */
template <class InputIt, class Size, class BinaryPredicate1, class BinaryPredicate2>
void find_orf(InputIt& first, InputIt& last, Size n, BinaryPredicate1 start_p, BinaryPredicate2 stop_p)
{
	first = find_start(first, last, n, start_p);
	if (first != last)
		last = find_stop(first, last, n, stop_p);
}

} // namespace dna

#endif // ANTKEEPER_DNA_FRAME_HPP
