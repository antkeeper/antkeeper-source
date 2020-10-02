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

#ifndef ANTKEEPER_MATH_MATRIX_FUNCTIONS_HPP
#define ANTKEEPER_MATH_MATRIX_FUNCTIONS_HPP

#include "math/matrix-type.hpp"
#include "math/vector-type.hpp"
#include "math/vector-functions.hpp"
#include <type_traits>

namespace math {

/// @addtogroup matrix
/// @{

/**
 * Adds two matrices.
 *
 * @param x First matrix.
 * @param y Second matrix.
 * @return Sum of the two matrices.
 */
template <class T>
matrix<T, 2, 2> add(const matrix<T, 2, 2>& x, const matrix<T, 2, 2>& y);

/// @copydoc add(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&)
template <class T>
matrix<T, 3, 3> add(const matrix<T, 3, 3>& x, const matrix<T, 3, 3>& y);

/// @copydoc add(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&)
template <class T>
matrix<T, 4, 4> add(const matrix<T, 4, 4>& x, const matrix<T, 4, 4>& y);

/**
 * Reinterprets data as an `NxM` matrix of type `T`.
 *
 * @tparam N Number of columns.
 * @tparam M Number of rows.
 * @tparam T Element type.
 * @param data Data to reinterpret.
 */
template <std::size_t N, std::size_t M, typename T>
matrix<T, N, M>& as_matrix(T& data);

/**
 * Calculates the determinant of a matrix.
 *
 * @param m Matrix of which to take the determinant. 
 */
template <class T>
T determinant(const matrix<T, 2, 2>& m);

/// @copydoc determinant(const matrix<T, 2, 2>&)
template <class T>
T determinant(const matrix<T, 3, 3>& m);

/// @copydoc determinant(const matrix<T, 2, 2>&)
template <class T>
T determinant(const matrix<T, 4, 4>& m);

/**
 * Calculates the inverse of a matrix.
 *
 * @param m Matrix of which to take the inverse.
 */
template <class T>
matrix<T, 2, 2> inverse(const matrix<T, 2, 2>& m);

/// @copydoc inverse(const matrix<T, 2, 2>&)
template <class T>
matrix<T, 3, 3> inverse(const matrix<T, 3, 3>& m);

/// @copydoc inverse(const matrix<T, 2, 2>&)
template <class T>
matrix<T, 4, 4> inverse(const matrix<T, 4, 4>& m);

/**
 * Performs a component-wise multiplication of two matrices.
 *
 * @param x First matrix multiplicand.
 * @param y Second matrix multiplicand.
 */
template <class T>
matrix<T, 2, 2> componentwise_mul(const matrix<T, 2, 2>& x, const matrix<T, 2, 2>& y);

/// @copydoc componentwise_mul(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&)
template <class T>
matrix<T, 3, 3> componentwise_mul(const matrix<T, 3, 3>& x, const matrix<T, 3, 3>& y);

/// @copydoc componentwise_mul(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&)
template <class T>
matrix<T, 4, 4> componentwise_mul(const matrix<T, 4, 4>& x, const matrix<T, 4, 4>& y);

/**
 * Creates a viewing transformation matrix.
 *
 * @param position Position of the view point.
 * @param target Position of the target.
 * @param up Normalized direction of the up vector.
 * @return Viewing transformation matrix.
 */
template <class T>
matrix<T, 4, 4> look_at(const vector<T, 3>& position, const vector<T, 3>& target, vector<T, 3> up);

/**
 * Multiplies two matrices.
 *
 * @param x First matrix.
 * @param y Second matrix.
 * @return Product of the two matrices.
 */
template <class T>
matrix<T, 2, 2> mul(const matrix<T, 2, 2>& x, const matrix<T, 2, 2>& y);

/// @copydoc mul(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&);
template <class T>
matrix<T, 3, 3> mul(const matrix<T, 3, 3>& x, const matrix<T, 3, 3>& y);

/// @copydoc mul(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&);
template <class T>
matrix<T, 4, 4> mul(const matrix<T, 4, 4>& x, const matrix<T, 4, 4>& y);

/**
 * Multiplies a matrix by a scalar.
 *
 * @param m Matrix.
 * @param s Scalar.
 * @return Product of the matrix and the scalar..
 */
template <class T, std::size_t N, std::size_t M>
matrix<T, N, M> mul(const matrix<T, N, M>& m, T s);

/**
 * Transforms a vector by a matrix.
 *
 * @param m Matrix.
 * @param v Vector.
 * @return Transformed vector.
 */
template <class T>
vector<T, 2> mul(const matrix<T, 2, 2>& m, const vector<T, 2>& v);

/// @copydoc mul(const matrix<T, 2, 2>&, const vector<T, 2>&)
template <class T>
vector<T, 3> mul(const matrix<T, 3, 3>& m, const vector<T, 3>& v);

/// @copydoc mul(const matrix<T, 2, 2>&, const vector<T, 2>&)
template <class T>
vector<T, 4> mul(const matrix<T, 4, 4>& m, const vector<T, 4>& v);

/**
 * Creates an orthographic projection matrix which will transform the near and far clipping planes to `[-1, 1]`, respectively.
 *
 * @param left Signed distance to the left clipping plane.
 * @param right Signed distance to the right clipping plane.
 * @param bottom Signed distance to the bottom clipping plane.
 * @param top Signed distance to the top clipping plane.
 * @param z_near Signed distance to the near clipping plane.
 * @param z_far Signed distance to the far clipping plane.
 * @return Orthographic projection matrix.
 */
template <class T>
matrix<T, 4, 4> ortho(T left, T right, T bottom, T top, T z_near, T z_far);

/**
 * Creates an orthographic projection matrix which will transform the near and far clipping planes to `[0, 1]`, respectively.
 *
 * @param left Signed distance to the left clipping plane.
 * @param right Signed distance to the right clipping plane.
 * @param bottom Signed distance to the bottom clipping plane.
 * @param top Signed distance to the top clipping plane.
 * @param z_near Signed distance to the near clipping plane.
 * @param z_far Signed distance to the far clipping plane.
 * @return Orthographic projection matrix.
 */
template <class T>
matrix<T, 4, 4> ortho_half_z(T left, T right, T bottom, T top, T z_near, T z_far);

/**
 * Calculates the outer product of a pair of vectors.
 *
 * @param c Parameter to be treated as a column vector.
 * @param r Parameter to be treated as a row vector.
 */
template <class T>
matrix<T, 2, 2> outer_product(const vector<T, 2>& c, const vector<T, 2>& r);

/// @copydoc outer_product(const vector<T, 2>&, const vector<T, 2>&)
template <class T>
matrix<T, 3, 3> outer_product(const vector<T, 3>& c, const vector<T, 3>& r);

/// @copydoc outer_product(const vector<T, 2>&, const vector<T, 2>&)
template <class T>
matrix<T, 4, 4> outer_product(const vector<T, 4>& c, const vector<T, 4> r);

/**
 * Creates a perspective projection matrix which will transform the near and far clipping planes to `[-1, 1]`, respectively.
 *
 * @param vertical_fov Vertical field of view angle, in radians.
 * @param aspect_ratio Aspect ratio which determines the horizontal field of view.
 * @param z_near Distance to the near clipping plane.
 * @param z_far Distance to the far clipping plane.
 * @return Perspective projection matrix.
 */
template <class T>
matrix<T, 4, 4> perspective(T vertical_fov, T aspect_ratio, T z_near, T z_far);

/**
 * Creates a perspective projection matrix which will transform the near and far clipping planes to `[0, 1]`, respectively.
 *
 * @param vertical_fov Vertical field of view angle, in radians.
 * @param aspect_ratio Aspect ratio which determines the horizontal field of view.
 * @param z_near Distance to the near clipping plane.
 * @param z_far Distance to the far clipping plane.
 * @return Perspective projection matrix.
 */
template <class T>
matrix<T, 4, 4> perspective_half_z(T vertical_fov, T aspect_ratio, T z_near, T z_far);

/**
 * Resizes a matrix. Any new elements will be set to `1` if in the diagonal, and `0` otherwise.
 *
 * @param m Matrix to resize.
 * @return Resized matrix.
 */
template <std::size_t N1, std::size_t M1, class T, std::size_t N0, std::size_t M0>
matrix<T, N1, M1> resize(const matrix<T, N0, M0>& m);

/**
 * Rotates a matrix.
 *
 * @param m Matrix to rotate.
 * @param angle Angle of rotation (in radians).
 * @param axis Axis of rotation
 * @return Rotated matrix.
 */
template <class T>
matrix<T, 4, 4> rotate(const matrix<T, 4, 4>& m, T angle, const vector<T, 3>& axis);

/**
 * Scales a matrix.
 *
 * @param m Matrix to scale.
 * @param v Scale vector.
 * @return Scaled matrix.
 */
template <class T>
matrix<T, 4, 4> scale(const matrix<T, 4, 4>& m, const vector<T, 3>& v);

/**
 * Subtracts a matrix from another matrix.
 *
 * @param x First matrix.
 * @param y Second matrix.
 * @return Difference between the two matrices.
 */
template <class T>
matrix<T, 2, 2> sub(const matrix<T, 2, 2>& x, const matrix<T, 2, 2>& y);

/// @copydoc sub(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&)
template <class T>
matrix<T, 3, 3> sub(const matrix<T, 3, 3>& x, const matrix<T, 3, 3>& y);

/// @copydoc sub(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&)
template <class T>
matrix<T, 4, 4> sub(const matrix<T, 4, 4>& x, const matrix<T, 4, 4>& y);

/**
 * Translates a matrix.
 *
 * @param m Matrix to translate.
 * @param v Translation vector.
 * @return Translated matrix.
 */
template <class T>
matrix<T, 4, 4> translate(const matrix<T, 4, 4>& m, const vector<T, 3>& v);

/**
 * Calculates the transpose of a matrix.
 *
 * @param m Matrix of which to take the transpose.
 */
template <class T>
matrix<T, 2, 2> transpose(const matrix<T, 2, 2>& m);

/// @copydoc transpose(const matrix<T, 2, 2>&)
template <class T>
matrix<T, 3, 3> transpose(const matrix<T, 3, 3>& m);

/// @copydoc transpose(const matrix<T, 2, 2>&)
template <class T>
matrix<T, 4, 4> transpose(const matrix<T, 4, 4>& m);

template <class T>
matrix<T, 2, 2> add(const matrix<T, 2, 2>& x, const matrix<T, 2, 2>& y)
{
	return
		{{
			x[0] + y[0],
			x[1] + y[1]
		}};
}

template <class T>
matrix<T, 3, 3> add(const matrix<T, 3, 3>& x, const matrix<T, 3, 3>& y)
{
	return
		{{
			x[0] + y[0],
			x[1] + y[1],
			x[2] + y[2]
		}};
}

template <class T>
matrix<T, 4, 4> add(const matrix<T, 4, 4>& x, const matrix<T, 4, 4>& y)
{
	return
		{{
			x[0] + y[0],
			x[1] + y[1],
			x[2] + y[2],
			x[3] + y[3]
		}};
}

template <std::size_t N, std::size_t M, typename T>
inline matrix<T, N, M>& as_matrix(T& data)
{
	static_assert(std::is_pod<matrix<T, N, M>>::value);
	return reinterpret_cast<matrix<T, N, M>&>(data);
}

template <class T>
T determinant(const matrix<T, 2, 2>& m)
{
	return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

template <class T>
T determinant(const matrix<T, 3, 3>& m)
{
	return m[0][0] * m [1][1] * m[2][2] +
		m[0][1] * m[1][2] * m[2][0] +
		m[0][2] * m[1][0] * m[2][1] -
		m[0][0] * m[1][2] * m[2][1] -
		m[0][1] * m[1][0] * m[2][2] -
		m[0][2] * m[1][1] * m[2][0];
}

template <class T>
T determinant(const matrix<T, 4, 4>& m)
{
	return m[0][3] * m[1][2] * m[2][1] * m[3][0] - m[0][2] * m[1][3] * m[2][1] * m[3][0] -
		m[0][3] * m[1][1] * m[2][2] * m[3][0] + m[0][1] * m[1][3] * m[2][2] * m[3][0] +
		m[0][2] * m[1][1] * m[2][3] * m[3][0] - m[0][1] * m[1][2] * m[2][3] * m[3][0] -
		m[0][3] * m[1][2] * m[2][0] * m[3][1] + m[0][2] * m[1][3] * m[2][0] * m[3][1] +
		m[0][3] * m[1][0] * m[2][2] * m[3][1] - m[0][0] * m[1][3] * m[2][2] * m[3][1] -
		m[0][2] * m[1][0] * m[2][3] * m[3][1] + m[0][0] * m[1][2] * m[2][3] * m[3][1] +
		m[0][3] * m[1][1] * m[2][0] * m[3][2] - m[0][1] * m[1][3] * m[2][0] * m[3][2] -
		m[0][3] * m[1][0] * m[2][1] * m[3][2] + m[0][0] * m[1][3] * m[2][1] * m[3][2] +
		m[0][1] * m[1][0] * m[2][3] * m[3][2] - m[0][0] * m[1][1] * m[2][3] * m[3][2] -
		m[0][2] * m[1][1] * m[2][0] * m[3][3] + m[0][1] * m[1][2] * m[2][0] * m[3][3] +
		m[0][2] * m[1][0] * m[2][1] * m[3][3] - m[0][0] * m[1][2] * m[2][1] * m[3][3] -
		m[0][1] * m[1][0] * m[2][2] * m[3][3] + m[0][0] * m[1][1] * m[2][2] * m[3][3];
}

template <class T>
matrix<T, 2, 2> inverse(const matrix<T, 2, 2>& m)
{
	static_assert(std::is_floating_point<T>::value);

	const T rd(T(1) / determinant(m));
	return
		{{
			{ m[1][1] * rd, -m[0][1] * rd},
			{-m[1][0] * rd,  m[0][0] * rd}
		}};
}

template <class T>
matrix<T, 3, 3> inverse(const matrix<T, 3, 3>& m)
{
	static_assert(std::is_floating_point<T>::value);

	const T rd(T(1) / determinant(m));

	return
		{{
			{
				(m[1][1] * m[2][2] - m[1][2] * m[2][1]) * rd,
				(m[0][2] * m[2][1] - m[0][1] * m[2][2]) * rd,
				(m[0][1] * m[1][2] - m[0][2] * m[1][1]) * rd
			},

			{
				(m[1][2] * m[2][0] - m[1][0] * m[2][2]) * rd,
				(m[0][0] * m[2][2] - m[0][2] * m[2][0]) * rd,
				(m[0][2] * m[1][0] - m[0][0] * m[1][2]) * rd
			},

			{
				(m[1][0] * m[2][1] - m[1][1] * m[2][0]) * rd,
				(m[0][1] * m[2][0] - m[0][0] * m[2][1]) * rd,
				(m[0][0] * m[1][1] - m[0][1] * m[1][0]) * rd
			}
		}};
}

template <class T>
matrix<T, 4, 4> inverse(const matrix<T, 4, 4>& m)
{
	static_assert(std::is_floating_point<T>::value);

	const T rd(T(1) / determinant(m));

	return
		{{
			{
				(m[1][2] * m[2][3] * m[3][1] - m[1][3] * m[2][2] * m[3][1] + m[1][3] * m[2][1] * m[3][2] - m[1][1] * m[2][3] * m[3][2] - m[1][2] * m[2][1] * m[3][3] + m[1][1] * m[2][2] * m[3][3]) * rd,
				(m[0][3] * m[2][2] * m[3][1] - m[0][2] * m[2][3] * m[3][1] - m[0][3] * m[2][1] * m[3][2] + m[0][1] * m[2][3] * m[3][2] + m[0][2] * m[2][1] * m[3][3] - m[0][1] * m[2][2] * m[3][3]) * rd,
				(m[0][2] * m[1][3] * m[3][1] - m[0][3] * m[1][2] * m[3][1] + m[0][3] * m[1][1] * m[3][2] - m[0][1] * m[1][3] * m[3][2] - m[0][2] * m[1][1] * m[3][3] + m[0][1] * m[1][2] * m[3][3]) * rd,
				(m[0][3] * m[1][2] * m[2][1] - m[0][2] * m[1][3] * m[2][1] - m[0][3] * m[1][1] * m[2][2] + m[0][1] * m[1][3] * m[2][2] + m[0][2] * m[1][1] * m[2][3] - m[0][1] * m[1][2] * m[2][3]) * rd
			},

			{
				(m[1][3] * m[2][2] * m[3][0] - m[1][2] * m[2][3] * m[3][0] - m[1][3] * m[2][0] * m[3][2] + m[1][0] * m[2][3] * m[3][2] + m[1][2] * m[2][0] * m[3][3] - m[1][0] * m[2][2] * m[3][3]) * rd,
				(m[0][2] * m[2][3] * m[3][0] - m[0][3] * m[2][2] * m[3][0] + m[0][3] * m[2][0] * m[3][2] - m[0][0] * m[2][3] * m[3][2] - m[0][2] * m[2][0] * m[3][3] + m[0][0] * m[2][2] * m[3][3]) * rd,
				(m[0][3] * m[1][2] * m[3][0] - m[0][2] * m[1][3] * m[3][0] - m[0][3] * m[1][0] * m[3][2] + m[0][0] * m[1][3] * m[3][2] + m[0][2] * m[1][0] * m[3][3] - m[0][0] * m[1][2] * m[3][3]) * rd,
				(m[0][2] * m[1][3] * m[2][0] - m[0][3] * m[1][2] * m[2][0] + m[0][3] * m[1][0] * m[2][2] - m[0][0] * m[1][3] * m[2][2] - m[0][2] * m[1][0] * m[2][3] + m[0][0] * m[1][2] * m[2][3]) * rd
			},

			{
				(m[1][1] * m[2][3] * m[3][0] - m[1][3] * m[2][1] * m[3][0] + m[1][3] * m[2][0] * m[3][1] - m[1][0] * m[2][3] * m[3][1] - m[1][1] * m[2][0] * m[3][3] + m[1][0] * m[2][1] * m[3][3]) * rd,
				(m[0][3] * m[2][1] * m[3][0] - m[0][1] * m[2][3] * m[3][0] - m[0][3] * m[2][0] * m[3][1] + m[0][0] * m[2][3] * m[3][1] + m[0][1] * m[2][0] * m[3][3] - m[0][0] * m[2][1] * m[3][3]) * rd,
				(m[0][1] * m[1][3] * m[3][0] - m[0][3] * m[1][1] * m[3][0] + m[0][3] * m[1][0] * m[3][1] - m[0][0] * m[1][3] * m[3][1] - m[0][1] * m[1][0] * m[3][3] + m[0][0] * m[1][1] * m[3][3]) * rd,
				(m[0][3] * m[1][1] * m[2][0] - m[0][1] * m[1][3] * m[2][0] - m[0][3] * m[1][0] * m[2][1] + m[0][0] * m[1][3] * m[2][1] + m[0][1] * m[1][0] * m[2][3] - m[0][0] * m[1][1] * m[2][3]) * rd
			},

			{
				(m[1][2] * m[2][1] * m[3][0] - m[1][1] * m[2][2] * m[3][0] - m[1][2] * m[2][0] * m[3][1] + m[1][0] * m[2][2] * m[3][1] + m[1][1] * m[2][0] * m[3][2] - m[1][0] * m[2][1] * m[3][2]) * rd,
				(m[0][1] * m[2][2] * m[3][0] - m[0][2] * m[2][1] * m[3][0] + m[0][2] * m[2][0] * m[3][1] - m[0][0] * m[2][2] * m[3][1] - m[0][1] * m[2][0] * m[3][2] + m[0][0] * m[2][1] * m[3][2]) * rd,
				(m[0][2] * m[1][1] * m[3][0] - m[0][1] * m[1][2] * m[3][0] - m[0][2] * m[1][0] * m[3][1] + m[0][0] * m[1][2] * m[3][1] + m[0][1] * m[1][0] * m[3][2] - m[0][0] * m[1][1] * m[3][2]) * rd,
				(m[0][1] * m[1][2] * m[2][0] - m[0][2] * m[1][1] * m[2][0] + m[0][2] * m[1][0] * m[2][1] - m[0][0] * m[1][2] * m[2][1] - m[0][1] * m[1][0] * m[2][2] + m[0][0] * m[1][1] * m[2][2]) * rd
			}
		}};
}

template <class T>
matrix<T, 2, 2> componentwise_mul(const matrix<T, 2, 2>& x, const matrix<T, 2, 2>& y)
{
	return
		{{
			 {x[0][0] * y[0][0], x[0][1] * y[0][1]},
			 {x[1][0] * y[1][0], x[1][1] * y[1][1]}
		}};
}

template <class T>
matrix<T, 3, 3> componentwise_mul(const matrix<T, 3, 3>& x, const matrix<T, 3, 3>& y)
{
	return
		{{
			 {x[0][0] * y[0][0], x[0][1] * y[0][1], x[0][2] * y[0][2]},
			 {x[1][0] * y[1][0], x[1][1] * y[1][1], x[1][2] * y[1][2]},
			 {x[2][0] * y[2][0], x[2][1] * y[2][1], x[2][2] * y[2][2]}
		}};
}

template <class T>
matrix<T, 4, 4> componentwise_mul(const matrix<T, 4, 4>& x, const matrix<T, 4, 4>& y)
{
	return
		{{
			 {x[0][0] * y[0][0], x[0][1] * y[0][1], x[0][2] * y[0][2], x[0][3] * y[0][3]},
			 {x[1][0] * y[1][0], x[1][1] * y[1][1], x[1][2] * y[1][2], x[1][3] * y[1][3]},
			 {x[2][0] * y[2][0], x[2][1] * y[2][1], x[2][2] * y[2][2], x[2][3] * y[2][3]},
			 {x[3][0] * y[3][0], x[3][1] * y[3][1], x[3][2] * y[3][2], x[3][3] * y[3][3]}
		}};
}

template <class T>
matrix<T, 4, 4> look_at(const vector<T, 3>& position, const vector<T, 3>& target, vector<T, 3> up)
{
	vector<T, 3> forward = normalize(sub(target, position));
	vector<T, 3> right = normalize(cross(forward, up));
	up = cross(right, forward);

	matrix<T, 4, 4> m = 
		{{
			{right[0], up[0], -forward[0], T(0)},
			{right[1], up[1], -forward[1], T(0)},
			{right[2], up[2], -forward[2], T(0)},
			{T(0), T(0), T(0), T(1)}
		}};
	
	return translate(m, negate(position));
}

template <class T>
matrix<T, 2, 2> mul(const matrix<T, 2, 2>& x, const matrix<T, 2, 2>& y)
{
	return
		{{
			x[0] * y[0][0] + x[1] * y[0][1],
			x[0] * y[1][0] + x[1] * y[1][1]
		}};
}

template <class T>
matrix<T, 3, 3> mul(const matrix<T, 3, 3>& x, const matrix<T, 3, 3>& y)
{
	return
		{{
			x[0] * y[0][0] + x[1] * y[0][1] + x[2] * y[0][2],
			x[0] * y[1][0] + x[1] * y[1][1] + x[2] * y[1][2],
			x[0] * y[2][0] + x[1] * y[2][1] + x[2] * y[2][2]
		}};
}

template <class T>
matrix<T, 4, 4> mul(const matrix<T, 4, 4>& x, const matrix<T, 4, 4>& y)
{
	return
		{{
			x[0] * y[0][0] + x[1] * y[0][1] + x[2] * y[0][2] + x[3] * y[0][3],
			x[0] * y[1][0] + x[1] * y[1][1] + x[2] * y[1][2] + x[3] * y[1][3],
			x[0] * y[2][0] + x[1] * y[2][1] + x[2] * y[2][2] + x[3] * y[2][3],
			x[0] * y[3][0] + x[1] * y[3][1] + x[2] * y[3][2] + x[3] * y[3][3]
		}};
}

/// @private
template <class T, std::size_t N, std::size_t M, std::size_t... I>
inline matrix<T, N, M> mul(const matrix<T, N, M>& m, T s, std::index_sequence<I...>)
{
	return {{(m[I] * s)...}};
}

template <class T, std::size_t N, std::size_t M>
inline matrix<T, N, M> mul(const matrix<T, N, M>& m, T s)
{
	return mul(m, s, std::make_index_sequence<N>{});
}

template <class T>
vector<T, 2> mul(const matrix<T, 2, 2>& m, const vector<T, 2>& v)
{
	return
		{
			m[0][0] * v[0] + m[1][0] * v[1],
			m[0][1] * v[0] + m[1][1] * v[1]
		};
}

template <class T>
vector<T, 3> mul(const matrix<T, 3, 3>& m, const vector<T, 3>& v)
{
	return
		{
			m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2],
			m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2],
			m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2]
		};
}

