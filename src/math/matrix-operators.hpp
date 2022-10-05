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

#ifndef ANTKEEPER_MATH_MATRIX_OPERATORS_HPP
#define ANTKEEPER_MATH_MATRIX_OPERATORS_HPP

#include "math/matrix-type.hpp"
#include "math/matrix-functions.hpp"

/// @copydoc math::add(const math::matrix<T, 2, 2>&, const math::matrix<T, 2, 2>&)
template <class T, std::size_t N, std::size_t M>
constexpr math::matrix<T, N, M> operator+(const math::matrix<T, N, M>& x, const math::matrix<T, N, M>& y);

/// @copydoc math::mul(const math::matrix<T, 2, 2>&, const math::matrix<T, 2, 2>&)
template <class T, std::size_t N, std::size_t M>
constexpr math::matrix<T, N, M> operator*(const math::matrix<T, N, M>& x, const math::matrix<T, N, M>& y);

/// @copydoc math::mul(const math::matrix<T, N, M>&, T)
template <class T, std::size_t N, std::size_t M>
constexpr math::matrix<T, N, M> operator*(const math::matrix<T, N, M>& m, T s);

/// @copydoc math::mul(const math::matrix<T, N, M>&, T)
template <class T, std::size_t N, std::size_t M>
constexpr math::matrix<T, N, M> operator*(T s, const math::matrix<T, N, M>& m);

/// @copydoc math::mul(const math::matrix<T, 2, 2>&, const math::vector<T, 2>&)
template <class T, std::size_t N>
constexpr math::vector<T, N> operator*(const math::matrix<T, N, N>& m, const math::vector<T, N>& v);

/// @copydoc math::sub(const math::matrix<T, 2, 2>&, const math::matrix<T, 2, 2>&)
template <class T, std::size_t N, std::size_t M>
constexpr math::matrix<T, N, M> operator-(const math::matrix<T, N, M>& x, const math::matrix<T, N, M>& y);

template <class T, std::size_t N, std::size_t M>
constexpr inline math::matrix<T, N, M> operator+(const math::matrix<T, N, M>& x, const math::matrix<T, N, M>& y)
{
	return add(x, y);
}

template <class T, std::size_t N, std::size_t M>
constexpr inline math::matrix<T, N, M> operator*(const math::matrix<T, N, M>& x, const math::matrix<T, N, M>& y)
{
	return mul(x, y);
}

template <class T, std::size_t N, std::size_t M>
constexpr inline math::matrix<T, N, M> operator*(const math::matrix<T, N, M>& m, T s)
{
	return mul(m, s);
}

template <class T, std::size_t N, std::size_t M>
constexpr inline math::matrix<T, N, M> operator*(T s, const math::matrix<T, N, M>& m)
{
	return mul(m, s);
}

template <class T, std::size_t N>
constexpr inline math::vector<T, N> operator*(const math::matrix<T, N, N>& m, const math::vector<T, N>& v)
{
	return mul(m, v);
}

template <class T, std::size_t N, std::size_t M>
constexpr inline math::matrix<T, N, M> operator-(const math::matrix<T, N, M>& x, const math::matrix<T, N, M>& y)
{
	return sub(x, y);
}

#endif // ANTKEEPER_MATH_MATRIX_OPERATORS_HPP
