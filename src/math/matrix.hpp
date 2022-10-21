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

#ifndef ANTKEEPER_MATH_MATRIX_HPP
#define ANTKEEPER_MATH_MATRIX_HPP

#include "math/vector.hpp"
#include <cstddef>
#include <istream>
#include <iterator>
#include <ostream>
#include <type_traits>
#include <utility>

namespace math {

/**
 * *n*-by-*m* column-major matrix.
 *
 * @tparam T Matrix element data type.
 * @tparam N Number of columns.
 * @tparam M Number of rows.
 *
 * @see https://en.wikipedia.org/wiki/Row-_and_column-major_order
 */
template <typename T, std::size_t N, std::size_t M>
struct matrix
{
	/// Matrix element data type.
	typedef T element_type;
	
	/// Number of matrix columns.
	static constexpr std::size_t column_count = N;
	
	/// Number of matrix rows.
	static constexpr std::size_t row_count = M;
	
	/// Number of matrix elements.
	static constexpr std::size_t element_count = column_count * row_count;
	
	/// Matrix column vector data type.
	typedef vector<element_type, row_count> column_vector_type;
	
	/// Matrix row vector data type.
	typedef vector<element_type, column_count> row_vector_type;
	
	/// Array of matrix column vectors.
	column_vector_type columns[column_count];
	
	/**
	 * Returns a reference to the column vector at a given index.
	 *
	 * @param i Index of a column vector.
	 *
	 * @return Reference to the column vector at index @p i.
	 */
	/// @{
	constexpr inline column_vector_type& operator[](std::size_t i) noexcept
	{
		return columns[i];
	}
	constexpr inline const column_vector_type& operator[](std::size_t i) const noexcept
	{
		return columns[i];
	}
	constexpr inline column_vector_type& column(std::size_t i) noexcept
	{
		return columns[i];
	}
	constexpr inline const column_vector_type& column(std::size_t i) const noexcept
	{
		return columns[i];
	}
	/// @}
	
	/**
	 * Returns a reference to the element at a given index, in column-major order.
	 *
	 * @param i Index of a matrix element.
	 *
	 * @return Reference to the element at index @p i.
	 */
	/// @{
	constexpr inline T& element(std::size_t i) noexcept
	{
		return columns[i / row_count][i % row_count];
	}
	constexpr inline const T& element(std::size_t i) const noexcept
	{
		return columns[i / row_count][i % row_count];
	}
	/// @}
	
	/// Returns a reference to the first column vector.
	/// @{
	constexpr inline column_vector_type& front() noexcept
	{
		return columns[0];
	}
	constexpr inline const column_vector_type& front() const noexcept
	{
		return columns[0];
	}
	/// @}
	
	/// Returns a reference to the last column vector.
	/// @{
	constexpr inline column_vector_type& back() noexcept
	{
		return elements[column_count - 1];
	}
	constexpr inline const column_vector_type& back() const noexcept
	{
		return elements[column_count - 1];
	}
	/// @}
	
	/// Returns a pointer to the column vector array.
	/// @{
	constexpr inline column_vector_type* data() noexcept
	{
		return columns;
	};
	constexpr inline const column_vector_type* data() const noexcept
	{
		return columns;
	};
	/// @}
	
	/// Returns an iterator to the first column vector.
	/// @{
	constexpr inline column_vector_type* begin() noexcept
	{
		return columns;
	}
	constexpr inline const column_vector_type* begin() const noexcept
	{
		return columns;
	}
	constexpr inline const column_vector_type* cbegin() const noexcept
	{
		return columns;
	}
	/// @}
	
	/// Returns an iterator to the column vector following the last column vector.
	/// @{
	constexpr inline column_vector_type* end() noexcept
	{
		return columns + column_count;
	}
	constexpr inline const column_vector_type* end() const noexcept
	{
		return columns + column_count;
	}
	constexpr inline const column_vector_type* cend() const noexcept
	{
		return columns + column_count;
	}
	/// @}
	
	/// Returns a reverse iterator to the first column vector of the reversed matrix.
	/// @{
	constexpr inline std::reverse_iterator<column_vector_type*> rbegin() noexcept
	{
		return std::reverse_iterator<column_vector_type*>(columns + column_count);
	}
	constexpr inline std::reverse_iterator<const column_vector_type*> rbegin() const noexcept
	{
		return std::reverse_iterator<const column_vector_type*>(columns + column_count);
	}
	constexpr inline std::reverse_iterator<const column_vector_type*> crbegin() const noexcept
	{
		return std::reverse_iterator<const column_vector_type*>(columns + column_count);
	}
	/// @}
	