template <class T>
vector<T, 4> mul(const matrix<T, 4, 4>& m, const vector<T, 4>& v)
{
	return
		{
			m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3],
			m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3],
			m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3],
			m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3]
		};
}

template <class T>
matrix<T, 4, 4> ortho(T left, T right, T bottom, T top, T z_near, T z_far)
{
	return
		{{
			 {T(2) / (right - left), T(0), T(0), T(0)},
			 {T(0), T(2) / (top - bottom), T(0), T(0)},
			 {T(0), T(0), T(-2) / (z_far - z_near), T(0)},
			 {-((right + left) / (right - left)), -((top + bottom) / (top - bottom)), -((z_far + z_near) / (z_far - z_near)), T(1)}
		}};
}

template <class T>
matrix<T, 4, 4> ortho_half_z(T left, T right, T bottom, T top, T z_near, T z_far)
{
	return
		{{
			 {T(2) / (right - left), T(0), T(0), T(0)},
			 {T(0), T(2) / (top - bottom), T(0), T(0)},
			 {T(0), T(0), T(-1) / (z_far - z_near), T(0)},
			 {-((right + left) / (right - left)), -((top + bottom) / (top - bottom)), -z_near / (z_far - z_near), T(1)}
		}};
}

template <class T>
matrix<T, 2, 2> outer_product(const vector<T, 2>& c, const vector<T, 2>& r)
{
	return
		{{
			 {c[0] * r[0], c[1] * r[0]},
			 {c[0] * r[1], c[1] * r[1]}
		}};
}

