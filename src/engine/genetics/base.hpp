// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