	/// Returns a reverse iterator to the column vector following the last column vector of the reversed matrix.
	/// @{
	constexpr inline std::reverse_iterator<column_vector_type*> rend() noexcept
	{
		return std::reverse_iterator<column_vector_type*>(columns);
	}
	constexpr inline std::reverse_iterator<const column_vector_type*> rend() const noexcept
	{
		return std::reverse_iterator<const column_vector_type*>(columns);
	}
	constexpr inline std::reverse_iterator<const column_vector_type*> crend() const noexcept
	{
		return std::reverse_iterator<const column_vector_type*>(columns);
	}
	/// @}
	
	/// Returns the number of elements in the matrix.
	constexpr inline std::size_t size() const noexcept
	{
		return element_count;
	};
	
	/// @private
	template <class U, std::size_t... I>
	constexpr inline matrix<U, N, M> type_cast(std::index_sequence<I...>) const noexcept
	{
		return {vector<U, M>(columns[I])...};
	}
	
	/**
	 * Type-casts the elements of this matrix using `static_cast`.
	 *
	 * @tparam U Target element type.
	 *
	 * @return Matrix containing the type-casted elements.
	 */
	template <class U>
	constexpr inline explicit operator matrix<U, N, M>() const noexcept
	{
		return type_cast<U>(std::make_index_sequence<N>{});
	}
	
	/// @private
	template <std::size_t P, std::size_t O, std::size_t... I>
	constexpr inline matrix<T, P, O> size_cast(std::index_sequence<I...>) const noexcept
	{
		if constexpr (O == M)
			return {((I < N) ? columns[I] : vector<T, O>::zero()) ...};
		else
			return {((I < N) ? vector<T, O>(columns[I]) : vector<T, O>::zero()) ...};
	}
	
	/**
	 * Size-casts this matrix to a matrix with different dimensions. Casting to greater dimensions causes new elements to be set to zero.
	 *
	 * @tparam P Target number of columns.
	 * @tparam O Target number of rows.
	 *
	 * @return *p*-by-*o* matrix.
	 */
	template <std::size_t P, std::size_t O>
	constexpr inline explicit operator matrix<T, P, O>() const noexcept
	{
		return size_cast<P, O>(std::make_index_sequence<P>{});
	}
	
	/// Returns a zero matrix, where every element is equal to zero.
	static constexpr matrix zero() noexcept
	{
		return {};
	}
	
	/// @private
	template <std::size_t... I>
	static constexpr inline matrix one(std::index_sequence<I...>) noexcept
	{
		//return {column_vector_type::one() ...};
		
		// MSVC bug workaround (I must be referenced for parameter pack expansion)
		return {(I ? column_vector_type::one() : column_vector_type::one()) ...};
	}
	
	/// Returns a matrix of ones, where every element is equal to one.
	static constexpr matrix one() noexcept
	{
		return one(std::make_index_sequence<column_count>{});
	}
	
	/// @private
	template <std::size_t... I>
	static constexpr inline column_vector_type identity_column(std::size_t i, std::index_sequence<I...>) noexcept
	{
		return {(I == i ? T{1} : T{0}) ...};
	}
	
	/// @private
	template <std::size_t... I>
	static constexpr inline matrix identity(std::index_sequence<I...>) noexcept
	{
		return {identity_column(I, std::make_index_sequence<row_count>{}) ...};
	}
	
