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

#ifndef ANTKEEPER_GENETICS_TRANSLATION_TABLE_HPP
#define ANTKEEPER_GENETICS_TRANSLATION_TABLE_HPP

namespace genetics {

/**
 * Genetic code translation table.
 *
 * @see https://www.ncbi.nlm.nih.gov/Taxonomy/Utils/wprintgc.cgi
 */
struct translation_table
{
	/// String of 64 IUPAC amino acid base symbols, in TCAG order.
	const char* aas;
	
	/// String of 64 IUPAC amino acid base symbols, in TCAG order, where symbols other than `-` and `*` indicate a start codon and its amino acid.
	const char* starts;
};

/// Translation table for standard genetic code.
constexpr translation_table standard_code =
{
	"FFLLSSSSYY**CC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
	"---M------**--*----M---------------M----------------------------",
};

} // namespace genetics

#endif // ANTKEEPER_GENETICS_TRANSLATION_TABLE_HPP
