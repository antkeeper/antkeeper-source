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

#ifndef ANTKEEPER_MATH_QUADRATURE_HPP
#define ANTKEEPER_MATH_QUADRATURE_HPP

#include <iterator>
#include <type_traits>

namespace math {

/// Numerical integration functions.
namespace quadrature {

/**
 * Approximates the definite integral of a function using the trapezoidal rule.
 *
 * @param f Unary function object to integrate.
 * @param first,last Range of sample points on `[first, last)`.
 * @return Approximated integral of @p f.
 *
 * @see https://en.wikipedia.org/wiki/Trapezoidal_rule
 */
template<class UnaryOp, class InputIt>
typename std::invoke_result<UnaryOp, typename std::iterator_traits<InputIt>::value_type>::type trapezoid(UnaryOp f, InputIt first, InputIt last)
{
	typedef typename std::invoke_result<UnaryOp, typename std::iterator_traits<InputIt>::value_type>::type result_type;
	
	if (first == last)
		return result_type(0);
	
	result_type f_a = f(*first);
	
	InputIt second = first;
	++second;
	
	if (second == last)
		return f_a;
	
	result_type f_b = f(*second);
	result_type sum = (f_a + f_b) / result_type(2) * (*second - *first);
	
	for (first = second++; second != last; first = second++)
	{
		f_a = f_b;
		f_b = f(*second);
		sum += (f_a + f_b) / result_type(2) * (*second - *first);
	}
	
	return sum;
}

} // namespace quadrature

} // namespace math

#endif // ANTKEEPER_MATH_QUADRATURE_HPP