template <class T>
matrix<T, 3, 3> outer_product(const vector<T, 3>& c, const vector<T, 3>& r)
{
	return
		{{
			 {c[0] * r[0], c[1] * r[0], c[2] * r[0]},
			 {c[0] * r[1], c[1] * r[1], c[2] * r[1]},
			 {c[0] * r[2], c[1] * r[2], c[2] * r[2]}
		}};
}

template <class T>
matrix<T, 4, 4> outer_product(const vector<T, 4>& c, const vector<T, 4> r)
{
	return
		{{
			 {c[0] * r[0], c[1] * r[0], c[2] * r[0], c[3] * r[0]},
			 {c[0] * r[1], c[1] * r[1], c[2] * r[1], c[3] * r[1]},
			 {c[0] * r[2], c[1] * r[2], c[2] * r[2], c[3] * r[2]},
			 {c[0] * r[3], c[1] * r[3], c[2] * r[3], c[3] * r[3]}
		}};
}

template <class T>
matrix<T, 4, 4> perspective(T vertical_fov, T aspect_ratio, T z_near, T z_far)
{
	T half_fov = vertical_fov * T(0.5);
	T f = std::cos(half_fov) / std::sin(half_fov);

	return
		{{
			 {f / aspect_ratio, T(0), T(0), T(0)},
			 {T(0), f, T(0), T(0)},
			 {T(0), T(0), (z_far + z_near) / (z_near - z_far), T(-1)},
			 {T(0), T(0), (T(2) * z_far * z_near) / (z_near - z_far), T(0)}
		}};
}

