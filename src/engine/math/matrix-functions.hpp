// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_MATRIX_FUNCTIONS_HPP
#define ANTKEEPER_MATH_MATRIX_FUNCTIONS_HPP

#include <engine/math/matrix-types.hpp>
#include <engine/math/vector.hpp>
#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

// export module math.matrix:functions;
// import :type;
// import math.vector;
// import <cstddef>;
// import <iterator>;
// import <tuple>;
// import <type_traits>;
// import <utility>;

namespace math {
namespace functions {

/// Adds two matrices.
/// @param a First matrix.
/// @param b Second matrix.
/// @return Sum of the two matrices.
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> add(const matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept;

/// Adds a matrix and a scalar.
/// @param a Matrix.
/// @param b scalar.
/// @return Sum of the matrix and scalar.
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> add(const matrix<T, N, M>& a, T b) noexcept;

/// Calculates the determinant of a square matrix.
/// @param m Matrix of which to take the determinant.
/// @return Determinant of @p m.
/// @warning Currently only implemented for 2x2, 3x3, and 4x4 matrices.
template <class T, std::size_t N>
[[nodiscard]] constexpr T determinant(const matrix<T, N, N>& m) noexcept;

/// Performs a component-wise multiplication of two matrices.
/// @param a First matrix multiplicand.
/// @param b Second matrix multiplicand.
/// @return Product of the component-wise multiplcation.
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> componentwise_mul(const matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept;

/// Divides a matrix by a matrix.
/// @param a First matrix.
/// @param b Second matrix.
/// @return Result of the division.
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> div(const matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept;

/// Divides a matrix by a scalar.
/// @param a Matrix.
/// @param b Scalar.
/// @return Result of the division.
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> div(const matrix<T, N, M>& a, T b) noexcept;

/// Divides a scalar by a matrix.
/// @param a Scalar.
/// @param b Matrix.
/// @return Result of the division.
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> div(T a, const matrix<T, N, M>& b) noexcept;

/// Calculates the inverse of a square matrix.
/// @param m Square matrix.
/// @return Inverse of matrix @p m.
/// @warning Currently only implemented for 2x2, 3x3, and 4x4 matrices.
template <class T, std::size_t N>
[[nodiscard]] constexpr matrix<T, N, N> inverse(const matrix<T, N, N>& m) noexcept;

/// Constructs a right-handed viewing transformation matrix.
/// @param position Position of the view point.
/// @param target Position of the target.
/// @param up Unit vector pointing in the up direction.
/// @return Viewing transformation matrix.
template <class T>
[[nodiscard]] constexpr mat4<T> look_at_rh(const vec3<T>& position, const vec3<T>& target, const vec3<T>& up);

/// Constructs a right-handed viewing transformation matrix and its inverse.
/// @param position Position of the view point.
/// @param target Position of the target.
/// @param up Unit vector pointing in the up direction.
/// @return Tuple containing the viewing transformation matrix, followed by its inverse.
/// @note Constructing the inverse viewing transformation matrix from viewing parameters is faster and more precise than inverting matrix.
template <class T>
[[nodiscard]] constexpr std::tuple<mat4<T>, mat4<T>> look_at_rh_inv(const vec3<T>& position, const vec3<T>& target, const vec3<T>& up);

/// Multiplies two matrices
/// @tparam T Matrix element type.
/// @tparam N Number of columns in matrix @p a, and rows in matrix @p b.
/// @tparam M Number of rows in matrix @p a.
/// @tparam P Number of columns in matrix @p b.
/// @param a First matrix.
/// @param b Second matrix.
/// @return Product of `a * b`.
template <class T, std::size_t N, std::size_t M, std::size_t P>
[[nodiscard]] constexpr matrix<T, P, M> mul(const matrix<T, N, M>& a, const matrix<T, P, N>& b) noexcept;

/// Multiplies a matrix by a scalar.
/// @param a Matrix.
/// @param b Scalar.
/// @return Product of the matrix and the scalar.
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> mul(const matrix<T, N, M>& a, T b) noexcept;

/// Calculates the product of a matrix and a row vector.
/// @param a Matrix.
/// @param b Row vector
/// @return Product of the matrix and the row vector.
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr typename matrix<T, N, M>::column_type mul(const matrix<T, N, M>& a, const typename matrix<T, N, M>::row_type& b) noexcept;

/// Calculates the product of a column vector and a matrix.
/// @param a Column vector.
/// @param b Matrix.
/// @return Product of the column vector and the matrix.
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr typename matrix<T, N, M>::row_type mul(const typename matrix<T, N, M>::column_type& a, const matrix<T, N, M>& b) noexcept;

/// Constructs a rotation matrix.
/// @param angle Angle of rotation, in radians.
/// @param axis Axis of rotation.
/// @return Rotation matrix.
template <class T>
[[nodiscard]] mat3<T> rotate(T angle, const vector<T, 3>& axis);

/// Produces a matrix which rotates Cartesian coordinates about the x-axis by a given angle.
/// @param angle Angle of rotation, in radians.
/// @return Rotation matrix.
template <class T>
[[nodiscard]] mat3<T> rotate_x(T angle);

/// Produces a matrix which rotates Cartesian coordinates about the y-axis by a given angle.
/// @param angle Angle of rotation, in radians.
/// @return Rotation matrix.
template <class T>
[[nodiscard]] mat3<T> rotate_y(T angle);

/// Produces a matrix which rotates Cartesian coordinates about the z-axis by a given angle.
/// @param angle Angle of rotation, in radians.
/// @return Rotation matrix.
template <class T>
[[nodiscard]] mat3<T> rotate_z(T angle);

/// Constructs a scale matrix.
/// @param v Scale vector.
/// @return Scale matrix.
template <class T>
[[nodiscard]] constexpr mat4<T> scale(const vec3<T>& v);

/// Subtracts a matrix from another matrix.
/// @param a First matrix.
/// @param b Second matrix.
/// @return Difference between the two matrices.
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> sub(const matrix<T, N, M>& a, const matrix<T, N, M>& b) noexcept;

/// Subtracts a scalar from matrix.
/// @param a Matrix.
/// @param b Scalar.
/// @return Difference between the matrix and scalar.
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> sub(const matrix<T, N, M>& a, T b) noexcept;

/// Subtracts a matrix from a scalar.
/// @param a Scalar.
/// @param b Matrix.
/// @return Difference between the scalar and matrix.
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] constexpr matrix<T, N, M> sub(T a, const matrix<T, N, M>& b) noexcept;

/// Calculates the trace of a square matrix.
/// @param m Square matrix.
/// @return Sum of elements on the main diagonal.
template <class T, std::size_t N>
[[nodiscard]] constexpr T trace(const matrix<T, N, N>& m) noexcept;

/// Constructs a translation matrix.
/// @param v Translation vector.
/// @return Translation matrix.
template <class T>
[[nodiscard]] constexpr mat4<T> translate(const vec3<T>& v);

/// Calculates the transpose of a matrix.
/// @param m Matrix to transpose.
/// @return Transposed matrix.
template <class T, std::size_t N, std::size_t M>
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
constexpr T determinant(const mat3<T>& m) noexcept
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
constexpr mat3<T> inverse(const mat3<T>& m) noexcept
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
constexpr mat4<T> look_at_rh(const vec3<T>& position, const vec3<T>& target, const vec3<T>& up)
{
	const auto f = normalize(target - position);
	const auto r = normalize(cross(f, up));
	const auto u = cross(r, f);
	const auto t = vec3<T>{dot(position, r), dot(position, u), dot(position, f)};
	
	return
	{{
		{ r.x(),  u.x(), -f.x(), T{0}},
		{ r.y(),  u.y(), -f.y(), T{0}},
		{ r.z(),  u.z(), -f.z(), T{0}},
		{-t.x(), -t.y(),  t.z(), T{1}}
	}};
}

template <class T>
constexpr std::tuple<mat4<T>, mat4<T>> look_at_rh_inv(const vec3<T>& position, const vec3<T>& target, const vec3<T>& up)
{
	const auto f = normalize(target - position);
	const auto r = normalize(cross(f, up));
	const auto u = cross(r, f);
	
	return
	{
		mat4<T>
		{{
			{ r.x(),  u.x(), -f.x(), T{0}},
			{ r.y(),  u.y(), -f.y(), T{0}},
			{ r.z(),  u.z(), -f.z(), T{0}},
			{-dot(position, r), -dot(position, u), dot(position, f), T{1}}
		}},
		
		mat4<T>
		{{
			{ r.x(),  r.y(),  r.z(), T{0}},
			{ u.x(),  u.y(),  u.z(), T{0}},
			{-f.x(), -f.y(), -f.z(), T{0}},
			{position.x(), position.y(), position.z(), T{1}}
		}}
	};
}

template <class T, std::size_t N, std::size_t M, std::size_t P>
constexpr matrix<T, P, M> mul(const matrix<T, N, M>& a, const matrix<T, P, N>& b) noexcept
{
	matrix<T, P, M> c = {};
	
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
template <class T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr typename matrix<T, N, M>::column_type mul(const matrix<T, N, M>& a, const typename matrix<T, N, M>::row_type& b, std::index_sequence<I...>) noexcept
{
	return ((a[I] * b[I]) + ...);
}

template <class T, std::size_t N, std::size_t M>
constexpr typename matrix<T, N, M>::column_type mul(const matrix<T, N, M>& a, const typename matrix<T, N, M>::row_type& b) noexcept
{
	return mul(a, b, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr typename matrix<T, N, M>::row_type mul(const typename matrix<T, N, M>::column_type& a, const matrix<T, N, M>& b, std::index_sequence<I...>) noexcept
{
	return {dot(a, b[I]) ...};
}

template <class T, std::size_t N, std::size_t M>
constexpr typename matrix<T, N, M>::row_type mul(const typename matrix<T, N, M>::column_type& a, const matrix<T, N, M>& b) noexcept
{
	return mul(a, b, std::make_index_sequence<N>{});
}

template <class T>
mat3<T> rotate(T angle, const vector<T, 3>& axis)
{
	const T c = std::cos(angle);
	const T s = std::sin(angle);
	const vector<T, 3> temp = mul(axis, T{1} - c);
	
	mat3<T> rotation;
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
mat3<T> rotate_x(T angle)
{
	const T c = std::cos(angle);
	const T s = std::sin(angle);
	
	return mat3<T>
	{
		T{1}, T{0}, T{0},
		T{0}, c, s,
		T{0}, -s, c
	};
}

template <class T>
mat3<T> rotate_y(T angle)
{
	const T c = std::cos(angle);
	const T s = std::sin(angle);
	
	return mat3<T>
	{
		c, T{0}, -s,
		T{0}, T{1}, T{0},
		s, T{0}, c
	};
}

template <class T>
mat3<T> rotate_z(T angle)
{
	const T c = std::cos(angle);
	const T s = std::sin(angle);
	
	return mat3<T>
	{
		c, s, T{0},
		-s, c, T{0},
		T{0}, T{0}, T{1}
	};
}

template <class T>
constexpr mat4<T> scale(const vec3<T>& v)
{
	return
	{{
		{v[0], T{0}, T{0}, T{0}},
		{T{0}, v[1], T{0}, T{0}},
		{T{0}, T{0}, v[2], T{0}},
		{T{0}, T{0}, T{0}, T{1}}
	}};
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
constexpr mat4<T> translate(const vec3<T>& v)
{
	return
	{{
		{T{1}, T{0}, T{0}, T{0}},
		{T{0}, T{1}, T{0}, T{0}},
		{T{0}, T{0}, T{1}, T{0}},
		{v[0], v[1], v[2], T{1}}
	}};
}

/// @private
template <class T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr typename matrix<T, M, N>::column_type transpose_column(const matrix<T, N, M>& m, std::size_t i, std::index_sequence<I...>) noexcept
{
	return {m[I][i] ...};
}

/// @private
template <class T, std::size_t N, std::size_t M, std::size_t... I>
inline constexpr matrix<T, M, N> transpose(const matrix<T, N, M>& m, std::index_sequence<I...>) noexcept
{
	return {transpose_column(m, I, std::make_index_sequence<N>{}) ...};
}

template <class T, std::size_t N, std::size_t M>
constexpr matrix<T, M, N> transpose(const matrix<T, N, M>& m) noexcept
{
	return transpose(m, std::make_index_sequence<M>{});
}

} // namespace functions

// Bring math::functions into math namespace
using namespace functions;

} // namespace math

/// @copydoc math::functions::add(const math::matrix<T, N, M>&, const math::matrix<T, N, M>&)
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr math::matrix<T, N, M> operator+(const math::matrix<T, N, M>& a, const math::matrix<T, N, M>& b) noexcept
{
	return math::add(a, b);
}

/// @copydoc math::functions::add(const math::matrix<T, N, M>&, T)
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr math::matrix<T, N, M> operator+(const math::matrix<T, N, M>& a, T b) noexcept
{
	return math::add(a, b);
}

/// @copydoc math::functions::add(const math::matrix<T, N, M>&, T)
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr math::matrix<T, N, M> operator+(T a, const math::matrix<T, N, M>& b) noexcept
{
	return math::add(b, a);
}

/// @copydoc math::functions::div(const math::matrix<T, N, M>&, const math::matrix<T, N, M>&)
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr math::matrix<T, N, M> operator/(const math::matrix<T, N, M>& a, const math::matrix<T, N, M>& b) noexcept
{
	return math::div(a, b);
}

/// @copydoc math::functions::div(const math::matrix<T, N, M>&, T)
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr math::matrix<T, N, M> operator/(const math::matrix<T, N, M>& a, T b) noexcept
{
	return math::div(a, b);
}

/// @copydoc math::functions::div(T, const math::matrix<T, N, M>&)
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr math::matrix<T, N, M> operator/(T a, const math::matrix<T, N, M>& b) noexcept
{
	return math::div(a, b);
}

/// @copydoc math::functions::mul(const math::matrix<T, N, M>&, math::const matrix<T, P, N>&)
template <class T, std::size_t N, std::size_t M, std::size_t P>
[[nodiscard]] inline constexpr math::matrix<T, P, M> operator*(const math::matrix<T, N, M>& a, const math::matrix<T, P, N>& b) noexcept
{
	return math::mul(a, b);
}

/// @copydoc math::functions::mul(const math::matrix<T, N, M>&, T)
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr math::matrix<T, N, M> operator*(const math::matrix<T, N, M>& a, T b) noexcept
{
	return math::mul(a, b);
}

/// @copydoc math::functions::mul(const math::matrix<T, N, M>&, T)
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr math::matrix<T, N, M> operator*(T a, const math::matrix<T, N, M>& b) noexcept
{
	return math::mul(b, a);
}

/// @copydoc math::functions::mul(const math::matrix<T, N, M>&, const typename math::matrix<T, N, M>::row_type&)
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr typename math::matrix<T, N, M>::column_type operator*(const math::matrix<T, N, M>& a, const typename math::matrix<T, N, M>::row_type& b) noexcept
{
	return math::mul(a, b);
}

/// @copydoc math::functions::mul(const typename math::matrix<T, N, M>::column_type&, const math::matrix<T, N, M>&)
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr typename math::matrix<T, N, M>::row_type operator*(const typename math::matrix<T, N, M>::column_type& a, const math::matrix<T, N, M>& b) noexcept
{
	return math::mul(a, b);
}

/// @copydoc math::functions::sub(const math::matrix<T, N, M>&, const math::matrix<T, N, M>&)
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr math::matrix<T, N, M> operator-(const math::matrix<T, N, M>& a, const math::matrix<T, N, M>& b) noexcept
{
	return math::sub(a, b);
}

/// @copydoc math::functions::sub(const math::matrix<T, N, M>&, T)
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr math::matrix<T, N, M> operator-(const math::matrix<T, N, M>& a, T b) noexcept
{
	return math::sub(a, b);
}

/// @copydoc math::functions::sub(T, const math::matrix<T, N, M>&)
template <class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr math::matrix<T, N, M> operator-(T a, const math::matrix<T, N, M>& b) noexcept
{
	return math::sub(a, b);
}

/// Adds two values and stores the result in the first value.
/// @param a First value.
/// @param b Second value.
/// @return Reference to the first value.
template <class T, std::size_t N, std::size_t M>
inline constexpr math::matrix<T, N, M>& operator+=(math::matrix<T, N, M>& a, const math::matrix<T, N, M>& b) noexcept
{
	return (a = a + b);
}

/// Adds two values and stores the result in the first value.
/// @param a First value.
/// @param b Second value.
/// @return Reference to the first value.
template <class T, std::size_t N, std::size_t M>
inline constexpr math::matrix<T, N, M>& operator+=(math::matrix<T, N, M>& a, T b) noexcept
{
	return (a = a + b);
}

/// Subtracts the first value by the second value and stores the result in the first value.
/// @param a First value.
/// @param b Second value.
/// @return Reference to the first value.
template <class T, std::size_t N, std::size_t M>
inline constexpr math::matrix<T, N, M>& operator-=(math::matrix<T, N, M>& a, const math::matrix<T, N, M>& b) noexcept
{
	return (a = a - b);
}

/// Subtracts the first value by the second value and stores the result in the first value.
/// @param a First value.
/// @param b Second value.
/// @return Reference to the first value.
template <class T, std::size_t N, std::size_t M>
inline constexpr math::matrix<T, N, M>& operator-=(math::matrix<T, N, M>& a, T b) noexcept
{
	return (a = a - b);
}

/// Multiplies two values and stores the result in the first value.
/// @param a First value.
/// @param b Second value.
/// @return Reference to the first value.
template <class T, std::size_t N>
inline constexpr math::matrix<T, N, N>& operator*=(math::matrix<T, N, N>& a, const math::matrix<T, N, N>& b) noexcept
{
	return (a = a * b);
}

/// Multiplies two values and stores the result in the first value.
/// @param a First value.
/// @param b Second value.
/// @return Reference to the first value.
template <class T, std::size_t N, std::size_t M>
inline constexpr math::matrix<T, N, M>& operator*=(math::matrix<T, N, M>& a, T b) noexcept
{
	return (a = a * b);
}

/// Divides the first value by the second value and stores the result in the first value.
/// @param a First value.
/// @param b Second value.
/// @return Reference to the first value.
template <class T, std::size_t N, std::size_t M>
inline constexpr math::matrix<T, N, M>& operator/=(math::matrix<T, N, M>& a, const math::matrix<T, N, M>& b) noexcept
{
	return (a = a / b);
}

/// Divides the first value by the second value and stores the result in the first value.
/// @param a First value.
/// @param b Second value.
/// @return Reference to the first value.
template <class T, std::size_t N, std::size_t M>
inline constexpr math::matrix<T, N, M>& operator/=(math::matrix<T, N, M>& a, T b) noexcept
{
	return (a = a / b);
}

#endif // ANTKEEPER_MATH_MATRIX_FUNCTIONS_HPP
