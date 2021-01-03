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

#ifndef ANTKEEPER_GENETICS_BASE_HPP
#define ANTKEEPER_GENETICS_BASE_HPP

namespace genetics {

/// Functions which operate on IUPAC degenerate base symbols.
namespace base {

/**
 * Returns the number of bases that are represented by both IUPAC degenerate base symbols.
 *
 * @param a First IUPAC degenerate base symbol.
 * @param b Second IUPAC degenerate base symbol.
 * @return Number of bases represented by both symbols.
 */
int compare(char a, char b);

/**
 * Transcribes an IUPAC degenerate base symbol between DNA and RNA, swapping `T` for `U` or `U` for `T`.
 *
 * @param symbol IUPAC degenerate base symbol.
 * @return `U` if @p symbol was `T`, `T` if @p symbol was `U`, or `symbol` if @p symbol was neither `T` nor `U`.
 */
char transcribe(char symbol);

/// Functions which operate on IUPAC degenerate **DNA** base symbols.
namespace dna
{
	/**
	 * Returns the DNA complement of an IUPAC degenerate base symbol.
	 *
	 * @param symbol IUPAC degenerate base symbol.
	 * @return IUPAC degenerate base symbol of DNA complement.
	 */
	char complement(char symbol);
}

/// Functions which operate on IUPAC degenerate **RNA** base symbols.
namespace rna
{
	/**
	 * Returns the RNA complement of an IUPAC degenerate base symbol.
	 *
	 * @param symbol IUPAC degenerate base symbol.
	 * @return IUPAC degenerate base symbol of RNA complement.
	 */
	char complement(char symbol);	
}

} // namespace base
} // namespace genetics

#endif // ANTKEEPER_GENETICS_BASE_HPP