	/// Returns an identity matrix, with ones on the main diagonal and zeros elsewhere.
	static constexpr matrix identity() noexcept
	{
		return identity(std::make_index_sequence<column_count>{});
	}
};

/// 2x2 matrix.
template <typename T>
using matrix2 = matrix<T, 2, 2>;

/// 2x2 matrix.
template <typename T>
using matrix2x2 = matrix<T, 2, 2>;

/// 3x3 matrix.
template <typename T>
using matrix3 = matrix<T, 3, 3>;

/// 3x3 matrix.
template <typename T>
using matrix3x3 = matrix<T, 3, 3>;

/// 4x4 matrix.
template <typename T>
using matrix4 = matrix<T, 4, 4>;

/// 4x4 matrix.
template <typename T>
using matrix4x4 = matrix<T, 4, 4>;

/**
 * Adds two matrices.
 *
 * @param x First matrix.
 * @param y Second matrix.
 * @return Sum of the two matrices.
 */
template <class T>
constexpr matrix<T, 2, 2> add(const matrix<T, 2, 2>& x, const matrix<T, 2, 2>& y);

/// @copydoc add(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&)
template <class T>
constexpr matrix<T, 3, 3> add(const matrix<T, 3, 3>& x, const matrix<T, 3, 3>& y);

/// @copydoc add(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&)
template <class T>
constexpr matrix<T, 4, 4> add(const matrix<T, 4, 4>& x, const matrix<T, 4, 4>& y);

/**
 * Reinterprets data as an `NxM` matrix of type `T`.
 *
 * @tparam N Number of columns.
 * @tparam M Number of rows.
 * @tparam T Element type.
 * @param data Data to reinterpret.
 */
template <std::size_t N, std::size_t M, typename T>
constexpr matrix<T, N, M>& as_matrix(T& data);

/**
 * Calculates the determinant of a matrix.
 *
 * @param m Matrix of which to take the determinant. 
 */
template <class T>
constexpr T determinant(const matrix<T, 2, 2>& m);

/// @copydoc determinant(const matrix<T, 2, 2>&)
template <class T>
constexpr T determinant(const matrix<T, 3, 3>& m);

/// @copydoc determinant(const matrix<T, 2, 2>&)
template <class T>
constexpr T determinant(const matrix<T, 4, 4>& m);

/**
 * Calculates the inverse of a matrix.
 *
 * @param m Matrix of which to take the inverse.
 */
template <class T>
constexpr matrix<T, 2, 2> inverse(const matrix<T, 2, 2>& m);

/// @copydoc inverse(const matrix<T, 2, 2>&)
template <class T>
constexpr matrix<T, 3, 3> inverse(const matrix<T, 3, 3>& m);

/// @copydoc inverse(const matrix<T, 2, 2>&)
template <class T>
constexpr matrix<T, 4, 4> inverse(const matrix<T, 4, 4>& m);

/**
 * Performs a component-wise multiplication of two matrices.
 *
 * @param x First matrix multiplicand.
 * @param y Second matrix multiplicand.
 */
template <class T>
constexpr matrix<T, 2, 2> componentwise_mul(const matrix<T, 2, 2>& x, const matrix<T, 2, 2>& y);

/// @copydoc componentwise_mul(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&)
template <class T>
constexpr matrix<T, 3, 3> componentwise_mul(const matrix<T, 3, 3>& x, const matrix<T, 3, 3>& y);

/// @copydoc componentwise_mul(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&)
template <class T>
constexpr matrix<T, 4, 4> componentwise_mul(const matrix<T, 4, 4>& x, const matrix<T, 4, 4>& y);

/**
 * Creates a viewing transformation matrix.
 *
 * @param position Position of the view point.
 * @param target Position of the target.
 * @param up Normalized direction of the up vector.
 * @return Viewing transformation matrix.
 */
template <class T>
constexpr matrix<T, 4, 4> look_at(const vector<T, 3>& position, const vector<T, 3>& target, vector<T, 3> up);

/**
 * Multiplies two matrices.
 *
 * @param x First matrix.
 * @param y Second matrix.
 * @return Product of the two matrices.
 */
template <class T>
constexpr matrix<T, 2, 2> mul(const matrix<T, 2, 2>& x, const matrix<T, 2, 2>& y);

/// @copydoc mul(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&);
template <class T>
constexpr matrix<T, 3, 3> mul(const matrix<T, 3, 3>& x, const matrix<T, 3, 3>& y);

/// @copydoc mul(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&);
template <class T>
constexpr matrix<T, 4, 4> mul(const matrix<T, 4, 4>& x, const matrix<T, 4, 4>& y);

/**
 * Multiplies a matrix by a scalar.
 *
 * @param m Matrix.
 * @param s Scalar.
 * @return Product of the matrix and the scalar..
 */
template <class T, std::size_t N, std::size_t M>
constexpr matrix<T, N, M> mul(const matrix<T, N, M>& m, T s);

/**
 * Transforms a vector by a matrix.
 *
 * @param m Matrix.
 * @param v Vector.
 * @return Transformed vector.
 */
template <class T>
constexpr vector<T, 2> mul(const matrix<T, 2, 2>& m, const vector<T, 2>& v);

/// @copydoc mul(const matrix<T, 2, 2>&, const vector<T, 2>&)
template <class T>
constexpr vector<T, 3> mul(const matrix<T, 3, 3>& m, const vector<T, 3>& v);

/// @copydoc mul(const matrix<T, 2, 2>&, const vector<T, 2>&)
template <class T>
constexpr vector<T, 4> mul(const matrix<T, 4, 4>& m, const vector<T, 4>& v);

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
constexpr matrix<T, 4, 4> ortho(T left, T right, T bottom, T top, T z_near, T z_far);

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
constexpr matrix<T, 4, 4> ortho_half_z(T left, T right, T bottom, T top, T z_near, T z_far);

/**
 * Calculates the outer product of a pair of vectors.
 *
 * @param c Parameter to be treated as a column vector.
 * @param r Parameter to be treated as a row vector.
 */
template <class T>
constexpr matrix<T, 2, 2> outer_product(const vector<T, 2>& c, const vector<T, 2>& r);

/// @copydoc outer_product(const vector<T, 2>&, const vector<T, 2>&)
template <class T>
constexpr matrix<T, 3, 3> outer_product(const vector<T, 3>& c, const vector<T, 3>& r);

/// @copydoc outer_product(const vector<T, 2>&, const vector<T, 2>&)
template <class T>
constexpr matrix<T, 4, 4> outer_product(const vector<T, 4>& c, const vector<T, 4> r);

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
constexpr matrix<T, N1, M1> resize(const matrix<T, N0, M0>& m);

/**
 * Constructs a rotation matrix.
 *
 * @param angle Angle of rotation, in radians.
 * @param axis Axis of rotation
 * @return Rotation matrix.
 */
template <class T>
matrix<T, 3, 3> rotate(T angle, const vector<T, 3>& axis);

/**
 * Produces a matrix which rotates Cartesian coordinates about the x-axis by a given angle.
 *
 * @param angle Angle of rotation, in radians.
 * @return Rotation matrix.
 */
template <class T>
matrix3<T> rotate_x(T angle);

/**
 * Produces a matrix which rotates Cartesian coordinates about the y-axis by a given angle.
 *
 * @param angle Angle of rotation, in radians.
 * @return Rotation matrix.
 */
template <class T>
matrix3<T> rotate_y(T angle);

/**
 * Produces a matrix which rotates Cartesian coordinates about the z-axis by a given angle.
 *
 * @param angle Angle of rotation, in radians.
 * @return Rotation matrix.
 */
template <class T>
matrix3<T> rotate_z(T angle);

/**
 * Scales a matrix.
 *
 * @param m Matrix to scale.
 * @param v Scale vector.
 * @return Scaled matrix.
 */
template <class T>
constexpr matrix<T, 4, 4> scale(const matrix<T, 4, 4>& m, const vector<T, 3>& v);

/**
 * Subtracts a matrix from another matrix.
 *
 * @param x First matrix.
 * @param y Second matrix.
 * @return Difference between the two matrices.
 */
template <class T>
constexpr matrix<T, 2, 2> sub(const matrix<T, 2, 2>& x, const matrix<T, 2, 2>& y);

/// @copydoc sub(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&)
template <class T>
constexpr matrix<T, 3, 3> sub(const matrix<T, 3, 3>& x, const matrix<T, 3, 3>& y);

/// @copydoc sub(const matrix<T, 2, 2>&, const matrix<T, 2, 2>&)
template <class T>
constexpr matrix<T, 4, 4> sub(const matrix<T, 4, 4>& x, const matrix<T, 4, 4>& y);

/**
 * Translates a matrix.
 *
 * @param m Matrix to translate.
 * @param v Translation vector.
 * @return Translated matrix.
 */
template <class T>
constexpr matrix<T, 4, 4> translate(const matrix<T, 4, 4>& m, const vector<T, 3>& v);

/**
 * Calculates the transpose of a matrix.
 *
 * @param m Matrix of which to take the transpose.
 */
template <class T>
constexpr matrix<T, 2, 2> transpose(const matrix<T, 2, 2>& m);

/// @copydoc transpose(const matrix<T, 2, 2>&)
template <class T>
constexpr matrix<T, 3, 3> transpose(const matrix<T, 3, 3>& m);

/// @copydoc transpose(const matrix<T, 2, 2>&)
template <class T>
constexpr matrix<T, 4, 4> transpose(const matrix<T, 4, 4>& m);

/**
 * Types casts each matrix element and returns a matrix of the casted type.
 *
 * @tparam T2 Target matrix element type.
 * @tparam T1 Source matrix element type.
 * @tparam N Number of columns.
 * @tparam M Number of rows.
 * @param m Matrix to type cast.
 * @return Type-casted matrix.
 */
template <class T2, class T1, std::size_t N, std::size_t M>
constexpr matrix<T2, N, M> type_cast(const matrix<T1, N, M>& m);

template <class T>
constexpr matrix<T, 2, 2> add(const matrix<T, 2, 2>& x, const matrix<T, 2, 2>& y)
{
	return
		{{
			x[0] + y[0],
			x[1] + y[1]
		}};
}

template <class T>
constexpr matrix<T, 3, 3> add(const matrix<T, 3, 3>& x, const matrix<T, 3, 3>& y)
{
	return
		{{
			x[0] + y[0],
			x[1] + y[1],
			x[2] + y[2]
		}};
}

template <class T>
constexpr matrix<T, 4, 4> add(const matrix<T, 4, 4>& x, const matrix<T, 4, 4>& y)
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
constexpr inline matrix<T, N, M>& as_matrix(T& data)
{
	static_assert(std::is_pod<matrix<T, N, M>>::value);
	return reinterpret_cast<matrix<T, N, M>&>(data);
}

template <class T>
constexpr T determinant(const matrix<T, 2, 2>& m)
{
	return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

template <class T>
constexpr T determinant(const matrix<T, 3, 3>& m)
{
	return m[0][0] * m [1][1] * m[2][2] +
		m[0][1] * m[1][2] * m[2][0] +
		m[0][2] * m[1][0] * m[2][1] -
		m[0][0] * m[1][2] * m[2][1] -
		m[0][1] * m[1][0] * m[2][2] -
		m[0][2] * m[1][1] * m[2][0];
}

template <class T>
constexpr T determinant(const matrix<T, 4, 4>& m)
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
constexpr matrix<T, 2, 2> inverse(const matrix<T, 2, 2>& m)
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
constexpr matrix<T, 3, 3> inverse(const matrix<T, 3, 3>& m)
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
constexpr matrix<T, 4, 4> inverse(const matrix<T, 4, 4>& m)
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
constexpr matrix<T, 2, 2> componentwise_mul(const matrix<T, 2, 2>& x, const matrix<T, 2, 2>& y)
{
	return
		{{
			 {x[0][0] * y[0][0], x[0][1] * y[0][1]},
			 {x[1][0] * y[1][0], x[1][1] * y[1][1]}
		}};
}

template <class T>
constexpr matrix<T, 3, 3> componentwise_mul(const matrix<T, 3, 3>& x, const matrix<T, 3, 3>& y)
{
	return
		{{
			 {x[0][0] * y[0][0], x[0][1] * y[0][1], x[0][2] * y[0][2]},
			 {x[1][0] * y[1][0], x[1][1] * y[1][1], x[1][2] * y[1][2]},
			 {x[2][0] * y[2][0], x[2][1] * y[2][1], x[2][2] * y[2][2]}
		}};
}

template <class T>
constexpr matrix<T, 4, 4> componentwise_mul(const matrix<T, 4, 4>& x, const matrix<T, 4, 4>& y)
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
constexpr matrix<T, 4, 4> look_at(const vector<T, 3>& position, const vector<T, 3>& target, vector<T, 3> up)
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
constexpr matrix<T, 2, 2> mul(const matrix<T, 2, 2>& x, const matrix<T, 2, 2>& y)
{
	return
		{{
			x[0] * y[0][0] + x[1] * y[0][1],
			x[0] * y[1][0] + x[1] * y[1][1]
		}};
}

template <class T>
constexpr matrix<T, 3, 3> mul(const matrix<T, 3, 3>& x, const matrix<T, 3, 3>& y)
{
	return
		{{
			x[0] * y[0][0] + x[1] * y[0][1] + x[2] * y[0][2],
			x[0] * y[1][0] + x[1] * y[1][1] + x[2] * y[1][2],
			x[0] * y[2][0] + x[1] * y[2][1] + x[2] * y[2][2]
		}};
}

template <class T>
constexpr matrix<T, 4, 4> mul(const matrix<T, 4, 4>& x, const matrix<T, 4, 4>& y)
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
constexpr inline matrix<T, N, M> mul(const matrix<T, N, M>& m, T s, std::index_sequence<I...>)
{
	return {{(m[I] * s)...}};
}

template <class T, std::size_t N, std::size_t M>
constexpr inline matrix<T, N, M> mul(const matrix<T, N, M>& m, T s)
{
	return mul(m, s, std::make_index_sequence<N>{});
}

template <class T>
constexpr vector<T, 2> mul(const matrix<T, 2, 2>& m, const vector<T, 2>& v)
{
	return
		{
			m[0][0] * v[0] + m[1][0] * v[1],
			m[0][1] * v[0] + m[1][1] * v[1]
		};
}

template <class T>
constexpr vector<T, 3> mul(const matrix<T, 3, 3>& m, const vector<T, 3>& v)
{
	return
		{
			m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2],
			m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2],
			m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2]
		};
}

