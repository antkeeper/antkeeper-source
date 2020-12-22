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

#ifndef ANTKEEPER_DNA_TRANSCRIBE_HPP
#define ANTKEEPER_DNA_TRANSCRIBE_HPP

#include "nucleobase.hpp"
#include <algorithm>

namespace dna {

/**
 * Transcribes a range of IUPAC degenerate base symbols between DNA and RNA, swapping `T` for `U` or `U` for `T`.
 *
 * @param first,last Range of elements to transcribe.
 * @param d_first Beginning of the destination range.
 * @return Output iterator to the element past the last element transcribed. 
 */
template <class InputIt, class OutputIt>
OutputIt transcribe(InputIt first, InputIt last, OutputIt d_first);

template <class InputIt, class OutputIt>
OutputIt transcribe(InputIt first, InputIt last, OutputIt d_first)
{
	return std::transform(first, last, d_first, base::transcribe);
}

} // namespace dna

#endif // ANTKEEPER_DNA_TRANSCRIBE_HPP
