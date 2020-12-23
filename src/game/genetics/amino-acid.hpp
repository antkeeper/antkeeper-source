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

#ifndef ANTKEEPER_GENETICS_AMINO_ACID_HPP
#define ANTKEEPER_GENETICS_AMINO_ACID_HPP

#include <type_traits>

namespace genetics {
namespace amino_acid {

/**
 * Scores two amino acids using a substitution matrix.
 *
 * @param a IUPAC amino acid code of first amino acid.
 * @param b IUPAC amino acid code of second amino acid.
 * @param matrix Substitution matrix.
 * @return Score of the two amino acids.
 */
template <class Matrix>
typename std::remove_all_extents<Matrix>::type score(char a, char b, const Matrix& matrix);

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
