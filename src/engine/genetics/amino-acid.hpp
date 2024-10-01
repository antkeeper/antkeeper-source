// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GENETICS_AMINO_ACID_HPP
#define ANTKEEPER_GENETICS_AMINO_ACID_HPP

#include <type_traits>

namespace genetics {

/// Functions which operate on IUPAC amino acid symbols.
namespace amino_acid {

/// Scores two amino acids using a substitution matrix.
/// @param a IUPAC amino acid code of first amino acid.
/// @param b IUPAC amino acid code of second amino acid.
/// @param matrix Substitution matrix.
/// @return Score of the two amino acids.
template <class Matrix>
typename std::remove_all_extents<Matrix>::type score(char a, char b, const Matrix& matrix)
{
	int i = (a < 'A' || a > 'Z') ? ((a == '*') ? 26 : -1) : a - 'A';
	int j = (b < 'A' || b > 'Z') ? ((b == '*') ? 26 : -1) : b - 'A';
	
	if (i < 0 || j < 0)
		return 0;
	
	return matrix[i][j];
}

} // namspace amino_acid
} // namespace genetics

#endif // ANTKEEPER_GENETICS_AMINO_ACID_HPP
