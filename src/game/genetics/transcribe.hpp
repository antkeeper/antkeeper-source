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

#include <algorithm>

namespace dna
{

/**
 * Transcribes DNA into RNA, replacing all occurences of `T` with `U`.
 *
 * @param first,last Range of DNA bases to transcribe.
 */
template <class ForwardIt>
void transcribe(ForwardIt first, ForwardIt last);

/**
 * Transcribes RNA back into DNA, replacing all occurences of `U` with `T`.
 *
 * @param first,last Range of RNA bases to transcribe.
 */
template <class ForwardIt>
void untranscribe(ForwardIt first, ForwardIt last);

template <class ForwardIt>
void transcribe(ForwardIt first, ForwardIt last)
{
	std::replace(first, last, 'T', 'U');
}

template <class ForwardIt>
void untranscribe(ForwardIt first, ForwardIt last)
{
	std::replace(first, last, 'U', 'T');
}

} // namespace dna

#endif // ANTKEEPER_DNA_TRANSCRIBE_HPP