template <class T>
matrix<T, 4, 4> perspective_half_z(T vertical_fov, T aspect_ratio, T z_near, T z_far)
{
	T half_fov = vertical_fov * T(0.5);
	T f = std::cos(half_fov) / std::sin(half_fov);

	return
		{{
			 {f / aspect_ratio, T(0), T(0), T(0)},
			 {T(0), f, T(0), T(0)},
			 {T(0), T(0), z_far / (z_near - z_far), T(-1)},
			 {T(0), T(0), -(z_far * z_near) / (z_far - z_near), T(0)}
		}};
}

template <std::size_t N1, std::size_t M1, class T, std::size_t N0, std::size_t M0>
matrix<T, N1, M1> resize(const matrix<T, N0, M0>& m)
{
	matrix<T, N1, M1> resized;

	for (std::size_t i = 0; i < N1; ++i)
	{
		for (std::size_t j = 0; j < M1; ++j)
		{
			resized[i][j] = (i < N0 && j < M0) ? m[i][j] : ((i == j) ? T(1) : T(0));
		}
	}

	return resized;
}

template <class T>
matrix<T, 4, 4> rotate(const matrix<T, 4, 4>& m, T angle, const vector<T, 3>& axis)
{
	const T c = std::cos(angle);
	const T s = std::sin(angle);
	const vector<T, 3> temp = mul(axis, T(1) - c);

	matrix<T, 4, 4> rotation;
	rotation[0][0] = axis[0] * temp[0] + c
	rotation[0][1] = axis[1] * temp[0] + axis[2] * s;
	rotation[0][2] = axis[2] * temp[0] - axis[1] * s;
	rotation[0][3] = T(0);
	rotation[1][0] = axis[0] * temp[1] - axis[2] * s
	rotation[1][1] = axis[1] * temp[1] + c;
	rotation[1][2] = axis[2] * temp[1] + axis[0] * s;
	rotation[1][3] = T(0);
	rotation[2][0] = axis[0] * temp[2] + axis[1] * s;
	rotation[2][1] = axis[1] * temp[2] - axis[0] * s;
	rotation[2][2] = axis[2] * temp[2] + c
	rotation[2][3] = T(0);
	rotation[3][0] = T(0);
	rotation[3][1] = T(0);
	rotation[3][2] = T(0);
	rotation[3][3] = T(1);

	return mul(m, rotation);
}