template <class T>
constexpr vector<T, 4> mul(const matrix<T, 4, 4>& m, const vector<T, 4>& v)
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
constexpr matrix<T, 4, 4> ortho(T left, T right, T bottom, T top, T z_near, T z_far)
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
constexpr matrix<T, 4, 4> ortho_half_z(T left, T right, T bottom, T top, T z_near, T z_far)
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
constexpr matrix<T, 2, 2> outer_product(const vector<T, 2>& c, const vector<T, 2>& r)
{
	return
		{{
			 {c[0] * r[0], c[1] * r[0]},
			 {c[0] * r[1], c[1] * r[1]}
		}};
}

template <class T>
constexpr matrix<T, 3, 3> outer_product(const vector<T, 3>& c, const vector<T, 3>& r)
{
	return
		{{
			 {c[0] * r[0], c[1] * r[0], c[2] * r[0]},
			 {c[0] * r[1], c[1] * r[1], c[2] * r[1]},
			 {c[0] * r[2], c[1] * r[2], c[2] * r[2]}
		}};
}

template <class T>
constexpr matrix<T, 4, 4> outer_product(const vector<T, 4>& c, const vector<T, 4> r)
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
constexpr matrix<T, N1, M1> resize(const matrix<T, N0, M0>& m)
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
matrix<T, 3, 3> rotate(T angle, const vector<T, 3>& axis)
{
	const T c = std::cos(angle);
	const T s = std::sin(angle);
	const vector<T, 3> temp = mul(axis, T(1) - c);

	matrix<T, 3, 3> rotation;
	rotation[0][0] = axis[0] * temp[0] + c;
	rotation[0][1] = axis[1] * temp[0] + axis[2] * s;
	rotation[0][2] = axis[2] * temp[0] - axis[1] * s;
	rotation[1][0] = axis[0] * temp[1] - axis[2] * s;
	rotation[1][1] = axis[1] * temp[1] + c;
	rotation[1][2] = axis[2] * temp[1] + axis[0] * s;
	rotation[2][0] = axis[0] * temp[2] + axis[1] * s;
	rotation[2][1] = axis[1] * temp[2] - axis[0] * s;
	rotation[2][2] = axis[2] * temp[2] + c;

	return rotation;
}

