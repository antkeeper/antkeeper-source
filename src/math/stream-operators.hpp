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

#ifndef ANTKEEPER_MATH_STREAM_OPERATORS_HPP
#define ANTKEEPER_MATH_STREAM_OPERATORS_HPP

#include "math/vector-type.hpp"
#include "math/matrix-type.hpp"
#include "math/quaternion-type.hpp"
#include <ostream>

namespace math {
namespace stream_operators {

/// @addtogroup io
/// @{

/**
 * Writes the elements of a vector to an output stream, with each element delimeted by a space.
 *
 * @param os Output stream.
 * @param v Vector.
 * @return Output stream.
 */
template <class T, std::size_t N>
std::ostream& operator<<(std::ostream& os, const vector<T, N>& v);

/**
 * Writes the elements of a matrix to an output stream, with each element delimeted by a space.
 *
 * @param os Output stream.
 * @param m Matrix.
 * @return Output stream.
 */
template <class T, std::size_t N, std::size_t M>
std::ostream& operator<<(std::ostream& os, const matrix<T, N, M>& m);

/**
 * Writes the real and imaginary parts of a quaternion to an output stream, with each number delimeted by a space.
 *
 * @param os Output stream.
 * @param q Quaternion.
 * @return Output stream.
 */
template <class T>
std::ostream& operator<<(std::ostream& os, const quaternion<T>& q);

/**
 * Reads the elements of a vector from an input stream, with each element delimeted by a space.
 *
 * @param is Input stream.
 * @param v Vector.
 * @return Input stream.
 */
template <class T, std::size_t N>
std::istream& operator>>(std::istream& is, vector<T, N>& v);

/**
 * Reads the elements of a matrix from an input stream, with each element delimeted by a space.
 *
 * @param is Input stream.
 * @param m Matrix.
 * @return Input stream.
 */
template <class T, std::size_t N, std::size_t M>
std::istream& operator>>(std::istream& is, matrix<T, N, M>& m);

/**
 * Reads the real and imaginary parts of a quaternion from an input stream, with each number delimeted by a space.
 *
 * @param is Input stream.
 * @param q Quaternion.
 * @return Input stream.
 */
template <class T>
std::istream& operator>>(std::istream& is, const quaternion<T>& q);

template <class T, std::size_t N>
std::ostream& operator<<(std::ostream& os, const vector<T, N>& v)
{
	for (std::size_t i = 0; i < N; ++i)
	{
		os << v[i];

		if (i < N - 1)
		{
			os << ' ';
		}
	}

	return os;
}

template <class T, std::size_t N, std::size_t M>
std::ostream& operator<<(std::ostream& os, const matrix<T, N, M>& m)
{
	for (std::size_t i = 0; i < N; ++i)
	{
		for (std::size_t j = 0; j < M; ++j)
		{
			os << m[i][j];

			if (i < N - 1 || j < M - 1)
			{
				os << ' ';
			}
		}
	}

	return os;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const quaternion<T>& q)
{
	os << q.w << ' ' << q.x << ' ' << q.y << ' ' << q.z;
	return os;
}

template <class T, std::size_t N>
std::istream& operator>>(std::istream& is, vector<T, N>& v)
{
	for (std::size_t i = 0; i < N; ++i)
		is >> v[i];

	return is;
}

template <class T, std::size_t N, std::size_t M>
std::istream& operator>>(std::istream& is, matrix<T, N, M>& m)
{
	for (std::size_t i = 0; i < N * M; ++i)
	{
		std::size_t j = i / M;
		std::size_t k = i % M;
		is >> m[j][k];
	}

	return is;
}

template <class T>
std::istream& operator>>(std::istream& is, const quaternion<T>& q)
{
	is >> q.w;
	is >> q.x;
	is >> q.y;
	is >> q.z;
	
	return is;
}

/// @}

} // namespace stream_operators
} // namespace math

// Bring stream operators into global namespace
using namespace math::stream_operators;

#endif // ANTKEEPER_MATH_STREAM_OPERATORS_HPP
