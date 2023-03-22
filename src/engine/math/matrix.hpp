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

#ifndef ANTKEEPER_MATH_MATRIX_HPP
#define ANTKEEPER_MATH_MATRIX_HPP

#include <engine/math/vector.hpp>
#include <cstddef>
#include <istream>
#include <iterator>
#include <ostream>
#include <type_traits>
#include <utility>

namespace math {

/**
 * *n* by *m* column-major matrix.
 *
 * @tparam T Element type.
 * @tparam N Number of columns.
 * @tparam M Number of rows.
 *
 * @see https://en.wikipedia.org/wiki/Row-_and_column-major_order
 */
template <typename T, std::size_t N, std::size_t M>
struct matrix
{
	/// Element type.
	typedef T element_type;
	
	/// Number of columns.
	static constexpr std::size_t column_count = N;
	
	/// Number of rows.
	static constexpr std::size_t row_count = M;
	
	/// Number of elements.
	static constexpr std::size_t element_count = column_count * row_count;
	
	/// Matrix column vector data type.
	typedef vector<element_type, row_count> column_vector_type;
	
	/// Matrix row vector data type.
	typedef vector<element_type, column_count> row_vector_type;
	
	/// Array of matrix column vectors.
	column_vector_type columns[column_count];
	
	/// @name Conversion
	/// @{
	
	/// @private
	template <class U, std::size_t... I>
	[[nodiscard]] inline constexpr matrix<U, N, M> type_cast(std::index_sequence<I...>) const noexcept
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
	[[nodiscard]] inline constexpr explicit operator matrix<U, N, M>() const noexcept
	{
		return type_cast<U>(std::make_index_sequence<N>{});
	}
	
	/// @private
	template <std::size_t P, std::size_t O, std::size_t... I>
	[[nodiscard]] inline constexpr matrix<T, P, O> size_cast(std::index_sequence<I...>) const noexcept
	{
		if constexpr (O == M)
		{
			return {((I < N) ? columns[I] : matrix<T, P, O>::identity()[I]) ...};
		}
		else
		{
			return {((I < N) ? vector<T, O>(columns[I]) : matrix<T, P, O>::identity()[I]) ...};
		}
	}
	
	/**
	 * Size-casts this matrix to a matrix with different dimensions. Casting to greater dimensions causes new elements to be set to identity matrix elements.
	 *
	 * @tparam P Target number of columns.
	 * @tparam O Target number of rows.
	 *
	 * @return *p* by *o* matrix.
	 */
	template <std::size_t P, std::size_t O>
	[[nodiscard]] inline constexpr explicit operator matrix<T, P, O>() const noexcept
	{
		return size_cast<P, O>(std::make_index_sequence<P>{});
	}
	
	/// @}
	
	/// @name Column access
	/// @{
	
	/**
	 * Returns a reference to the column vector at a given index.
	 *
	 * @param i Index of a column vector.
	 *
	 * @return Reference to the column vector at index @p i.
	 */
	/// @{
	[[nodiscard]] inline constexpr column_vector_type& operator[](std::size_t i) noexcept
	{
		return columns[i];
	}
	[[nodiscard]] inline constexpr const column_vector_type& operator[](std::size_t i) const noexcept
	{
		return columns[i];
	}
	[[nodiscard]] inline constexpr column_vector_type& column(std::size_t i) noexcept
	{
		return columns[i];
	}
	[[nodiscard]] inline constexpr const column_vector_type& column(std::size_t i) const noexcept
	{
		return columns[i];
	}
	/// @}
	
	/**
	 * Returns a reference to the first column vector.
	 */
	/// @{
	[[nodiscard]] inline constexpr column_vector_type& front() noexcept
	{
		return columns[0];
	}
	[[nodiscard]] inline constexpr const column_vector_type& front() const noexcept
	{
		return columns[0];
	}
	/// @}
	
	/**
	 * Returns a reference to the last column vector.
	 */
	/// @{
	[[nodiscard]] inline constexpr column_vector_type& back() noexcept
	{
		return columns[column_count - 1];
	}
	[[nodiscard]] inline constexpr const column_vector_type& back() const noexcept
	{
		return columns[column_count - 1];
	}
	/// @}
	
	/// @}
	
	/// @name Element access
	/// @{
	
	/**
	 * Returns a reference to the element at a given column-major index.
	 *
	 * @param i Column-major index of a matrix element.
	 *
	 * @return Reference to the element at column-major index @p i.
	 */
	/// @{
	[[nodiscard]] inline constexpr T& element(std::size_t i) noexcept
	{
		return columns[i / row_count][i % row_count];
	}
	[[nodiscard]] inline constexpr const T& element(std::size_t i) const noexcept
	{
		return columns[i / row_count][i % row_count];
	}
	/// @}
	
