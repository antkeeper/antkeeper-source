/*
 * Copyright (C) 2023  Christopher J. Howard
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

/*
 * Easing Functions (Equations)
 *
 * Copyright (C) 2001 Robert Penner
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the author nor the names of contributors may be used to
 *   endorse or promote products derived from this software without specific
 *   prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ANTKEEPER_EASE_HPP
#define ANTKEEPER_EASE_HPP

#include "math/interpolation.hpp"
#include <cmath>

/**
 * Container for templated easing functions.
 *
 * All easing functions require the following operators to be defined:
 *
 *     value_type operator+(const value_type&, const value_type&);
 *     value_type operator-(const value_type&, const value_type&);
 *     value_type operator*(const value_type&, scalar_type);
 *
 * @tparam T Value type.
 * @tparam S Scalar type.
 */
template <typename T, typename S = float>
struct ease
{
	typedef T value_type;
	typedef S scalar_type;
	
	static T in_sine(const T& x, const T& y, S a);
	static T out_sine(const T& x, const T& y, S a);
	static T in_out_sine(const T& x, const T& y, S a);
	
	static T in_quad(const T& x, const T& y, S a);
	static T out_quad(const T& x, const T& y, S a);
	static T in_out_quad(const T& x, const T& y, S a);
	
	static T in_cubic(const T& x, const T& y, S a);
	static T out_cubic(const T& x, const T& y, S a);
	static T in_out_cubic(const T& x, const T& y, S a);
	
	static T in_quart(const T& x, const T& y, S a);
	static T out_quart(const T& x, const T& y, S a);
	static T in_out_quart(const T& x, const T& y, S a);
	
	static T in_quint(const T& x, const T& y, S a);
	static T out_quint(const T& x, const T& y, S a);
	static T in_out_quint(const T& x, const T& y, S a);
	
	static T in_expo(const T& x, const T& y, S a);
	static T out_expo(const T& x, const T& y, S a);
	static T in_out_expo(const T& x, const T& y, S a);
	
	static T in_circ(const T& x, const T& y, S a);
	static T out_circ(const T& x, const T& y, S a);
	static T in_out_circ(const T& x, const T& y, S a);
	
	static T in_back(const T& x, const T& y, S a);
	static T out_back(const T& x, const T& y, S a);
	static T in_out_back(const T& x, const T& y, S a);
	
	static T in_elastic(const T& x, const T& y, S a);
	static T out_elastic(const T& x, const T& y, S a);
	static T in_out_elastic(const T& x, const T& y, S a);
	
	static T in_bounce(const T& x, const T& y, S a);
	static T out_bounce(const T& x, const T& y, S a);
	static T in_out_bounce(const T& x, const T& y, S a);
};

template <typename T, typename S>
T ease<T, S>::in_sine(const T& x, const T& y, S a)
{
	return math::lerp(y, x, std::cos(a * math::half_pi<S>));
}

template <typename T, typename S>
T ease<T, S>::out_sine(const T& x, const T& y, S a)
{
	return math::lerp(x, y, std::sin(a * math::half_pi<S>));
}

template <typename T, typename S>
T ease<T, S>::in_out_sine(const T& x, const T& y, S a)
{
	return math::lerp(x, y, -(std::cos(a * math::pi<S>) - S(1)) * S(0.5));
}

template <typename T, typename S>
T ease<T, S>::in_quad(const T& x, const T& y, S a)
{
	return math::lerp(x, y, a * a);
}

template <typename T, typename S>
T ease<T, S>::out_quad(const T& x, const T& y, S a)
{
	return math::lerp(x, y, (S(2) - a) * a);
}

template <typename T, typename S>
T ease<T, S>::in_out_quad(const T& x, const T& y, S a)
{
	return math::lerp(x, y, (a < S(0.5)) ? S(2) * a * a : -(S(2) * a * a - S(4) * a + S(1)));
}

template <typename T, typename S>
T ease<T, S>::in_cubic(const T& x, const T& y, S a)
{
	return math::lerp(x, y, a * a * a);
}

template <typename T, typename S>
T ease<T, S>::out_cubic(const T& x, const T& y, S a)
{
	return math::lerp(x, y, a * ((a - S(3)) * a + S(3)));	
}

template <typename T, typename S>
T ease<T, S>::in_out_cubic(const T& x, const T& y, S a)
{
	return math::lerp(x, y, (a < S(0.5)) ? S(4) * a * a * a : S(4) * a * a * a - S(12) * a * a + S(12) * a - 3);
}

template <typename T, typename S>
T ease<T, S>::in_quart(const T& x, const T& y, S a)
{
	return math::lerp(x, y, a * a * a * a);
}

template <typename T, typename S>
T ease<T, S>::out_quart(const T& x, const T& y, S a)
{
	return math::lerp(x, y, a * (a * ((S(4) - a) * a - S(6)) + S(4)));
}

template <typename T, typename S>
T ease<T, S>::in_out_quart(const T& x, const T& y, S a)
{
	return math::lerp(x, y, (a < S(0.5)) ? S(8) * a * a * a * a : a * (a * ((S(32) - S(8) * a) * a - S(48)) + S(32)) - S(7));
}

template <typename T, typename S>
T ease<T, S>::in_quint(const T& x, const T& y, S a)
{
	return math::lerp(x, y, a * a * a * a * a);
}

template <typename T, typename S>
T ease<T, S>::out_quint(const T& x, const T& y, S a)
{
	return math::lerp(x, y, a * (a * (a * ((a - S(5)) * a + S(10)) - S(10)) + S(5)));
}