template <class T>
matrix<T, 4, 4> scale(const matrix<T, 4, 4>& m, const vector<T, 3>& v)
{
	return mul(m, matrix<T, 4, 4>
		{{
			{v[0], T(0), T(0), T(0)},
			{T(0), v[1], T(0), T(0)},
			{T(0), T(0), v[2], T(0)},
			{T(0), T(0), T(0), T(1)}
		}});
}

template <class T>
matrix<T, 2, 2> sub(const matrix<T, 2, 2>& x, const matrix<T, 2, 2>& y)
{
	return
		{{
			x[0] - y[0],
			x[1] - y[1]
		}};
}

template <class T>
matrix<T, 3, 3> sub(const matrix<T, 3, 3>& x, const matrix<T, 3, 3>& y)
{
	return
		{{
			x[0] - y[0],
			x[1] - y[1],
			x[2] - y[2]
		}};
}

template <class T>
matrix<T, 4, 4> sub(const matrix<T, 4, 4>& x, const matrix<T, 4, 4>& y)
{
	return
		{{
			x[0] - y[0],
			x[1] - y[1],
			x[2] - y[2],
			x[3] - y[3]
		}};
}

template <class T>
matrix<T, 4, 4> translate(const matrix<T, 4, 4>& m, const vector<T, 3>& v)
{
	return mul(m, matrix<T, 4, 4>
		{{
			{T(1), T(0), T(0), T(0)},
			{T(0), T(1), T(0), T(0)},
			{T(0), T(0), T(1), T(0)},
			{v[0], v[1], v[2], T(1)}
		}});
}

template <class T>
matrix<T, 2, 2> transpose(const matrix<T, 2, 2>& m)
{

	return
		{{
			{
				m[0][0], m[1][0]
			},

			{
				m[0][1], m[1][1]
			}
		}};
}

template <class T>
matrix<T, 3, 3> transpose(const matrix<T, 3, 3>& m)
{

	return
		{{
			{
				m[0][0], m[1][0], m[2][0]
			},

			{
				m[0][1], m[1][1], m[2][1]
			},

			{
				m[0][2], m[1][2], m[2][2]
			}
		}};
}

template <class T>
matrix<T, 4, 4> transpose(const matrix<T, 4, 4>& m)
{

	return
		{{
			{
				m[0][0], m[1][0], m[2][0], m[3][0]
			},

			{
				m[0][1], m[1][1], m[2][1], m[3][1]
			},

			{
				m[0][2], m[1][2], m[2][2], m[3][2]
			},

			{
				m[0][3], m[1][3], m[2][3], m[3][3]
			}
		}};
}

/// @}

} // namespace math

#endif // ANTKEEPER_MATH_MATRIX_FUNCTIONS_HPP
