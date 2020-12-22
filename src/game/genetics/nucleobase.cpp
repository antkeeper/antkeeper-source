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

#include "nucleobase.hpp"
#include <cstdint>

namespace dna {
namespace base {

/**
 * Decodes an IUPAC degenerate base symbol into a bit mask representing the possible bases represented by the symbol.
 *
 * @param symbol IUPAC degenerate base symbol.
 * @return Bit mask representing the possible bases represented by the symbol.
 */
static std::uint8_t decode(char symbol)
{
	static constexpr std::uint8_t bases[26] =
	{
		0b0001, // A
		0b1110, // B
		0b0010, // C
		0b1101, // D
		0,      // E
		0,      // F
		0b0100, // G
		0b1011, // H
		0,      // I
		0,      // J
		0b1100, // K
		0,      // L
		0b0011, // M
		0b1111, // N
		0,      // O
		0,      // P
		0,      // Q
		0b0101, // R
		0b0110, // S
		0b1000, // T
		0b1000, // U
		0b0111, // V
		0b1001, // W
		0,      // X
		0b1010, // Y
		0,      // Z
	};
	
	return (symbol < 'A' || symbol > 'Z') ? 0 : bases[symbol - 'A'];
}

char complement_rna(char symbol)
{
	static constexpr char* complements = "TVGHZZCDZZMZKNZZZYSAABWZRZ";
	return (symbol < 'A' || symbol > 'Z') ? 'Z' : complements[symbol - 'A'];
}

char complement_dna(char symbol)
{
	static constexpr char* complements = "UVGHZZCDZZMZKNZZZYSAABWZRZ";
	return (symbol < 'A' || symbol > 'Z') ? 'Z' : complements[symbol - 'A'];
}

char transcribe(char symbol)
{
	return (symbol == 'T') ? 'U' : (symbol == 'U') ? 'T' : symbol;
}

int compare(char a, char b)
{
	std::uint8_t bases = decode(a) & decode(b);
	return (bases & 1) + (bases >> 1 & 1) + (bases >> 2 & 1) + (bases >> 3 & 1);
}

} // namespace base
} // namespace dna