template <typename T, typename S>
T ease<T, S>::in_out_quint(const T& x, const T& y, S a)
{
	if (a < S(0.5))
	{
		return math::lerp(x, y, S(16) * a * a * a * a * a);
	}
	else
	{
		a = S(2) * (S(1) - a);
		return math::lerp(x, y, S(0.5) * (S(2) - a * a * a * a * a));
	}
}

template <typename T, typename S>
T ease<T, S>::in_expo(const T& x, const T& y, S a)
{
	return (a == S(0)) ? x : math::lerp(x, y, std::pow(S(1024), a - S(1)));
}

template <typename T, typename S>
T ease<T, S>::out_expo(const T& x, const T& y, S a)
{
	return (a == S(1)) ? y : math::lerp(y, x, std::pow(S(2), S(-10) * a));
}

template <typename T, typename S>
T ease<T, S>::in_out_expo(const T& x, const T& y, S a)
{
	if (a == S(0))
	{
		return x;
	}
	else if (a == S(1))
	{
		return y;
	}
	
	return math::lerp(x, y, (a < S(0.5)) ? std::pow(S(2), S(20) * a - S(11)) : S(1) - std::pow(S(2), S(9) - S(20) * a));
}

template <typename T, typename S>
T ease<T, S>::in_circ(const T& x, const T& y, S a)
{
	return math::lerp(y, x, std::sqrt(S(1) - a * a));
}

template <typename T, typename S>
T ease<T, S>::out_circ(const T& x, const T& y, S a)
{
	return math::lerp(x, y, std::sqrt(-(a - S(2)) * a));
}

template <typename T, typename S>
T ease<T, S>::in_out_circ(const T& x, const T& y, S a)
{
	if (a < S(0.5))
	{
		return math::lerp(x, y, S(0.5) - S(0.5) * std::sqrt(S(1) - S(4) * a * a));
	}
	else
	{
		return math::lerp(x, y, S(0.5) * (std::sqrt(S(-4) * (a - S(2)) * a - S(3)) + S(1)));
	}
}

template <typename T, typename S>
T ease<T, S>::in_back(const T& x, const T& y, S a)
{
	const S c = S(1.70158);
	return math::lerp(x, y, a * a * (a * c + a - c));
}

template <typename T, typename S>
T ease<T, S>::out_back(const T& x, const T& y, S a)
{
	const S c = S(1.70158);
	a -= S(1);
	return math::lerp(x, y, a * a * (a * c + a + c) + S(1));
}

template <typename T, typename S>
T ease<T, S>::in_out_back(const T& x, const T& y, S a)
{
	const S c = S(1.70158) * S(1.525f);
	
	if (a < S(0.5))
	{
		return math::lerp(x, y, a * a * (a * (S(4) * c + S(4)) - S(2) * c));
	}
	else
	{
		S b = S(1) - S(2) * a;
		return math::lerp(x, y, b * b * (a * c + a - c * S(0.5) - S(1)) + S(1));
	}
}

template <typename T, typename S>
T ease<T, S>::in_elastic(const T& x, const T& y, S a)
{
	if (a == S(0))
	{
		return x;
	}
	else if (a == S(1))
	{
		return y;
	}
	
	return math::lerp(x, y, -std::pow(S(1024), a - S(1)) * std::sin(S(20.944) * (a - S(1.075))));
}

template <typename T, typename S>
T ease<T, S>::out_elastic(const T& x, const T& y, S a)
{
	if (a == S(0))
	{
		return x;
	}
	else if (a == S(1))
	{
		return y;
	}
	
	return math::lerp(x, y, std::pow(S(2), S(-10) * a) * std::sin(S(20.944) * (a - S(0.075))) + S(1));
}

template <typename T, typename S>
T ease<T, S>::in_out_elastic(const T& x, const T& y, S a)
{
	if (a == S(0))
	{
		return x;
	}
	else if (a == S(1))
	{
		return y;
	}
	
	if (a < S(0.5))
	{
		return math::lerp(x, y, std::pow(S(2), S(20) * a - S(11)) * std::sin(S(15.5334) - S(27.5293) * a));
	}
	else
	{
		return math::lerp(y, x, std::pow(2, S(9) - S(20) * a) * std::sin(S(15.5334) - S(27.5293) * a));
	}
}

template <typename T, typename S>
T ease<T, S>::in_bounce(const T& x, const T& y, S a)
{
	return math::lerp(x, y, S(1) - ease<S, S>::out_bounce(S(0), S(1), S(1) - a));
}

template <typename T, typename S>
T ease<T, S>::out_bounce(const T& x, const T& y, S a)
{
	const S n = S(7.5625);
	const S d = S(2.75);
	
	if (a < S(1) / d)
	{
		a = n * a * a;
	}
	else if (a < S(2) / d)
	{
		a -= S(1.5) / d;
		a = n * a * a + S(0.75);
	}
	else if (a < S(2.5) / d)
	{
		a -= S(2.25) / d;
		a = n * a * a + S(0.9375);
	}
	else
	{
		a -= S(2.625) / d;
		a = n * a * a + S(0.984375);
	}
	
	return math::lerp(x, y, a);
}

template <typename T, typename S>
T ease<T, S>::in_out_bounce(const T& x, const T& y, S a)
{
	if (a < S(0.5))
	{
		return math::lerp(x, y, (S(1) - ease<S, S>::out_bounce(S(0), S(1), S(1) - S(2) * a)) * S(0.5));
	}
	else
	{
		return math::lerp(x, y, (S(1) + ease<S, S>::out_bounce(S(0), S(1), S(2) * a - S(1))) * S(0.5));
	}
}

#endif // ANTKEEPER_EASE_HPP
