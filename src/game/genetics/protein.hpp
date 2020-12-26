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

#ifndef ANTKEEPER_GENETICS_PROTEIN_HPP
#define ANTKEEPER_GENETICS_PROTEIN_HPP

#include "amino-acid.hpp"
#include <type_traits>

namespace genetics {
namespace protein {

/**
 * Returns the percent identity between two proteins.
 *
 * @param first1,last1 Range of IUPAC amino acids which constitute the first protein.
 * @param first2 Beginning of the range of IUPAC amino acids which constitute the second protein.
 * @return Percent identity between the two proteins.
 */
template <class T, class ForwardIt1, class ForwardIt2>
T identity(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2);

/**
 * Scores two proteins using a substitution matrix.
 *
 * @param first1,last1 Range of IUPAC amino acid codes which constitute the first protein.
 * @param first2 Beginning of the range of IUPAC amino acid codes which constitute the second protein.
 * @param matrix Substitution matrix.
 * @return Score of the two proteins.
 */
template <class ForwardIt1, class ForwardIt2, class Matrix>
typename std::remove_all_extents<Matrix>::type score(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2, const Matrix& matrix);

/**
 * Returns the percent similarity between two proteins.
 *
 * @param first1,last1 Range of IUPAC amino acids which constitute the first protein.
 * @param first2 Beginning of the range of IUPAC amino acids which constitute the second protein.
 * @return Percent similarity between the two proteins.
 */
template <class T, class ForwardIt1, class ForwardIt2, class Matrix>
typename T similarity(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2, const Matrix& matrix);

template <class T, class ForwardIt1, class ForwardIt2>
T identity(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2)
{
	auto length = std::distance(first1, last1);
	
	T sum = 0;
	for (; first1 != last1; ++first1, ++first2)
		if (*first1 == *first2)
			++sum;
	
	return sum / static_cast<T>(length);
}

template <class ForwardIt1, class ForwardIt2, class Matrix>
typename std::remove_all_extents<Matrix>::type score(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2, const Matrix& matrix)
{
	typename std::remove_all_extents<Matrix>::type result = 0;
	for (; first1 != last1; ++first1, ++first2)
		result += amino_acid::score(*first1, *first2, matrix);
	return result;
}

template <class T, class ForwardIt1, class ForwardIt2, class Matrix>
typename T similarity(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2, const Matrix& matrix)
{
	T length = static_cast<T>(std::distance(first1, last1));
	T positive_count = T(0);
	
	for (; first1 != last1; ++first1, ++first2)
		if (amino_acid::score(*first1, *first2, matrix) > 0)
			++positive_count;
	
	return positive_count / length;
}

} // namespace protein
} // namespace genetics

#endif // ANTKEEPER_GENETICS_PROTEIN_HPP