template <class T>
matrix3<T> rotate_x(T angle)
{
	const T c = std::cos(angle);
	const T s = std::sin(angle);
	
	return matrix3<T>
	{
		T(1), T(0), T(0),
		T(0), c, s,
		T(0), -s, c
	};
}

template <class T>
matrix3<T> rotate_y(T angle)
{
	const T c = std::cos(angle);
	const T s = std::sin(angle);
	
	return matrix3<T>
	{
		c, T(0), -s,
		T(0), T(1), T(0),
		s, T(0), c
	};
}

template <class T>
matrix3<T> rotate_z(T angle)
{
	const T c = std::cos(angle);
	const T s = std::sin(angle);
	
	return matrix3<T>
	{
		c, s, T(0),
		-s, c, T(0),
		T(0), T(0), T(1)
	};
}

template <class T>
constexpr matrix<T, 4, 4> scale(const matrix<T, 4, 4>& m, const vector<T, 3>& v)
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
constexpr matrix<T, 2, 2> sub(const matrix<T, 2, 2>& x, const matrix<T, 2, 2>& y)
{
	return
		{{
			x[0] - y[0],
			x[1] - y[1]
		}};
}

template <class T>
constexpr matrix<T, 3, 3> sub(const matrix<T, 3, 3>& x, const matrix<T, 3, 3>& y)
{
	return
		{{
			x[0] - y[0],
			x[1] - y[1],
			x[2] - y[2]
		}};
}