	/**
	 * Returns a pointer to the first element.
	 *
	 * @warning If matrix::element_type is not a POD type, elements may not be stored contiguously.
	 */
	/// @{
	[[nodiscard]] inline constexpr element_type* data() noexcept
	{
		return &columns[0][0];
	};
	[[nodiscard]] inline constexpr const element_type* data() const noexcept
	{
		return &columns[0][0];
	};
	/// @}
	
	/// @}
	
	/// @name Iterators
	/// @{
	
	/**
	 * Returns an iterator to the first column vector.
	 */
	/// @{
	[[nodiscard]] inline constexpr column_vector_type* begin() noexcept
	{
		return columns;
	}
	[[nodiscard]] inline constexpr const column_vector_type* begin() const noexcept
	{
		return columns;
	}
	[[nodiscard]] inline constexpr const column_vector_type* cbegin() const noexcept
	{
		return columns;
	}
	/// @}
	
	/**
	 * Returns an iterator to the column vector following the last column vector.
	 */
	/// @{
	[[nodiscard]] inline constexpr column_vector_type* end() noexcept
	{
		return columns + column_count;
	}
	[[nodiscard]] inline constexpr const column_vector_type* end() const noexcept
	{
		return columns + column_count;
	}
	[[nodiscard]] inline constexpr const column_vector_type* cend() const noexcept
	{
		return columns + column_count;
	}
	/// @}
	
	/**
	 * Returns a reverse iterator to the first column vector of the reversed matrix.
	 */
	/// @{
	[[nodiscard]] inline constexpr std::reverse_iterator<column_vector_type*> rbegin() noexcept
	{
		return std::reverse_iterator<column_vector_type*>(columns + column_count);
	}
	[[nodiscard]] inline constexpr std::reverse_iterator<const column_vector_type*> rbegin() const noexcept
	{
		return std::reverse_iterator<const column_vector_type*>(columns + column_count);
	}
	[[nodiscard]] inline constexpr std::reverse_iterator<const column_vector_type*> crbegin() const noexcept
	{
		return std::reverse_iterator<const column_vector_type*>(columns + column_count);
	}
	/// @}
	
	/**
	 * Returns a reverse iterator to the column vector following the last column vector of the reversed matrix.
	 */
	/// @{
	[[nodiscard]] inline constexpr std::reverse_iterator<column_vector_type*> rend() noexcept
	{
		return std::reverse_iterator<column_vector_type*>(columns);
	}
	[[nodiscard]] inline constexpr std::reverse_iterator<const column_vector_type*> rend() const noexcept
	{
		return std::reverse_iterator<const column_vector_type*>(columns);
	}
	[[nodiscard]] inline constexpr std::reverse_iterator<const column_vector_type*> crend() const noexcept
	{
		return std::reverse_iterator<const column_vector_type*>(columns);
	}
	/// @}
	
	/// @}
	
	/// @name Capacity
	/// @{
	
	/**
	 * Returns the number of elements in the matrix.
	 */
	[[nodiscard]] inline constexpr std::size_t size() const noexcept
	{
		return element_count;
	};
	
	/// @}
	
	/// @name Constants
	/// @{
	
	/**
	 * Returns a zero matrix, where every element is equal to zero.
	 */
	[[nodiscard]] static constexpr matrix zero() noexcept
	{
		return {};
	}
	
	/// @private
	template <std::size_t... I>
	[[nodiscard]] static inline constexpr matrix one(std::index_sequence<I...>) noexcept
	{
		//return {column_vector_type::one() ...};
		
		// MSVC bug workaround (I must be referenced for parameter pack expansion)
		return {(I ? column_vector_type::one() : column_vector_type::one()) ...};
	}
	
	/**
	 * Returns a matrix of ones, where every element is equal to one.
	 */
	[[nodiscard]] static constexpr matrix one() noexcept
	{
		return one(std::make_index_sequence<column_count>{});
	}
	
	/// @private
	template <std::size_t... I>
	[[nodiscard]] static inline constexpr column_vector_type identity_column(std::size_t i, std::index_sequence<I...>) noexcept
	{
		return {(I == i ? T{1} : T{0}) ...};
	}
	
	/// @private
	template <std::size_t... I>
	[[nodiscard]] static inline constexpr matrix identity(std::index_sequence<I...>) noexcept
	{
		return {identity_column(I, std::make_index_sequence<row_count>{}) ...};
	}
	
	/**
	 * Returns an identity matrix, with ones on the main diagonal and zeros elsewhere.
	 */
	[[nodiscard]] static constexpr matrix identity() noexcept
	{
		return identity(std::make_index_sequence<column_count>{});
	}
	
