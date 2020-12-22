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

#ifndef ANTKEEPER_DNA_TRANSLATE_HPP
#define ANTKEEPER_DNA_TRANSLATE_HPP

#include <iterator>

namespace dna {

/**
 * Divides a range into consecutive subranges of @p n elements, then applies the given function to each subrange and stores the result in another range.
 *
 * @param first,last Range of elements to translate.
 * @param d_first Beginning of the destination range.
 * @param n Number of elements by which to divide the range.
 * @param binary_op Binary operation function object that will be applied to each subrange of @p n elements.
 * @return Output iterator to the element past the last element translated. 
 */
template <class InputIt, class OutputIt, class Size, class BinaryOperation>
OutputIt translate(InputIt first, InputIt last, OutputIt d_first, Size n, BinaryOperation binary_op);

template <class InputIt, class OutputIt, class Size, class BinaryOperation>
OutputIt translate(InputIt first, InputIt last, OutputIt d_first, Size n, BinaryOperation binary_op)
{
	for (auto length = std::distance(first, last); length >= n; length -= n)
	{
		InputIt next = first;
		std::advance(next, n);
		*(d_first++) = binary_op(first, next);
		first = next;
	}
	
	return d_first;
}

} // namespace dna

#endif // ANTKEEPER_DNA_TRANSLATE_HPP