template <class T>
constexpr matrix<T, 4, 4> sub(const matrix<T, 4, 4>& x, const matrix<T, 4, 4>& y)
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
constexpr matrix<T, 4, 4> translate(const matrix<T, 4, 4>& m, const vector<T, 3>& v)
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
constexpr matrix<T, 2, 2> transpose(const matrix<T, 2, 2>& m)
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
constexpr matrix<T, 3, 3> transpose(const matrix<T, 3, 3>& m)
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
constexpr matrix<T, 4, 4> transpose(const matrix<T, 4, 4>& m)
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

/// @private
template <class T2, class T1, std::size_t N, std::size_t M, std::size_t... I>
constexpr inline matrix<T2, N, M> type_cast(const matrix<T1, N, M>& m, std::index_sequence<I...>)
{
	return {type_cast<T2>(m[I])...};
}

template <class T2, class T1, std::size_t N, std::size_t M>
constexpr matrix<T2, N, M> type_cast(const matrix<T1, N, M>& m)
{
	return type_cast<T2>(m, std::make_index_sequence<N>{}); 
}

} // namespace math

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

/**
 * Writes the elements of a matrix to an output stream, with each element delimeted by a space.
 *
 * @param os Output stream.
 * @param m Matrix.
 * @return Output stream.
 */
