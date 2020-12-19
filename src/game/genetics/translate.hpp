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

#ifndef ANTKEEPER_TRANSLATE_HPP
#define ANTKEEPER_TRANSLATE_HPP

#include <algorithm>
#include <cstdint>
#include <iterator>

namespace dna
{

constexpr char* standard_code =
	"FFLLSSSSYY**CC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG"  // Amino acid
	"---M------**--*----M---------------M----------------------------"  // Start/stop
	"TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG"  // Base 1
	"TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG"  // Base 2
	"TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG"; // Base 3

/**
 * Translates codons into amino acids until a stop codon is read or the end of the sequence is reached.
 *
 * @param first,last Range of codons to translate.
 * @param t_first Beginning of the translation table.
 * @param d_first Beginning of the destination range.
 * @return Output iterator to the amino acid in the destination range, one past the last amino acid translated.
 */
template <class InputIt1, class InputIt2, class OutputIt>
OutputIt translate(InputIt1 first, InputIt1 last, InputIt2 t_first, OutputIt d_first)
{
	InputIt1 second = first;
	++second;
	InputIt1 third = second;
	++third;
	
	InputIt2 base1_first = t_first;
	std::advance(base1_first, 128);
	InputIt2 base2_first = base1_first;
	std::advance(base2_first, 64);
	InputIt2 base3_first = base2_first;
	std::advance(base3_first, 64);
	
	while (first != last && second != last && third != last)
	{
		InputIt2 aa = t_first;
		InputIt2 base1 = base1_first;
		InputIt2 base2 = base2_first;
		InputIt2 base3 = base3_first;
		
		for (std::uint_fast8_t i = 0; i < 64; ++i)
		{
			if (*first == *base1 && *second == *base2 && *third == *base3)
			{
				if (*aa == '*')
					return d_first;
				
				*(d_first++) = *aa;
				break;
			}
			
			++aa;
			++base1;
			++base2;
			++base3;
		}
		
		first = ++third;
		second = ++third;
		++third;
	}
	
	return d_first;
}

} // namespace dna

#endif // ANTKEEPER_TRANSLATE_HPP
