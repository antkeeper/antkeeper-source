// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GENETICS_PROTEIN_HPP
#define ANTKEEPER_GENETICS_PROTEIN_HPP

#include <engine/genetics/amino-acid.hpp>
#include <type_traits>

namespace genetics {

/// Functions which operate on sequences of IUPAC amino acid symbols.
namespace protein {

/// Returns the percent identity between two proteins.
/// @param first1,last1 Range of IUPAC amino acids which constitute the first protein.
/// @param first2 Beginning of the range of IUPAC amino acids which constitute the second protein.
/// @return Percent identity between the two proteins.
template <class T, class ForwardIt1, class ForwardIt2>
T identity(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2);

/// Scores two proteins using a substitution matrix.
/// @param first1,last1 Range of IUPAC amino acid codes which constitute the first protein.
/// @param first2 Beginning of the range of IUPAC amino acid codes which constitute the second protein.
/// @param matrix Substitution matrix.
/// @return Score of the two proteins.
template <class ForwardIt1, class ForwardIt2, class Matrix>
typename std::remove_all_extents<Matrix>::type score(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2, const Matrix& matrix);

/// Returns the percent similarity between two proteins.
/// @param first1,last1 Range of IUPAC amino acids which constitute the first protein.
/// @param first2 Beginning of the range of IUPAC amino acids which constitute the second protein.
/// @param matrix Substitution matrix.
/// @return Percent similarity between the two proteins.
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
