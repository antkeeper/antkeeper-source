/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "codon.hpp"

namespace genetics {
namespace codon {

/**
 * Returns the index of a nucleobase for use with a translation table.
 *
 * @param base IUPAC code of nucleobase, either `U`, `T`, `C`, `A`, or `G`.
 * @return Index of the nucleobase, or a negative value if a non-standard nucleobase was supplied.
 */
static inline int base_index(char base)
{
	switch (base)
	{
		case 'U':
		case 'T':
			return 0;
		case 'C':
			return 1;
		case 'A':
			return 2;
		case 'G':
			return 3;
	}
	
	return ~3;
}

/**
 * Returns the index of a codon for use with a translation table.
 *
 * @param base1 IUPAC code of first nucleobase, either `U`, `T`, `C`, `A`, or `G`.
 * @param base2 IUPAC code of second nucleobase, either `U`, `T`, `C`, `A`, or `G`.
 * @param base3 IUPAC code of third nucleobase, either `U`, `T`, `C`, `A`, or `G`.
 * @return Index of codon, or a negative value if a non-standard nucleobase was supplied.
 */
static inline int codon_index(char base1, char base2, char base3)
{
	int i = base_index(base1);
	int j = base_index(base2);
	int k = base_index(base3);
	return (i << 4) | (j << 2) | k;
}

inline char translate(char base1, char base2, char base3, const char* aas)
{
	int index = codon_index(base1, base2, base3);
	if (index < 0)
		return '-';
	return aas[index];
}

bool is_start(char base1, char base2, char base3, const char* starts)
{
	char aa = translate(base1, base2, base3, starts);
	return ((aa != '-') && (aa != '*'));
}

bool is_stop(char base1, char base2, char base3, const char* aas)
{
	char aa = translate(base1, base2, base3, aas);
	return (aa == '*');
}

} // namspace codon
} // namespace genetics