	/// @}
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
 * @param a First matrix.
 * @param b Second matrix.
 *
 * @return Sum of the two matrices.
 */
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> add(const matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept;

/**
 * Adds a matrix and a scalar.
 *
 * @param a Matrix.
 * @param b scalar.
 *
 * @return Sum of the matrix and scalar.
 */
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> add(const matrix<T, N, M>& a, T b) noexcept;

/**
 * Calculates the determinant of a square matrix.
 *
 * @param m Matrix of which to take the determinant.
 *
 * @return Determinant of @p m.
 *
 * @warning Currently only implemented for 2x2, 3x3, and 4x4 matrices.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr T determinant(const matrix<T, N, N>& m) noexcept;

/**
 * Performs a component-wise multiplication of two matrices.
 *
 * @param x First matrix multiplicand.
 * @param y Second matrix multiplicand.
 *
 * @return Product of the component-wise multiplcation.
 */
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> componentwise_mul(const matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept;

/**
 * Divides a matrix by a matrix.
 *
 * @param a First matrix.
 * @param b Second matrix.
 *
 * @return Result of the division.
 */
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> div(const matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept;

/**
 * Divides a matrix by a scalar.
 *
 * @param a Matrix.
 * @param b Scalar.
 *
 * @return Result of the division.
 */
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> div(const matrix<T, N, M>& a, T b) noexcept;

/**
 * Divides a scalar by a matrix.
 *
 * @param a Scalar.
 * @param b Matrix.
 *
 * @return Result of the division.
 */
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> div(T a, const matrix<T, N, M>& b) noexcept;

/**
 * Extracts the Ith column from a matrix.
 *
 * @tparam I Index of a column.
 * @tparam T Element type.
 * @tparam N Number of columns.
 * @tparam M Number of rows.
 *
 * @param m Matrix from which to extract a column.
 *
 * @return Reference to the Ith column of @p m.
 */
/// @{
template<std::size_t I, class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr typename matrix<T, N, M>::column_vector_type& get(math::matrix<T, N, M>& m) noexcept;
template<std::size_t I, class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr typename matrix<T, N, M>::column_vector_type&& get(math::matrix<T, N, M>&& m) noexcept;
template<std::size_t I, class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr const typename matrix<T, N, M>::column_vector_type& get(const math::matrix<T, N, M>& m) noexcept;
template<std::size_t I, class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr const typename matrix<T, N, M>::column_vector_type&& get(const math::matrix<T, N, M>&& m) noexcept;
/// @}

/**
 * Calculates the inverse of a square matrix.
 *
 * @param m Square matrix.
 *
 * @return Inverse of matrix @p m.
 *
 * @warning Currently only implemented for 2x2, 3x3, and 4x4 matrices.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr matrix<T, N, N> inverse(const matrix<T, N, N>& m) noexcept;

/**
 * Creates a viewing transformation matrix.
 *
 * @param position Position of the view point.
 * @param target Position of the target.
 * @param up Normalized direction of the up vector.
 *
 * @return Viewing transformation matrix.
 */
template <class T>
[[nodiscard]] constexpr matrix<T, 4, 4> look_at(const vector<T, 3>& position, const vector<T, 3>& target, vector<T, 3> up);

/**
 * Multiplies two matrices
 *
 * @tparam T Matrix element type.
 * @tparam N Number of columns in matrix @p a, and rows in matrix @p b.
 * @tparam M Number of rows in matrix @p a.
 * @tparam P Number of columns in matrix @p b.
 *
 * @param a First matrix.
 * @param b Second matrix.
 *
 * @return Product of `a * b`.
 */
template <typename T, std::size_t N, std::size_t M, std::size_t P>
[[nodiscard]] constexpr matrix<T, P, M> mul(const matrix<T, N, M>& a, const matrix<T, P, N>& b) noexcept;

/**
 * Multiplies a matrix by a scalar.
 *
 * @param a Matrix.
 * @param b Scalar.
 *
 * @return Product of the matrix and the scalar.
 */
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> mul(const matrix<T, N, M>& a, T b) noexcept;

/**
 * Calculates the product of a matrix and a row vector.
 *
 * @param a Matrix.
 * @param b Row vector
 *
 * @return Product of the matrix and the row vector.
 */
template <typename T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr typename matrix<T, N, M>::column_vector_type mul(const matrix<T, N, M>& a, const typename matrix<T, N, M>::row_vector_type& b) noexcept;

/**
 * Calculates the product of a column vector and a matrix.
 *
 * @param a Column vector.
 * @param b Matrix.
 *
 * @return Product of the column vector and the matrix.
 */
template <typename T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr typename matrix<T, N, M>::row_vector_type mul(const typename matrix<T, N, M>::column_vector_type& a, const matrix<T, N, M>& b) noexcept;

/**
 * Constructs a rotation matrix.
 *
 * @param angle Angle of rotation, in radians.
 * @param axis Axis of rotation.
 *
 * @return Rotation matrix.
 */
template <class T>
[[nodiscard]] matrix<T, 3, 3> rotate(T angle, const vector<T, 3>& axis);

/**
 * Produces a matrix which rotates Cartesian coordinates about the x-axis by a given angle.
 *
 * @param angle Angle of rotation, in radians.
 *
 * @return Rotation matrix.
 */
template <class T>
[[nodiscard]] matrix3<T> rotate_x(T angle);

/**
 * Produces a matrix which rotates Cartesian coordinates about the y-axis by a given angle.
 *
 * @param angle Angle of rotation, in radians.
 *
 * @return Rotation matrix.
 */
template <class T>
[[nodiscard]] matrix3<T> rotate_y(T angle);

/**
 * Produces a matrix which rotates Cartesian coordinates about the z-axis by a given angle.
 *
 * @param angle Angle of rotation, in radians.
 *
 * @return Rotation matrix.
 */
template <class T>
[[nodiscard]] matrix3<T> rotate_z(T angle);

/**
 * Scales a matrix.
 *
 * @param m Matrix to scale.
 * @param v Scale vector.
 *
 * @return Scaled matrix.
 */
template <class T>
[[nodiscard]] constexpr matrix<T, 4, 4> scale(const matrix<T, 4, 4>& m, const vector<T, 3>& v);

/**
 * Subtracts a matrix from another matrix.
 *
 * @param a First matrix.
 * @param b Second matrix.
 *
 * @return Difference between the two matrices.
 */
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> sub(const matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept;

/**
 * Subtracts a scalar from matrix.
 *
 * @param a Matrix.
 * @param b Scalar.
 *
 * @return Difference between the matrix and scalar.
 */
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> sub(const matrix<T, N, M>& a, T b) noexcept;

/**
 * Subtracts a matrix from a scalar.
 *
 * @param a Scalar.
 * @param b Matrix.
 *
 * @return Difference between the scalar and matrix.
 */
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> sub(T a, const matrix<T, N, M>& b) noexcept;

/**
 * Calculates the trace of a square matrix.
 *
 * @param m Square matrix.
 *
 * @return Sum of elements on the main diagonal.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr T trace(const matrix<T, N, N>& m) noexcept;

/**
 * Translates a matrix.
 *
 * @param m Matrix to translate.
 * @param v Translation vector.
 *
 * @return Translated matrix.
 */
template <class T>
[[nodiscard]] constexpr matrix<T, 4, 4> translate(const matrix<T, 4, 4>& m, const vector<T, 3>& v);

/**
 * Calculates the transpose of a matrix.
 *
 * @param m Matrix to transpose.
 *
 * @return Transposed matrix.
 */
template <typename T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, M, N> transpose(const matrix<T, N, M>& m) noexcept;

/// @private
template <class T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr matrix<T, N, M> add(const matrix<T, N, M>& a, const matrix<T, N, M>& b, std::index_sequence<I...>) noexcept
{
	return {(a[I] + b[I]) ...};
}

template <class T, std::size_t N, std::size_t M>
constexpr matrix<T, N, M> add(const matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept
{
	return add(a, b, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr matrix<T, N, M> add(const matrix<T, N, M>& a, T b, std::index_sequence<I...>) noexcept
{
	return {(a[I] + b) ...};
}

template <class T, std::size_t N, std::size_t M>
constexpr matrix<T, N, M> add(const matrix<T, N, M>& a, T b) noexcept
{
	return add(a, b, std::make_index_sequence<N>{});
}

/// @private
template <class T>
constexpr T determinant(const matrix<T, 2, 2>& m) noexcept
{
	return
		m[0][0] * m[1][1] -
		m[0][1] * m[1][0];
}

/// @private
template <class T>
constexpr T determinant(const matrix<T, 3, 3>& m) noexcept
{
	return
		m[0][0] * m[1][1] * m[2][2] +
		m[0][1] * m[1][2] * m[2][0] +
		m[0][2] * m[1][0] * m[2][1] -
		m[0][0] * m[1][2] * m[2][1] -
		m[0][1] * m[1][0] * m[2][2] -
		m[0][2] * m[1][1] * m[2][0];
}

/// @private
template <class T>
constexpr T determinant(const matrix<T, 4, 4>& m) noexcept
{
	return
		m[0][3] * m[1][2] * m[2][1] * m[3][0] - m[0][2] * m[1][3] * m[2][1] * m[3][0] -
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

/// @private
template <class T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr matrix<T, N, M> componentwise_mul(const matrix<T, N, M>& a, const matrix<T, N, M>& b, std::index_sequence<I...>) noexcept
{
	return {(a[I] * b[I]) ...};
}

template <class T, std::size_t N, std::size_t M>
constexpr matrix<T, N, M> componentwise_mul(const matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept
{
	return componentwise_mul(a, b, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr matrix<T, N, M> div(const matrix<T, N, M>& a, const matrix<T, N, M>& b, std::index_sequence<I...>) noexcept
{
	return {(a[I] / b[I]) ...};
}

template <class T, std::size_t N, std::size_t M>
constexpr matrix<T, N, M> div(const matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept
{
	return div(a, b, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr matrix<T, N, M> div(const matrix<T, N, M>& a, T b, std::index_sequence<I...>) noexcept
{
	return {(a[I] / b) ...};
}

template <class T, std::size_t N, std::size_t M>
constexpr matrix<T, N, M> div(const matrix<T, N, M>& a, T b) noexcept
{
	return div(a, b, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr matrix<T, N, M> div(T a, const matrix<T, N, M>& b, std::index_sequence<I...>) noexcept
{
	return {(a / b[I]) ...};
}

template <class T, std::size_t N, std::size_t M>
constexpr matrix<T, N, M> div(T a, const matrix<T, N, M>& b) noexcept
{
	return div(a, b, std::make_index_sequence<N>{});
}

template<std::size_t I, class T, std::size_t N, std::size_t M>
inline constexpr typename matrix<T, N, M>::column_vector_type& get(math::matrix<T, N, M>& m) noexcept
{
	static_assert(I < N);
	return m.columns[I];
}

template<std::size_t I, class T, std::size_t N, std::size_t M>
inline constexpr typename matrix<T, N, M>::column_vector_type&& get(math::matrix<T, N, M>&& m) noexcept
{
	static_assert(I < N);
	return std::move(m.columns[I]);
}

template<std::size_t I, class T, std::size_t N, std::size_t M>
inline constexpr const typename matrix<T, N, M>::column_vector_type& get(const math::matrix<T, N, M>& m) noexcept
{
	static_assert(I < N);
	return m.columns[I];
}

template<std::size_t I, class T, std::size_t N, std::size_t M>
inline constexpr const typename matrix<T, N, M>::column_vector_type&& get(const math::matrix<T, N, M>&& m) noexcept
{
	static_assert(I < N);
	return std::move(m.columns[I]);
}

/// @private
template <class T>
constexpr matrix<T, 2, 2> inverse(const matrix<T, 2, 2>& m) noexcept
{
	const T inv_det = T{1} / determinant(m);
	
	return
	{
		 m[1][1] * inv_det,
		-m[0][1] * inv_det,
		-m[1][0] * inv_det,
		 m[0][0] * inv_det
	};
}

/// @private
template <class T>
constexpr matrix<T, 3, 3> inverse(const matrix<T, 3, 3>& m) noexcept
{
	const T inv_det = T{1} / determinant(m);

	return
	{
		(m[1][1] * m[2][2] - m[1][2] * m[2][1]) * inv_det,
		(m[0][2] * m[2][1] - m[0][1] * m[2][2]) * inv_det,
		(m[0][1] * m[1][2] - m[0][2] * m[1][1]) * inv_det,
		
		(m[1][2] * m[2][0] - m[1][0] * m[2][2]) * inv_det,
		(m[0][0] * m[2][2] - m[0][2] * m[2][0]) * inv_det,
		(m[0][2] * m[1][0] - m[0][0] * m[1][2]) * inv_det,

		(m[1][0] * m[2][1] - m[1][1] * m[2][0]) * inv_det,
		(m[0][1] * m[2][0] - m[0][0] * m[2][1]) * inv_det,
		(m[0][0] * m[1][1] - m[0][1] * m[1][0]) * inv_det
	};
}

/// @private
template <class T>
constexpr matrix<T, 4, 4> inverse(const matrix<T, 4, 4>& m) noexcept
{
	const T inv_det = T{1} / determinant(m);
	
	return
	{
		(m[1][2] * m[2][3] * m[3][1] - m[1][3] * m[2][2] * m[3][1] + m[1][3] * m[2][1] * m[3][2] - m[1][1] * m[2][3] * m[3][2] - m[1][2] * m[2][1] * m[3][3] + m[1][1] * m[2][2] * m[3][3]) * inv_det,
		(m[0][3] * m[2][2] * m[3][1] - m[0][2] * m[2][3] * m[3][1] - m[0][3] * m[2][1] * m[3][2] + m[0][1] * m[2][3] * m[3][2] + m[0][2] * m[2][1] * m[3][3] - m[0][1] * m[2][2] * m[3][3]) * inv_det,
		(m[0][2] * m[1][3] * m[3][1] - m[0][3] * m[1][2] * m[3][1] + m[0][3] * m[1][1] * m[3][2] - m[0][1] * m[1][3] * m[3][2] - m[0][2] * m[1][1] * m[3][3] + m[0][1] * m[1][2] * m[3][3]) * inv_det,
		(m[0][3] * m[1][2] * m[2][1] - m[0][2] * m[1][3] * m[2][1] - m[0][3] * m[1][1] * m[2][2] + m[0][1] * m[1][3] * m[2][2] + m[0][2] * m[1][1] * m[2][3] - m[0][1] * m[1][2] * m[2][3]) * inv_det,

		(m[1][3] * m[2][2] * m[3][0] - m[1][2] * m[2][3] * m[3][0] - m[1][3] * m[2][0] * m[3][2] + m[1][0] * m[2][3] * m[3][2] + m[1][2] * m[2][0] * m[3][3] - m[1][0] * m[2][2] * m[3][3]) * inv_det,
		(m[0][2] * m[2][3] * m[3][0] - m[0][3] * m[2][2] * m[3][0] + m[0][3] * m[2][0] * m[3][2] - m[0][0] * m[2][3] * m[3][2] - m[0][2] * m[2][0] * m[3][3] + m[0][0] * m[2][2] * m[3][3]) * inv_det,
		(m[0][3] * m[1][2] * m[3][0] - m[0][2] * m[1][3] * m[3][0] - m[0][3] * m[1][0] * m[3][2] + m[0][0] * m[1][3] * m[3][2] + m[0][2] * m[1][0] * m[3][3] - m[0][0] * m[1][2] * m[3][3]) * inv_det,
		(m[0][2] * m[1][3] * m[2][0] - m[0][3] * m[1][2] * m[2][0] + m[0][3] * m[1][0] * m[2][2] - m[0][0] * m[1][3] * m[2][2] - m[0][2] * m[1][0] * m[2][3] + m[0][0] * m[1][2] * m[2][3]) * inv_det,

		(m[1][1] * m[2][3] * m[3][0] - m[1][3] * m[2][1] * m[3][0] + m[1][3] * m[2][0] * m[3][1] - m[1][0] * m[2][3] * m[3][1] - m[1][1] * m[2][0] * m[3][3] + m[1][0] * m[2][1] * m[3][3]) * inv_det,
		(m[0][3] * m[2][1] * m[3][0] - m[0][1] * m[2][3] * m[3][0] - m[0][3] * m[2][0] * m[3][1] + m[0][0] * m[2][3] * m[3][1] + m[0][1] * m[2][0] * m[3][3] - m[0][0] * m[2][1] * m[3][3]) * inv_det,
		(m[0][1] * m[1][3] * m[3][0] - m[0][3] * m[1][1] * m[3][0] + m[0][3] * m[1][0] * m[3][1] - m[0][0] * m[1][3] * m[3][1] - m[0][1] * m[1][0] * m[3][3] + m[0][0] * m[1][1] * m[3][3]) * inv_det,
		(m[0][3] * m[1][1] * m[2][0] - m[0][1] * m[1][3] * m[2][0] - m[0][3] * m[1][0] * m[2][1] + m[0][0] * m[1][3] * m[2][1] + m[0][1] * m[1][0] * m[2][3] - m[0][0] * m[1][1] * m[2][3]) * inv_det,

		(m[1][2] * m[2][1] * m[3][0] - m[1][1] * m[2][2] * m[3][0] - m[1][2] * m[2][0] * m[3][1] + m[1][0] * m[2][2] * m[3][1] + m[1][1] * m[2][0] * m[3][2] - m[1][0] * m[2][1] * m[3][2]) * inv_det,
		(m[0][1] * m[2][2] * m[3][0] - m[0][2] * m[2][1] * m[3][0] + m[0][2] * m[2][0] * m[3][1] - m[0][0] * m[2][2] * m[3][1] - m[0][1] * m[2][0] * m[3][2] + m[0][0] * m[2][1] * m[3][2]) * inv_det,
		(m[0][2] * m[1][1] * m[3][0] - m[0][1] * m[1][2] * m[3][0] - m[0][2] * m[1][0] * m[3][1] + m[0][0] * m[1][2] * m[3][1] + m[0][1] * m[1][0] * m[3][2] - m[0][0] * m[1][1] * m[3][2]) * inv_det,
		(m[0][1] * m[1][2] * m[2][0] - m[0][2] * m[1][1] * m[2][0] + m[0][2] * m[1][0] * m[2][1] - m[0][0] * m[1][2] * m[2][1] - m[0][1] * m[1][0] * m[2][2] + m[0][0] * m[1][1] * m[2][2]) * inv_det
	};
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

template <typename T, std::size_t N, std::size_t M, std::size_t P>
constexpr matrix<T, P, M> mul(const matrix<T, N, M>& a, const matrix<T, P, N>& b) noexcept
{
	matrix<T, P, M> c = matrix<T, P, M>::zero();
	
	for (std::size_t i = 0; i < P; ++i)
	{
		for (std::size_t j = 0; j < M; ++j)
		{
			for (std::size_t k = 0; k < N; ++k)
			{
				c[i][j] += a[k][j] * b[i][k];
			}
		}
	}
	
	return c;
}

/// @private
template <class T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr matrix<T, N, M> mul(const matrix<T, N, M>& a, T b, std::index_sequence<I...>) noexcept
{
	return {(a[I] * b) ...};
}

template <class T, std::size_t N, std::size_t M>
constexpr matrix<T, N, M> mul(const matrix<T, N, M>& a, T b) noexcept
{
	return mul(a, b, std::make_index_sequence<N>{});
}

/// @private
template <typename T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr typename matrix<T, N, M>::column_vector_type mul(const matrix<T, N, M>& a, const typename matrix<T, N, M>::row_vector_type& b, std::index_sequence<I...>) noexcept
{
	return ((a[I] * b[I]) + ...);
}

template <typename T, std::size_t N, std::size_t M>
constexpr typename matrix<T, N, M>::column_vector_type mul(const matrix<T, N, M>& a, const typename matrix<T, N, M>::row_vector_type& b) noexcept
{
	return mul(a, b, std::make_index_sequence<N>{});
}

/// @private
template <typename T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr typename matrix<T, N, M>::row_vector_type mul(const typename matrix<T, N, M>::column_vector_type& a, const matrix<T, N, M>& b, std::index_sequence<I...>) noexcept
{
	return {dot(a, b[I]) ...};
}

template <typename T, std::size_t N, std::size_t M>
constexpr typename matrix<T, N, M>::row_vector_type mul(const typename matrix<T, N, M>::column_vector_type& a, const matrix<T, N, M>& b) noexcept
{
	return mul(a, b, std::make_index_sequence<N>{});
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

/// @private
template <class T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr matrix<T, N, M> sub(const matrix<T, N, M>& a, const matrix<T, N, M>& b, std::index_sequence<I...>) noexcept
{
	return {(a[I] - b[I]) ...};
}

template <class T, std::size_t N, std::size_t M>
constexpr matrix<T, N, M> sub(const matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept
{
	return sub(a, b, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr matrix<T, N, M> sub(const matrix<T, N, M>& a, T b, std::index_sequence<I...>) noexcept
{
	return {(a[I] - b) ...};
}

template <class T, std::size_t N, std::size_t M>
constexpr matrix<T, N, M> sub(const matrix<T, N, M>& a, T b) noexcept
{
	return sub(a, b, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr matrix<T, N, M> sub(T a, const matrix<T, N, M>& b, std::index_sequence<I...>) noexcept
{
	return {(a - b[I]) ...};
}

template <class T, std::size_t N, std::size_t M>
constexpr matrix<T, N, M> sub(T a, const matrix<T, N, M>& b) noexcept
{
	return sub(a, b, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr T trace(const matrix<T, N, N>& m, std::index_sequence<I...>) noexcept
{
	return ((m[I][I]) + ...);
}

template <class T, std::size_t N>
constexpr T trace(const matrix<T, N, N>& m) noexcept
{
	return trace(m, std::make_index_sequence<N>{});
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

/// @private
template <typename T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr typename matrix<T, M, N>::column_vector_type transpose_column(const matrix<T, N, M>& m, std::size_t i, std::index_sequence<I...>) noexcept
{
	return {m[I][i] ...};
}

/// @private
template <typename T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr matrix<T, M, N> transpose(const matrix<T, N, M>& m, std::index_sequence<I...>) noexcept
{
	return {transpose_column(m, I, std::make_index_sequence<N>{}) ...};
}

template <typename T, std::size_t N, std::size_t M>
constexpr matrix<T, M, N> transpose(const matrix<T, N, M>& m) noexcept
{
	return transpose(m, std::make_index_sequence<M>{});
}

namespace operators {

/// @copydoc add(const matrix<T, N, M>&, const matrix<T, N, M>&)
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M> operator+(const matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept
{
	return add(a, b);
}

/// @copydoc add(const matrix<T, N, M>&, T)
/// @{
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M> operator+(const matrix<T, N, M>& a, T b) noexcept
{
	return add(a, b);
}
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M> operator+(T a, const matrix<T, N, M>& b) noexcept
{
	return add(b, a);
}
/// @}

/// @copydoc div(const matrix<T, N, M>&, const matrix<T, N, M>&)
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M> operator/(const matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept
{
	return div(a, b);
}

/// @copydoc div(const matrix<T, N, M>&, T)
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M> operator/(const matrix<T, N, M>& a, T b) noexcept
{
	return div(a, b);
}

/// @copydoc div(T, const matrix<T, N, M>&)
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M> operator/(T a, const matrix<T, N, M>& b) noexcept
{
	return div(a, b);
}

/// @copydoc mul(const matrix<T, N, M>&, const matrix<T, P, N>&)
template <typename T, std::size_t N, std::size_t M, std::size_t P>
inline constexpr matrix<T, P, M> operator*(const matrix<T, N, M>& a, const matrix<T, P, N>& b) noexcept
{
	return mul(a, b);
}

/// @copydoc mul(const matrix<T, N, M>&, T)
/// @{
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M> operator*(const matrix<T, N, M>& a, T b) noexcept
{
	return mul(a, b);
}
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M> operator*(T a, const matrix<T, N, M>& b) noexcept
{
	return mul(b, a);
}
/// @}

/// @copydoc mul(const matrix<T, N, M>&, const typename matrix<T, N, M>::row_vector_type&)
template <typename T, std::size_t N, std::size_t M>
inline constexpr typename matrix<T, N, M>::column_vector_type operator*(const matrix<T, N, M>& a, const typename matrix<T, N, M>::row_vector_type& b) noexcept
{
	return mul(a, b);
}

/// @copydoc mul(const typename matrix<T, N, M>::column_vector_type&, const matrix<T, N, M>&)
template <typename T, std::size_t N, std::size_t M>
inline constexpr typename matrix<T, N, M>::row_vector_type operator*(const typename matrix<T, N, M>::column_vector_type& a, const matrix<T, N, M>& b) noexcept
{
	return mul(a, b);
}

/// @copydoc sub(const matrix<T, N, M>&, const matrix<T, N, M>&)
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M> operator-(const matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept
{
	return sub(a, b);
}

/// @copydoc sub(const matrix<T, N, M>&, T)
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M> operator-(const matrix<T, N, M>& a, T b) noexcept
{
	return sub(a, b);
}

/// @copydoc sub(T, const matrix<T, N, M>&)
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M> operator-(T a, const matrix<T, N, M>& b) noexcept
{
	return sub(a, b);
}

/**
 * Adds two values and stores the result in the first value.
 *
 * @param a First value.
 * @param b Second value.
 *
 * @return Reference to the first value.
 */
/// @{
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M>& operator+=(matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept
{
	return (a = a + b);
}
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M>& operator+=(matrix<T, N, M>& a, T b) noexcept
{
	return (a = a + b);
}
/// @}

/**
 * Subtracts the first value by the second value and stores the result in the first value.
 *
 * @param a First value.
 * @param b Second value.
 *
 * @return Reference to the first value.
 */
/// @{
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M>& operator-=(matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept
{
	return (a = a - b);
}
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M>& operator-=(matrix<T, N, M>& a, T b) noexcept
{
	return (a = a - b);
}
/// @}

/**
 * Multiplies two values and stores the result in the first value.
 *
 * @param a First value.
 * @param b Second value.
 *
 * @return Reference to the first value.
 */
/// @{
template <class T, std::size_t N>
inline constexpr matrix<T, N, N>& operator*=(matrix<T, N, N>& a, const matrix<T, N, N>& b) noexcept
{
	return (a = a * b);
}
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M>& operator*=(matrix<T, N, M>& a, T b) noexcept
{
	return (a = a * b);
}
/// @}

/**
 * Divides the first value by the second value and stores the result in the first value.
 *
 * @param a First value.
 * @param b Second value.
 *
 * @return Reference to the first value.
 */
/// @{
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M>& operator/=(matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept
{
	return (a = a / b);
}
template <class T, std::size_t N, std::size_t M>
inline constexpr matrix<T, N, M>& operator/=(matrix<T, N, M>& a, T b) noexcept
{
	return (a = a / b);
}
/// @}

/**
 * Writes the elements of a matrix to an output stream, with each element delimeted by a space.
 *
 * @param os Output stream.
 * @param m Matrix.
 *
 * @return Output stream.
 */
template <class T, std::size_t N, std::size_t M>
std::ostream& operator<<(std::ostream& os, const matrix<T, N, M>& m)
{
	for (std::size_t i = 0; i < m.size(); ++i)
	{
		if (i)
			os << ' ';
		os << m.element(i);
	}
	
	return os;
}

/**
 * Reads the elements of a matrix from an input stream, with each element delimeted by a space.
 *
 * @param is Input stream.
 * @param m Matrix.
 *
 * @return Input stream.
 */
template <class T, std::size_t N, std::size_t M>
std::istream& operator>>(std::istream& is, matrix<T, N, M>& m)
{
	for (std::size_t i = 0; i < m.size(); ++i)
		is >> m.element(i);
	
	return is;
}

} // namespace operators

} // namespace math

using namespace math::operators;

// Structured binding support
namespace std
{
	/**
	 * Provides access to the number of columns in a math::matrix as a compile-time constant expression.
	 *
	 * @tparam T Element type.
	 * @tparam N Number of columns.
	 * @tparam M Number of rows.
	 */
	template<class T, std::size_t N, std::size_t M>
	struct tuple_size<math::matrix<T, N, M>>
	{
		/// Number of columns in the matrix. 
		static constexpr std::size_t value = math::matrix<T, N, M>::column_count;
	};
	
	/**
	 * Provides compile-time indexed access to the type of the columns in a math::matrix using a tuple-like interface.
	 *
	 * @tparam I Index of a column.
	 * @tparam T Element type.
	 * @tparam N Number of columns.
	 * @tparam M Number of rows.
	 */
	template<std::size_t I, class T, std::size_t N, std::size_t M>
	struct tuple_element<I, math::matrix<T, N, M>>
	{
		/// Type of columns in the matrix.
		using type = math::matrix<T, N, M>::column_vector_type;
	};
}

#endif // ANTKEEPER_MATH_MATRIX_HPP