template <class T, std::size_t N, std::size_t M>
std::ostream& operator<<(std::ostream& os, const math::matrix<T, N, M>& m);

/**
 * Reads the elements of a matrix from an input stream, with each element delimeted by a space.
 *
 * @param is Input stream.
 * @param m Matrix.
 * @return Input stream.
 */
template <class T, std::size_t N, std::size_t M>
std::istream& operator>>(std::istream& is, math::matrix<T, N, M>& m);

template <class T, std::size_t N, std::size_t M>
constexpr inline math::matrix<T, N, M> operator+(const math::matrix<T, N, M>& x, const math::matrix<T, N, M>& y)
{
	return math::add(x, y);
}

template <class T, std::size_t N, std::size_t M>
constexpr inline math::matrix<T, N, M> operator*(const math::matrix<T, N, M>& x, const math::matrix<T, N, M>& y)
{
	return math::mul(x, y);
}

template <class T, std::size_t N, std::size_t M>
constexpr inline math::matrix<T, N, M> operator*(const math::matrix<T, N, M>& m, T s)
{
	return math::mul(m, s);
}

template <class T, std::size_t N, std::size_t M>
constexpr inline math::matrix<T, N, M> operator*(T s, const math::matrix<T, N, M>& m)
{
	return math::mul(m, s);
}

template <class T, std::size_t N>
constexpr inline math::vector<T, N> operator*(const math::matrix<T, N, N>& m, const math::vector<T, N>& v)
{
	return math::mul(m, v);
}

template <class T, std::size_t N, std::size_t M>
constexpr inline math::matrix<T, N, M> operator-(const math::matrix<T, N, M>& x, const math::matrix<T, N, M>& y)
{
	return math::sub(x, y);
}

template <class T, std::size_t N, std::size_t M>
std::ostream& operator<<(std::ostream& os, const math::matrix<T, N, M>& m)
{
	for (std::size_t i = 0; i < m.size(); ++i)
	{
		if (i)
			os << ' ';
		os << m.element(i);
	}
	
	return os;
}

template <class T, std::size_t N, std::size_t M>
std::istream& operator>>(std::istream& is, math::matrix<T, N, M>& m)
{
	for (std::size_t i = 0; i < m.size(); ++i)
		is >> m.element(i);
	
	return is;
}

#endif // ANTKEEPER_MATH_MATRIX_HPP
