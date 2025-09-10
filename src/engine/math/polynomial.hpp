// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/functions.hpp>
#include <engine/math/constants.hpp>

namespace engine::math
{
	/// Evaluates a polynomial using Horner's method.
	/// @param first,last Range of polynomial coefficients, in descending order of degree.
	/// @param x Variable value.
	/// @return Evaluation of P(x).
	/// @see https://en.wikipedia.org/wiki/Horner%27s_method
	template <class InputIt, class T>
	[[nodiscard]] constexpr T horner(InputIt first, InputIt last, T x)
	{
		T y = *first;
		for (++first; first != last; ++first)
		{
			y = y * x + *first;
		}
		return y;
	}

	/// Evaluates a Chebyshev polynomial.
	/// @param[in] first,last Range of Chebychev polynomial coefficients.
	/// @param[in] x Value on the interval `[-1, 1]`.
	/// @return Evaluated value.
	template <class InputIt, class T>
	[[nodiscard]] T chebyshev(InputIt first, InputIt last, T x)
	{
		T y = *(first++);
		y += *(first++) * x;

		T n2 = T{1};
		T n1 = x;
		T n0;

		x += x;

		for (; first != last; n2 = n1, n1 = n0)
		{
			n0 = x * n1 - n2;
			y += *(first++) * n0;
		}

		return y;
	}

	/// Evaluates a Chebyshev polynomial.
	/// @param first,last Range of Chebychev polynomial coefficients.
	/// @param min,max Domain of the approximated function.
	/// @param x Value on the interval `[min, max]`.
	/// @return Evaluated value.
	template <class InputIt, class T>
	[[nodiscard]] T chebyshev(InputIt first, InputIt last, T min, T max, T x)
	{
		return evaluate<InputIt, T>(first, last, map_range<T>(x, min, max, T{-1}, T{1}));
	}
}
