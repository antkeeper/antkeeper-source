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

#ifndef ANTKEEPER_MATH_MATRIX_OPERATORS_HPP
#define ANTKEEPER_MATH_MATRIX_OPERATORS_HPP

#include "math/matrix-type.hpp"
#include "math/matrix-functions.hpp"

namespace math {
namespace matrix_operators {

/// @addtogroup matrix
/// @{

/// @copydoc add(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&)
template <class T, std::size_t N, std::size_t M>
matrix<T, N, M> operator+(const matrix<T, N, M>& x, const matrix<T, N, M>& y);

/// @copydoc mul(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&)
template <class T, std::size_t N, std::size_t M>
matrix<T, N, M> operator*(const matrix<T, N, M>& x, const matrix<T, N, M>& y);

/// @copydoc mul(const matrix<T, N, M>&, T)
template <class T, std::size_t N, std::size_t M>
matrix<T, N, M> operator*(const matrix<T, N, M>& m, T s);

/// @copydoc mul(const matrix<T, N, M>&, T)
template <class T, std::size_t N, std::size_t M>
matrix<T, N, M> operator*(T s, const matrix<T, N, M>& m);

/// @copydoc mul(const matrix<T, 2, 2>&, const vector<T, 2>&)
template <class T, std::size_t N>
vector<T, N> operator*(const matrix<T, N, N>& m, const vector<T, N>& v);

/// @copydoc sub(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&)
template <class T, std::size_t N, std::size_t M>
matrix<T, N, M> operator-(const matrix<T, N, M>& x, const matrix<T, N, M>& y);

template <class T, std::size_t N, std::size_t M>
inline matrix<T, N, M> operator+(const matrix<T, N, M>& x, const matrix<T, N, M>& y)
{
	return add(x, y);
}

template <class T, std::size_t N, std::size_t M>
inline matrix<T, N, M> operator*(const matrix<T, N, M>& x, const matrix<T, N, M>& y)
{
	return mul(x, y);
}

template <class T, std::size_t N, std::size_t M>
inline matrix<T, N, M> operator*(const matrix<T, N, M>& m, T s)
{
	return mul(m, s);
}

template <class T, std::size_t N, std::size_t M>
inline matrix<T, N, M> operator*(T s, const matrix<T, N, M>& m)
{
	return mul(m, s);
}

template <class T, std::size_t N>
inline vector<T, N> operator*(const matrix<T, N, N>& m, const vector<T, N>& v)
{
	return mul(m, v);
}

template <class T, std::size_t N, std::size_t M>
inline matrix<T, N, M> operator-(const matrix<T, N, M>& x, const matrix<T, N, M>& y)
{
	return sub(x, y);
}

/// @}

} // namespace matrix_operators
} // namespace math

// Bring matrix operators into global namespace
using namespace math::matrix_operators;

#endif // ANTKEEPER_MATH_MATRIX_OPERATORS_HPP
