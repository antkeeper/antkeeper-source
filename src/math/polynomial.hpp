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

#ifndef ANTKEEPER_MATH_POLYNOMIAL_HPP
#define ANTKEEPER_MATH_POLYNOMIAL_HPP

namespace math {

/// Polynomial evaluation functions.
namespace polynomial {

/**
 * Evaluates a polynomial using Horner's method.
 *
 * @param first,last Range of polynomial coefficients, in descending order of degree.
 * @param x Variable value.
 * @return Evaluation of P(x).
 *
 * @see https://en.wikipedia.org/wiki/Horner%27s_method
 */
template <class InputIt, class T>
T horner(InputIt first, InputIt last, T x)
{
	T sum = *first;
	for (++first; first != last; ++first)
		sum = sum * x + *first;
	return sum;
}

} // namespace polynomial
} // namespace math

#endif // ANTKEEPER_MATH_POLYNOMIAL_HPP
