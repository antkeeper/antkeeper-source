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

#ifndef ANTKEEPER_MATH_VECTOR_FUNCTIONS_HPP
#define ANTKEEPER_MATH_VECTOR_FUNCTIONS_HPP

#include "math/vector-type.hpp"
#include <algorithm>
#include <cmath>
#include <type_traits>
#include <utility>

namespace math {

/// @addtogroup vector
/// @{

/**
 * Adds two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Sum of the two vectors.
 */
template <class T, std::size_t N>
vector<T, N> add(const vector<T, N>& x, const vector<T, N>& y);

/**
 * Checks if all elements of a boolean vector are `true`.
 *
 * @param x Vector to be tested for truth.
 * @return `true` if all elements are `true`, `false` otherwise.
 */
template <std::size_t N>
bool all(const vector<bool, N>& x);

/**
 * Checks if any elements of a boolean vector are `true`.
 *
 * @param x Vector to be tested for truth.
 * @return `true` if any elements are `true`, `false` otherwise.
 */
template <std::size_t N>
bool any(const vector<bool, N>& x);

/**
 * Reinterprets data as an `N`-dimensional vector of type `T`.
 *
 * @tparam N Number of vector dimensions.
 * @tparam T Scalar type.
 * @param data Data to reinterpret.
 */
template <std::size_t N, typename T>
vector<T, N>& as_vector(T& data);

/**
 * Clamps the values of a vector's elements.
 *
 * @param x Vector to clamp.
 * @param min_value Minimum element value.
 * @param max_value Maximum element value.
 * @return Clamped vector.
 */
template <class T, std::size_t N>
vector<T, N> clamp(const vector<T, N>& x, T min_value, T max_value);

/**
 * Clamps the length of a vector.
 *
 * @param x Vector to clamp.
 * @param max_length Maximum length.
 * @return Length-clamped vector.
 */
template <class T, std::size_t N>
vector<T, N> clamp_length(const vector<T, N>& x, T max_length);

/**
 * Calculate the cross product of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Cross product of the two vectors.
 */
template <class T>
vector<T, 3> cross(const vector<T, 3>& x, const vector<T, 3>& y);

/**
 * Calculates the distance between two points.
 *
 * @param p0 First of two points.
 * @param p1 Second of two points.
 * @return Distance between the two points.
 */
template <class T, std::size_t N>
vector<T, N> distance(const vector<T, N>& p0, const vector<T, N>& p1);

/**
 * Calculates the squared distance between two points. The squared distance can be calculated faster than the distance because a call to `std::sqrt` is saved.
 *
 * @param p0 First of two points.
 * @param p1 Second of two points.
 * @return Squared distance between the two points.
 */
template <class T, std::size_t N>
vector<T, N> distance_squared(const vector<T, N>& p0, const vector<T, N>& p1);

/**
 * Divides a vector by another vector.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Result of the division.
 */
template <class T, std::size_t N>
vector<T, N> div(const vector<T, N>& x, const vector<T, N>& y);

/**
 * Divides a vector by a scalar.
 *
 * @param v Vector.
 * @param s Scalar.
 * @return Result of the division.
 */
template <class T, std::size_t N>
vector<T, N> div(const vector<T, N>& v, T s);

/**
 * Calculates the dot product of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Dot product of the two vectors.
 */
template <class T, std::size_t N>
T dot(const vector<T, N>& x, const vector<T, N>& y);

/**
 * Compares two vectors for equality
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
vector<bool, N> equal(const vector<T, N>& x, const vector<T, N>& y);

/**
 * Performs a component-wise greater-than comparison of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
vector<bool, N> greater_than(const vector<T, N>& x, const vector<T, N>& y);

/**
 * Performs a component-wise greater-than or equal-to comparison of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
vector<bool, N> greater_than_equal(const vector<T, N>& x, const vector<T, N>& y);

/**
 * Calculates the length of a vector.
 *
 * @param x Vector of which to calculate the length.
 * @return Length of the vector.
 */
template <class T, std::size_t N>
T length(const vector<T, N>& x);

/**
 * Calculates the squared length of a vector. The squared length can be calculated faster than the length because a call to `std::sqrt` is saved.
 *
 * @param x Vector of which to calculate the squared length.
 * @return Squared length of the vector.
 */
template <class T, std::size_t N>
T length_squared(const vector<T, N>& x);

/**
 * Performs a component-wise less-than comparison of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
vector<bool, N> less_than(const vector<T, N>& x, const vector<T, N>& y);

/**
 * Performs a component-wise less-than or equal-to comparison of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
vector<bool, N> less_than_equal(const vector<T, N>& x, const vector<T, N>& y);

/**
 * Multiplies two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Product of the two vectors.
 */
template <class T, std::size_t N>
vector<T, N> mul(const vector<T, N>& x, const vector<T, N>& y);

/**
 * Multiplies a vector by a scalar.
 *
 * @param v Vector.
 * @param s Scalar.
 * @return Product of the vector and scalar.
 */
template <class T, std::size_t N>
vector<T, N> mul(const vector<T, N>& v, T s);

/**
 * Negates a vector.
 *
 * @param x Vector to negate.
 * @return Negated vector.
 */
template <class T, std::size_t N>
vector<T, N> negate(const vector<T, N>& x);

/**
 * Calculates the unit vector in the same direction as the original vector.
 *
 * @param x Vector to normalize.
 * @return Normalized vector.
 */
template <class T, std::size_t N>
vector<T, N> normalize(const vector<T, N>& x);

/**
 * Logically inverts a boolean vector.
 *
 * @param x Vector to be inverted.
 * @return Logically inverted vector.
 */
template <class T, std::size_t N>
vector<bool, N> not(const vector<T, N>& x);

/**
 * Compares two vectors for inequality
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
vector<bool, N> not_equal(const vector<T, N>& x, const vector<T, N>& y);

/**
 * Resizes a vector. Any new elements will be set to `0`.
 *
 * @param v Vector to resize.
 * @return Resized vector.
 */
template <std::size_t N1, class T, std::size_t N0>
vector<T, N1> resize(const vector<T, N0>& v);

/**
 * Subtracts a vector from another vector.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Difference between the two vectors.
 */
template <class T, std::size_t N>
vector<T, N> sub(const vector<T, N>& x, const vector<T, N>& y);

/**
 * Makes an m-dimensional vector by rearranging and/or duplicating elements of an n-dimensional vector.
 *
 * @tparam Indices List of indices of elements in the vector `v`.
 * @tparam T Vector component type.
 * @tparam N Number of dimensions in vector `v`.
 * @return Vector containing elements from vector `v` in the order specified by `Indices`. The size of the returned vector is equivalent to the number of indices in `Indices`.
 */
template <std::size_t... Indices, class T, std::size_t N>
vector<T, sizeof...(Indices)> swizzle(const vector<T, N>& v);

/**
 * Types casts each vector component and returns a vector of the casted type.
 *
 * @tparam T2 Target vector component type.
 * @tparam T1 Source vector component type.
 * @tparam N Number of dimensions.
 * @param v Vector to type cast.
 * @return Type-casted vector.
 */
template <class T2, class T1, std::size_t N>
vector<T2, N> type_cast(const vector<T1, N>& v);

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<T, N> add(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {(x[I] + y[I])...};
}

template <class T, std::size_t N>
inline vector<T, N> add(const vector<T, N>& x, const vector<T, N>& y)
{
	return add(x, y, std::make_index_sequence<N>{});
}

/// @private
template <std::size_t N, std::size_t... I>
inline bool all(const vector<bool, N>& x, std::index_sequence<I...>)
{
	return (x[I] && ...);
}

template <std::size_t N>
inline bool all(const vector<bool, N>& x)
{
	return all(x, std::make_index_sequence<N>{});
}

/// @private
template <std::size_t N, std::size_t... I>
inline bool any(const vector<bool, N>& x, std::index_sequence<I...>)
{
	return (x[I] || ...);
}

template <std::size_t N>
inline bool any(const vector<bool, N>& x)
{
	return any(x, std::make_index_sequence<N>{});
}

template <std::size_t N, typename T>
inline vector<T, N>& as_vector(T& data)
{
	static_assert(std::is_pod<vector<T, N>>::value);
	return reinterpret_cast<vector<T, N>&>(data);
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<T, N> clamp(const vector<T, N>& x, T min_value, T max_value, std::index_sequence<I...>)
{
	return {std::min<T>(max_value, std::max<T>(min_value, x[I]))...};
}

template <class T, std::size_t N>
inline vector<T, N> clamp(const vector<T, N>& x, T min_value, T max_value)
{
	return clamp(x, min_value, max_value, std::make_index_sequence<N>{});
}

template <class T, std::size_t N>
vector<T, N> clamp_length(const vector<T, N>& x, T max_length)
{
	T length2 = length_squared(x);
	return (length2 > max_length * max_length) ? (x * max_length / std::sqrt(length2)) : x;
}

template <class T>
inline vector<T, 3> cross(const vector<T, 3>& x, const vector<T, 3>& y)
{
	return
		{
			x[1] * y[2] - y[1] * x[2],
			x[2] * y[0] - y[2] * x[0],
			x[0] * y[1] - y[0] * x[1]
		};
}

template <class T, std::size_t N>
inline vector<T, N> distance(const vector<T, N>& p0, const vector<T, N>& p1)
{
	static_assert(std::is_floating_point<T>::value);
	return length(sub(p0, p1));
}

template <class T, std::size_t N>
inline vector<T, N> distance_squared(const vector<T, N>& p0, const vector<T, N>& p1)
{
	static_assert(std::is_floating_point<T>::value);
	return length_squared(sub(p0, p1));
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<T, N> div(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {(x[I] / y[I])...};
}

template <class T, std::size_t N>
inline vector<T, N> div(const vector<T, N>& x, const vector<T, N>& y)
{
	return div(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<T, N> div(const vector<T, N>& v, T s, std::index_sequence<I...>)
{
	return {(v[I] / s)...};
}

template <class T, std::size_t N>
inline vector<T, N> div(const vector<T, N>& v, T s)
{
	return div(v, s, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline T dot(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return ((x[I] * y[I]) + ...);
}

template <class T, std::size_t N>
inline T dot(const vector<T, N>& x, const vector<T, N>& y)
{
	return dot(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<bool, N> equal(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {(x[I] == y[I])...};
}

template <class T, std::size_t N>
inline vector<bool, N> equal(const vector<T, N>& x, const vector<T, N>& y)
{
	return equal(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<bool, N> greater_than(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {(x[I] > y[I])...};
}

template <class T, std::size_t N>
inline vector<bool, N> greater_than(const vector<T, N>& x, const vector<T, N>& y)
{
	return greater_than(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<bool, N> greater_than_equal(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {(x[I] >= y[I])...};
}

template <class T, std::size_t N>
inline vector<bool, N> greater_than_equal(const vector<T, N>& x, const vector<T, N>& y)
{
	return greater_than_equal(x, y, std::make_index_sequence<N>{});
}

template <class T, std::size_t N>
inline T length(const vector<T, N>& x)
{
	static_assert(std::is_floating_point<T>::value);
	return std::sqrt(dot(x, x));
}

template <class T, std::size_t N>
inline T length_squared(const vector<T, N>& x)
{
	static_assert(std::is_floating_point<T>::value);
	return dot(x, x);
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<bool, N> less_than(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {(x[I] < y[I])...};
}

template <class T, std::size_t N>
inline vector<bool, N> less_than(const vector<T, N>& x, const vector<T, N>& y)
{
	return less_than(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<bool, N> less_than_equal(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {(x[I] <= y[I])...};
}

template <class T, std::size_t N>
inline vector<bool, N> less_than_equal(const vector<T, N>& x, const vector<T, N>& y)
{
	return less_than_equal(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<T, N> mul(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {(x[I] * y[I])...};
}

template <class T, std::size_t N>
inline vector<T, N> mul(const vector<T, N>& x, const vector<T, N>& y)
{
	return mul(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<T, N> mul(const vector<T, N>& v, T s, std::index_sequence<I...>)
{
	return {(v[I] * s)...};
}

template <class T, std::size_t N>
inline vector<T, N> mul(const vector<T, N>& v, T s)
{
	return mul(v, s, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<T, N> negate(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {(-x[I])...};
}

template <class T, std::size_t N>
inline vector<T, N> negate(const vector<T, N>& x)
{
	return negate(x, std::make_index_sequence<N>{});
}

template <class T, std::size_t N>
inline vector<T, N> normalize(const vector<T, N>& x)
{
	static_assert(std::is_floating_point<T>::value);
	return mul(x, T(1) / length(x));
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<bool, N> not(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {!x[I]...};
}

template <class T, std::size_t N>
inline vector<bool, N> not(const vector<T, N>& x)
{
	return not(x, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<bool, N> not_equal(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {(x[I] != y[I])...};
}

template <class T, std::size_t N>
inline vector<bool, N> not_equal(const vector<T, N>& x, const vector<T, N>& y)
{
	return not_equal(x, y, std::make_index_sequence<N>{});
}

template <std::size_t N1, class T, std::size_t N0>
vector<T, N1> resize(const vector<T, N0>& v)
{
	vector<T, N1> resized;

	for (std::size_t i = 0; i < N1; ++i)
	{
		resized[i] = (i < N0) ? v[i] : T(0);
	}

	return resized;
}


/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<T, N> sub(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {(x[I] - y[I])...};
}

template <class T, std::size_t N>
inline vector<T, N> sub(const vector<T, N>& x, const vector<T, N>& y)
{
	return sub(x, y, std::make_index_sequence<N>{});
}

template <std::size_t... Indices, class T, std::size_t N>
inline vector<T, sizeof...(Indices)> swizzle(const vector<T, N>& v)
{
	return { v[Indices]... };
}

/// @private
template <class T2, class T1, std::size_t N, std::size_t... I>
inline vector<T2, N> type_cast(const vector<T1, N>& v, std::index_sequence<I...>)
{
	return {static_cast<T2>(v[I])...};
}

template <class T2, class T1, std::size_t N>
inline vector<T2, N> type_cast(const vector<T1, N>& v)
{
	return type_cast<T2>(v, std::make_index_sequence<N>{}); 
}

/// @}

} // namespace math

#endif // ANTKEEPER_MATH_VECTOR_FUNCTIONS_HPP

