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

#include "math/constants.hpp"
#include "math/map.hpp"

namespace math {

/// Polynomial functions.
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
	T y = *first;
	for (++first; first != last; ++first)
		y = y * x + *first;
	return y;
}

/** Chebychev polynomials.
 *
 * @see https://en.wikipedia.org/wiki/Chebyshev_polynomials
 */
namespace chebyshev {

	/**
	 * Generates a Chebyshev approximation of a function.
	 *
	 * @param[out] first,last Range of Chebyshev polynomial coefficients.
	 * @param[in] f Unary function to approximate.
	 * @param[in] min,max Domain of @p f.
	 */
	template <class OutputIt, class UnaryOp, class T>
	void approximate(OutputIt first, OutputIt last, UnaryOp f, T min, T max)
	{
		std::size_t n = last - first;
		const T two_over_n = T(2) / static_cast<T>(n);
		const T pi_over_n = math::pi<T> / static_cast<T>(n);
		
		last = first;
		for (std::size_t i = 0; i < n; ++i)
			*(last++) = T(0);
		
		for (std::size_t i = 0; i < n; ++i)
		{
			const T y = pi_over_n * (static_cast<T>(i) + T(0.5));
			
			T x = f(math::map<T>(std::cos(y), T(-1), T(1), min, max)) * two_over_n;
			
			*first += x;
			last = first;
			for (std::size_t j = 1; j < n; ++j)
			{
				*(++last) += x * std::cos(y * static_cast<T>(j));
			}
		}
	}

	/**
	 * Evaluates a Chebyshev polynomial.
	 *
	 * @param[in] first,last Range of Chebychev polynomial coefficients.
	 * @param[in] x Value on the interval `[-1, 1]`.
	 *
	 * @return Evaluated value.
	 */
	template <class InputIt, class T>
	T evaluate(InputIt first, InputIt last, T x)
	{
		T y = *(first++) * T(0.5) + *(first++) * x;
		
		const T x2 = x * T(2);
		for (T n2 = T(1), n1 = x, n0; first != last; n2 = n1, n1 = n0)
		{
			n0 = x2 * n1 - n2;
			y += *(first++) * n0;
		}
		
		return y;
	}

	/**
	 * Evaluates a Chebyshev polynomial.
	 *
	 * @param first,last Range of Chebychev polynomial coefficients.
	 * @param min,max Domain of the approximated function.
	 * @param x Value on the interval `[min, max]`.
	 *
	 * @return Evaluated value.
	 */
	template <class InputIt, class T>
	T evaluate(InputIt first, InputIt last, T min, T max, T x)
	{
		return evaluate<InputIt, T>(first, last, math::map<T>(x, min, max, T(-1), T(1)));
	}

} // namespace chebyshev

} // namespace polynomial
} // namespace math

#endif // ANTKEEPER_MATH_POLYNOMIAL_HPP