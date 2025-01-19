// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/genetics/base.hpp>

namespace genetics {
namespace base {

/// Decodes an IUPAC degenerate base symbol into a bit mask representing the possible bases represented by the symbol.
/// @param symbol IUPAC degenerate base symbol.
/// @return Bit mask representing the possible bases represented by the symbol.
static inline unsigned char decode(char symbol)
{
	static constexpr unsigned char bases[25] =
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
	};
	
	return (symbol < 'A' || symbol >= 'Z') ? 0 : bases[symbol - 'A'];
}

int compare(char a, char b)
{
	static constexpr int popcount[16] =
	{
		0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
	};
	
	return popcount[decode(a) & decode(b)];
}

char transcribe(char symbol)
{
	return (symbol == 'T') ? 'U' : (symbol == 'U') ? 'T' : symbol;
}

namespace dna
{
	char complement(char symbol)
	{
		constexpr const char* complements = "TVGHZZCDZZMZKNZZZYSAABWZR";
		return (symbol < 'A' || symbol >= 'Z') ? 'Z' : complements[symbol - 'A'];
	}
}

namespace rna
{
	char complement(char symbol)
	{
		constexpr const char* complements = "UVGHZZCDZZMZKNZZZYSAABWZR";
		return (symbol < 'A' || symbol >= 'Z') ? 'Z' : complements[symbol - 'A'];
	}
}

} // namespace base
} // namespace genetics
