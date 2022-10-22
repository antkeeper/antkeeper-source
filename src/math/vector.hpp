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

#ifndef ANTKEEPER_MATH_VECTOR_HPP
#define ANTKEEPER_MATH_VECTOR_HPP

#include <algorithm>
#include <cstddef>
#include <cmath>
#include <istream>
#include <iterator>
#include <ostream>
#include <type_traits>
#include <utility>

namespace math {

/**
 * *n*-dimensional vector.
 *
 * @tparam T Element type.
 * @tparam N Number of elements.
 */
template <typename T, std::size_t N>
struct vector
{
	/// Vector element data type.
	typedef T element_type;
	
	/// Number of vector elements.
	static constexpr std::size_t element_count = N;
	
	/// Array of vector elements.
	element_type elements[N];
	
	/**
	 * Returns a reference to the element at a given index.
	 *
	 * @param i Index of an element.
	 *
	 * @return Reference to the element at index @p i.
	 */
	/// @{
	constexpr inline element_type& operator[](std::size_t i) noexcept
	{
		return elements[i];
	}
	constexpr inline const element_type& operator[](std::size_t i) const noexcept
	{
		return elements[i];
	}
	/// @}
	
	/// Returns a reference to the first element.
	/// @{
	constexpr inline element_type& front() noexcept
	{
		return elements[0];
	}
	constexpr inline const element_type& front() const noexcept
	{
		return elements[0];
	}
	/// @}
	
	/// Returns a reference to the last element.
	/// @{
	constexpr inline element_type& back() noexcept
	{
		return elements[N - 1];
	}
	constexpr inline const element_type& back() const noexcept
	{
		return elements[N - 1];
	}
	/// @}
	
	/// Returns a reference to the first element.
	/// @{
	constexpr inline element_type& x() noexcept
	{
		static_assert(N > 0, "Vector does not contain an x element.");
		return elements[0];
	}
	constexpr inline const element_type& x() const noexcept
	{
		static_assert(N > 0, "Vector does not contain an x element.");
		return elements[0];
	}
	/// @}
	
	/// Returns a reference to the second element.
	/// @{
	constexpr inline element_type& y() noexcept
	{
		static_assert(N > 1, "Vector does not contain a y element.");
		return elements[1];
	}
	constexpr inline const element_type& y() const noexcept
	{
		static_assert(N > 1, "Vector does not contain a y element.");
		return elements[1];
	}
	/// @}
	
	/// Returns a reference to the third element.
	/// @{
	constexpr inline element_type& z() noexcept
	{
		static_assert(N > 2, "Vector does not contain a z element.");
		return elements[2];
	}
	constexpr inline const element_type& z() const noexcept
	{
		static_assert(N > 2, "Vector does not contain a z element.");
		return elements[2];
	}
	/// @}
	
	/// Returns a pointer to the element array.
	/// @{
	constexpr inline element_type* data() noexcept
	{
		return elements;
	};
	constexpr inline const element_type* data() const noexcept
	{
		return elements;
	};
	/// @}
	
	/// Returns an iterator to the first element.
	/// @{
	constexpr inline element_type* begin() noexcept
	{
		return elements;
	}
	constexpr inline const element_type* begin() const noexcept
	{
		return elements;
	}
	constexpr inline const element_type* cbegin() const noexcept
	{
		return elements;
	}
	/// @}
	
	/// Returns an iterator to the element following the last element.
	/// @{
	constexpr inline element_type* end() noexcept
	{
		return elements + N;
	}
	constexpr inline const element_type* end() const noexcept
	{
		return elements + N;
	}
	constexpr inline const element_type* cend() const noexcept
	{
		return elements + N;
	}
	/// @}
	
	/// Returns a reverse iterator to the first element of the reversed vector.
	/// @{
	constexpr inline std::reverse_iterator<element_type*> rbegin() noexcept
	{
		return std::reverse_iterator<element_type*>(elements + N);
	}
	constexpr inline std::reverse_iterator<const element_type*> rbegin() const noexcept
	{
		return std::reverse_iterator<const element_type*>(elements + N);
	}
	constexpr inline std::reverse_iterator<const element_type*> crbegin() const noexcept
	{
		return std::reverse_iterator<const element_type*>(elements + N);
	}
	/// @}
	
	/// Returns a reverse iterator to the element following the last element of the reversed vector.
	/// @{
	constexpr inline std::reverse_iterator<element_type*> rend() noexcept
	{
		return std::reverse_iterator<element_type*>(elements);
	}
	constexpr inline std::reverse_iterator<const element_type*> rend() const noexcept
	{
		return std::reverse_iterator<const element_type*>(elements);
	}
	constexpr inline std::reverse_iterator<const element_type*> crend() const noexcept
	{
		return std::reverse_iterator<const element_type*>(elements);
	}
	/// @}
	
	/// Returns the number of elements in the vector.
	constexpr inline std::size_t size() const noexcept
	{
		return N;
	};
	
	/// @private
	template <class U, std::size_t... I>
	constexpr inline vector<U, N> type_cast(std::index_sequence<I...>) const noexcept
	{
		return {static_cast<U>(elements[I])...};
	}
	
	/**
	 * Type-casts the elements of this vector using `static_cast`.
	 *
	 * @tparam U Target element type.
	 *
	 * @return Vector containing the type-casted elements.
	 */
	template <class U>
	constexpr inline explicit operator vector<U, N>() const noexcept
	{
		return type_cast<U>(std::make_index_sequence<N>{});
	}
	
	/// @private
	template <std::size_t M, std::size_t... I>
	constexpr inline vector<T, M> size_cast(std::index_sequence<I...>) const noexcept
	{
		return {(I < N) ? elements[I] : T{0} ...};
	}
	
	/**
	 * Size-casts this vector to a vector with a different number of elements. Casting to a greater number of elements causes new elements to be set to zero.
	 *
	 * @tparam M Target number of elements.
	 *
	 * @return *m*-dimensional vector.
	 */
	template <std::size_t M>
	constexpr inline explicit operator vector<T, M>() const noexcept
	{
		return size_cast<M>(std::make_index_sequence<M>{});
	}
	
	/// Returns a zero vector, where every element is equal to zero.
	static constexpr vector zero() noexcept
	{
		return {};
	}
	
	/// @private
	template <std::size_t... I>
	static constexpr vector one(std::index_sequence<I...>) noexcept
	{
		//return {T{1}...};
		
		// MSVC bug workaround (I must be referenced for parameter pack expansion)
		return {(I ? T{1} : T{1})...};
	}
	
	/// Returns a vector of ones, where every element is equal to one.
	static constexpr vector one() noexcept
	{
		return one(std::make_index_sequence<N>{});
	}
};

/// Vector with two elements.
template <typename T>
using vector2 = vector<T, 2>;

/// Vector with three elements.
template <typename T>
using vector3 = vector<T, 3>;

/// Vector with four elements.
template <typename T>
using vector4 = vector<T, 4>;

/**
 * Returns the absolute values of each element.
 *
 * @param x Input vector
 * @return Absolute values of input vector elements.
 */
template <class T, std::size_t N>
constexpr vector<T, N> abs(const vector<T, N>& x);

/**
 * Adds two values.
 *
 * @param x First value.
 * @param y Second value.
 * @return Sum of the two values.
 */
/// @{
template <class T, std::size_t N>
constexpr vector<T, N> add(const vector<T, N>& x, const vector<T, N>& y) noexcept;
template <class T, std::size_t N>
constexpr vector<T, N> add(const vector<T, N>& x, T y) noexcept;
/// @}

/**
 * Checks if all elements of a boolean vector are `true`.
 *
 * @param x Vector to be tested for truth.
 * @return `true` if all elements are `true`, `false` otherwise.
 */
template <std::size_t N>
constexpr bool all(const vector<bool, N>& x) noexcept;

/**
 * Checks if any elements of a boolean vector are `true`.
 *
 * @param x Vector to be tested for truth.
 * @return `true` if any elements are `true`, `false` otherwise.
 */
template <std::size_t N>
constexpr bool any(const vector<bool, N>& x) noexcept;

/**
 * Performs a element-wise ceil operation.
 *
 * @param x Input vector
 * @return Component-wise ceil of input vector.
 */
template <class T, std::size_t N>
constexpr vector<T, N> ceil(const vector<T, N>& x);

/**
 * Clamps the values of a vector's elements.
 *
 * @param x Vector to clamp.
 * @param min Minimum value.
 * @param max Maximum value.
 * @return Clamped vector.
 */
/// @{
template <class T, std::size_t N>
constexpr vector<T, N> clamp(const vector<T, N>& x, const vector<T, N>& min, const vector<T, N>& max);
template <class T, std::size_t N>
constexpr vector<T, N> clamp(const vector<T, N>& x, T min, T max);
/// @}

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
constexpr vector<T, 3> cross(const vector<T, 3>& x, const vector<T, 3>& y) noexcept;

/**
 * Calculates the distance between two points.
 *
 * @param p0 First of two points.
 * @param p1 Second of two points.
 * @return Distance between the two points.
 */
template <class T, std::size_t N>
T distance(const vector<T, N>& p0, const vector<T, N>& p1);

/**
 * Calculates the squared distance between two points. The squared distance can be calculated faster than the distance because a call to `std::sqrt` is saved.
 *
 * @param p0 First of two points.
 * @param p1 Second of two points.
 * @return Squared distance between the two points.
 */
template <class T, std::size_t N>
constexpr T distance_squared(const vector<T, N>& p0, const vector<T, N>& p1) noexcept;

/**
 * Divides a vector by a value.
 *
 * @param x First value.
 * @param y Second value.
 * @return Result of the division.
 */
/// @{
template <class T, std::size_t N>
constexpr vector<T, N> div(const vector<T, N>& x, const vector<T, N>& y) noexcept;
template <class T, std::size_t N>
constexpr vector<T, N> div(const vector<T, N>& x, T y) noexcept;
template <class T, std::size_t N>
constexpr vector<T, N> div(T x, const vector<T, N>& y) noexcept;
/// @}

/**
 * Calculates the dot product of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Dot product of the two vectors.
 */
template <class T, std::size_t N>
constexpr T dot(const vector<T, N>& x, const vector<T, N>& y) noexcept;

/**
 * Compares two vectors for equality
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
constexpr vector<bool, N> equal(const vector<T, N>& x, const vector<T, N>& y) noexcept;

/**
 * Performs a element-wise floor operation.
 *
 * @param x Input vector
 * @return Component-wise floor of input vector.
 */
template <class T, std::size_t N>
constexpr vector<T, N> floor(const vector<T, N>& x);

/**
 * Performs a multiply-add operation.
 *
 * @param x Input vector
 * @param y Value to multiply.
 * @param z Value to add.
 *
 * @return Vector containing the multiply-add results.
 */
/// @{
template <class T, std::size_t N>
constexpr vector<T, N> fma(const vector<T, N>& x, const vector<T, N>& y, const vector<T, N>& z);
template <class T, std::size_t N>
constexpr vector<T, N> fma(const vector<T, N>& x, T y, T z);
/// @}

/**
 * Returns a vector containing the fractional part of each element.
 *
 * @param x Input vector
 * @return Fractional parts of input vector.
 */
template <class T, std::size_t N>
constexpr vector<T, N> fract(const vector<T, N>& x);

/**
 * Performs a element-wise greater-than comparison of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
constexpr vector<bool, N> greater_than(const vector<T, N>& x, const vector<T, N>& y) noexcept;

/**
 * Performs a element-wise greater-than or equal-to comparison of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
constexpr vector<bool, N> greater_than_equal(const vector<T, N>& x, const vector<T, N>& y) noexcept;

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
constexpr T length_squared(const vector<T, N>& x) noexcept;

/**
 * Performs a element-wise less-than comparison of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
constexpr vector<bool, N> less_than(const vector<T, N>& x, const vector<T, N>& y) noexcept;

/**
 * Performs a element-wise less-than or equal-to comparison of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
constexpr vector<bool, N> less_than_equal(const vector<T, N>& x, const vector<T, N>& y) noexcept;

/**
 * Returns a vector containing the maximum elements of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 *
 * @return Maximum elements of the two vectors.
 */
template <class T, std::size_t N>
constexpr vector<T, N> max(const vector<T, N>& x, const vector<T, N>& y);

/**
 * Returns the value of the greatest element in a vector.
 *
 * @param x Input vector.
 *
 * @return Value of the greatest element in the input vector.
 */
template <class T, std::size_t N>
constexpr T max(const vector<T, N>& x);

/**
 * Returns a vector containing the minimum elements of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 *
 * @return Minimum elements of the two vectors.
 */
template <class T, std::size_t N>
constexpr vector<T, N> min(const vector<T, N>& x, const vector<T, N>& y);

/**
 * Returns the value of the smallest element in a vector.
 *
 * @param x Input vector.
 *
 * @return Value of the smallest element in the input vector.
 */
template <class T, std::size_t N>
constexpr T min(const vector<T, N>& x);

/**
 * Calculates the element-wise remainder of the division operation `x / y`.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Remainders of `x / y`.
 */
/// @{
template <class T, std::size_t N>
constexpr vector<T, N> mod(const vector<T, N>& x, const vector<T, N>& y);
template <class T, std::size_t N>
constexpr vector<T, N> mod(const vector<T, N>& x, T y);
/// @}

/**
 * Multiplies two values.
 *
 * @param x First value.
 * @param y Second value.
 * @return Product of the two values.
 */
/// @{
template <class T, std::size_t N>
constexpr vector<T, N> mul(const vector<T, N>& x, const vector<T, N>& y) noexcept;
template <class T, std::size_t N>
constexpr vector<T, N> mul(const vector<T, N>& x, T y) noexcept;
/// @}

/**
 * Negates a vector.
 *
 * @param x Vector to negate.
 * @return Negated vector.
 */
template <class T, std::size_t N>
constexpr vector<T, N> negate(const vector<T, N>& x) noexcept;

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
constexpr vector<bool, N> not(const vector<T, N>& x) noexcept;

/**
 * Compares two vectors for inequality
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
constexpr vector<bool, N> not_equal(const vector<T, N>& x, const vector<T, N>& y) noexcept;

/**
 * Raises each element to a power.
 *
 * @param x Input vector
 * @param y Exponent.
 *
 * @return Vector with its elements raised to the given power.
 */
/// @{
template <class T, std::size_t N>
vector<T, N> pow(const vector<T, N>& x, const vector<T, N>& y);
template <class T, std::size_t N>
vector<T, N> pow(const vector<T, N>& x, T y);
/// @}

/**
 * Performs a element-wise round operation.
 *
 * @param x Input vector
 * @return Component-wise round of input vector.
 */
template <class T, std::size_t N>
constexpr vector<T, N> round(const vector<T, N>& x);

/**
 * Returns a vector containing the signs of each element.
 *
 * @param x Input vector
 * @return Signs of input vector elements.
 */
template <class T, std::size_t N>
constexpr vector<T, N> sign(const vector<T, N>& x);

/**
 * Takes the square root of each element.
 *
 * @param x Input vector
 *
 * @return Square roots of the input vector elements.
 */
template <class T, std::size_t N>
vector<T, N> sqrt(const vector<T, N>& x);

/**
 * Subtracts a value by another value.
 *
 * @param x First value.
 * @param y Second value.
 * @return Difference between the two values.
 */
/// @{
template <class T, std::size_t N>
constexpr vector<T, N> sub(const vector<T, N>& x, const vector<T, N>& y) noexcept;
template <class T, std::size_t N>
constexpr vector<T, N> sub(const vector<T, N>& x, T y) noexcept;
template <class T, std::size_t N>
constexpr vector<T, N> sub(T x, const vector<T, N>& y) noexcept;
/// @}

/**
 * Calculates the sum of all elements in a vector.
 *
 * @param x Vector to sum.
 * @return Sum of the vector's elements.
 */
template <class T, std::size_t N>
constexpr T sum(const vector<T, N>& x) noexcept;

/**
 * Makes an *m*-dimensional vector by rearranging and/or duplicating elements of an *n*-dimensional vector.
 *
 * @tparam Indices List of indices of elements in @p x.
 * @tparam T Vector element type.
 * @tparam N Number of dimensions in @p x.
 *
 * @param x Vector to swizzle.
 *
 * @return Vector containing elements from @p x in the order specified by @p Indices. The size of the returned vector is equivalent to the number of indices in @p Indices.
 */
template <std::size_t... Indices, class T, std::size_t N>
constexpr vector<T, sizeof...(Indices)> swizzle(const vector<T, N>& x) noexcept;

/**
 * Performs a element-wise trunc operation.
 *
 * @param x Input vector
 * @return Component-wise trunc of input vector.
 */
template <class T, std::size_t N>
constexpr vector<T, N> trunc(const vector<T, N>& x);

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> abs(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {std::abs(x[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> abs(const vector<T, N>& x)
{
	return abs(x, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> add(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] + y[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> add(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return add(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> add(const vector<T, N>& x, T y, std::index_sequence<I...>) noexcept
{
	return {(x[I] + y)...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> add(const vector<T, N>& x, T y) noexcept
{
	return add(x, y, std::make_index_sequence<N>{});
}

/// @private
template <std::size_t N, std::size_t... I>
constexpr inline bool all(const vector<bool, N>& x, std::index_sequence<I...>) noexcept
{
	return (x[I] && ...);
}

template <std::size_t N>
constexpr inline bool all(const vector<bool, N>& x) noexcept
{
	return all(x, std::make_index_sequence<N>{});
}

/// @private
template <std::size_t N, std::size_t... I>
constexpr inline bool any(const vector<bool, N>& x, std::index_sequence<I...>) noexcept
{
	return (x[I] || ...);
}

template <std::size_t N>
constexpr inline bool any(const vector<bool, N>& x) noexcept
{
	return any(x, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> ceil(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {std::ceil(x[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> ceil(const vector<T, N>& x)
{
	static_assert(std::is_floating_point<T>::value);
	return ceil(x, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> clamp(const vector<T, N>& x, const vector<T, N>& min_val, const vector<T, N>& max_val, std::index_sequence<I...>)
{
	return {std::min<T>(max_val[I], std::max<T>(min_val[I], x[I]))...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> clamp(const vector<T, N>& x, const vector<T, N>& min, const vector<T, N>& max)
{
	return clamp(x, min, max, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> clamp(const vector<T, N>& x, T min, T max, std::index_sequence<I...>)
{
	return {std::min<T>(max, std::max<T>(min, x[I]))...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> clamp(const vector<T, N>& x, T min, T max)
{
	return clamp(x, min, max, std::make_index_sequence<N>{});
}

template <class T, std::size_t N>
vector<T, N> clamp_length(const vector<T, N>& x, T max_length)
{
	static_assert(std::is_floating_point<T>::value);
	T length2 = length_squared(x);
	return (length2 > max_length * max_length) ? (x * max_length / std::sqrt(length2)) : x;
}

template <class T>
constexpr inline vector<T, 3> cross(const vector<T, 3>& x, const vector<T, 3>& y) noexcept
{
	return
	{
		x[1] * y[2] - y[1] * x[2],
		x[2] * y[0] - y[2] * x[0],
		x[0] * y[1] - y[0] * x[1]
	};
}

template <class T, std::size_t N>
inline T distance(const vector<T, N>& p0, const vector<T, N>& p1)
{
	static_assert(std::is_floating_point<T>::value);
	return length(sub(p0, p1));
}

template <class T, std::size_t N>
constexpr inline T distance_squared(const vector<T, N>& p0, const vector<T, N>& p1) noexcept
{
	return length_squared(sub(p0, p1));
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> div(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] / y[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> div(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return div(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> div(const vector<T, N>& x, T y, std::index_sequence<I...>) noexcept
{
	return {(x[I] / y)...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> div(const vector<T, N>& x, T y) noexcept
{
	return div(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> div(T x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x / y[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> div(T x, const vector<T, N>& y) noexcept
{
	return div(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline T dot(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return ((x[I] * y[I]) + ...);
}

template <class T, std::size_t N>
constexpr inline T dot(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return dot(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<bool, N> equal(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] == y[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<bool, N> equal(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return equal(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> floor(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {std::floor(x[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> floor(const vector<T, N>& x)
{
	static_assert(std::is_floating_point<T>::value);
	return floor(x, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> fma(const vector<T, N>& x, const vector<T, N>& y, const vector<T, N>& z, std::index_sequence<I...>)
{
	return {std::fma(x[I], y[I], z[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> fma(const vector<T, N>& x, const vector<T, N>& y, const vector<T, N>& z)
{
	static_assert(std::is_floating_point<T>::value);
	return fma(x, y, z, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> fma(const vector<T, N>& x, T y, T z, std::index_sequence<I...>)
{
	return {std::fma(x[I], y, z)...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> fma(const vector<T, N>& x, T y, T z)
{
	static_assert(std::is_floating_point<T>::value);
	return fma(x, y, z, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> fract(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {x[I] - std::floor(x[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> fract(const vector<T, N>& x)
{
	static_assert(std::is_floating_point<T>::value);
	return fract(x, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<bool, N> greater_than(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] > y[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<bool, N> greater_than(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return greater_than(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<bool, N> greater_than_equal(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] >= y[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<bool, N> greater_than_equal(const vector<T, N>& x, const vector<T, N>& y) noexcept
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
constexpr inline T length_squared(const vector<T, N>& x) noexcept
{
	return dot(x, x);
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<bool, N> less_than(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] < y[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<bool, N> less_than(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return less_than(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<bool, N> less_than_equal(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] <= y[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<bool, N> less_than_equal(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return less_than_equal(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> max(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {std::max<T>(x[I], y[I])...};
}

template <class T, std::size_t N>
constexpr vector<T, N> max(const vector<T, N>& x, const vector<T, N>& y)
{
	return max(x, y, std::make_index_sequence<N>{});
}

template <class T, std::size_t N>
constexpr inline T max(const vector<T, N>& x)
{
	return *std::max_element(x.elements, x.elements + N);
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> min(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {std::min<T>(x[I], y[I])...};
}

template <class T, std::size_t N>
constexpr vector<T, N> min(const vector<T, N>& x, const vector<T, N>& y)
{
	return min(x, y, std::make_index_sequence<N>{});
}

template <class T, std::size_t N>
constexpr inline T min(const vector<T, N>& x)
{
	return *std::min_element(x.elements, x.elements + N);
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> mod(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {std::fmod(x[I], y[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> mod(const vector<T, N>& x, const vector<T, N>& y)
{
	static_assert(std::is_floating_point<T>::value);
	return mod(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> mod(const vector<T, N>& x, T y, std::index_sequence<I...>)
{
	return {std::fmod(x[I], y)...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> mod(const vector<T, N>& x, T y)
{
	static_assert(std::is_floating_point<T>::value);
	return mod(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> mul(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] * y[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> mul(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return mul(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> mul(const vector<T, N>& x, T y, std::index_sequence<I...>) noexcept
{
	return {(x[I] * y)...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> mul(const vector<T, N>& x, T y) noexcept
{
	return mul(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> negate(const vector<T, N>& x, std::index_sequence<I...>) noexcept
{
	return {(-x[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> negate(const vector<T, N>& x) noexcept
{
	return negate(x, std::make_index_sequence<N>{});
}

template <class T, std::size_t N>
inline vector<T, N> normalize(const vector<T, N>& x)
{
	static_assert(std::is_floating_point<T>::value);
	return mul(x, T{1} / length(x));
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<bool, N> not(const vector<T, N>& x, std::index_sequence<I...>) noexcept
{
	return {!x[I]...};
}

template <class T, std::size_t N>
constexpr inline vector<bool, N> not(const vector<T, N>& x) noexcept
{
	return not(x, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<bool, N> not_equal(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] != y[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<bool, N> not_equal(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return not_equal(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<T, N> pow(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {std::pow(x[I], y[I])...};
}

template <class T, std::size_t N>
inline vector<T, N> pow(const vector<T, N>& x, const vector<T, N>& y)
{
	static_assert(std::is_floating_point<T>::value);
	return pow(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<T, N> pow(const vector<T, N>& x, T y, std::index_sequence<I...>)
{
	return {std::pow(x[I], y)...};
}

template <class T, std::size_t N>
inline vector<T, N> pow(const vector<T, N>& x, T y)
{
	static_assert(std::is_floating_point<T>::value);
	return pow(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> round(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {std::round(x[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> round(const vector<T, N>& x)
{
	static_assert(std::is_floating_point<T>::value);
	return round(x, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> sign(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {std::copysign(T{1}, x[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> sign(const vector<T, N>& x)
{
	static_assert(std::is_floating_point<T>::value);
	return sign(x, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline vector<T, N> sqrt(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {std::sqrt(x[I])...};
}

template <class T, std::size_t N>
inline vector<T, N> sqrt(const vector<T, N>& x, const vector<T, N>& y)
{
	static_assert(std::is_floating_point<T>::value);
	return pow(x, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> sub(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] - y[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> sub(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return sub(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> sub(const vector<T, N>& x, T y, std::index_sequence<I...>) noexcept
{
	return {(x[I] - y)...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> sub(const vector<T, N>& x, T y) noexcept
{
	return sub(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> sub(T x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x - y[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> sub(T x, const vector<T, N>& y) noexcept
{
	return sub(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline T sum(const vector<T, N>& x, std::index_sequence<I...>) noexcept
{
	return (x[I] + ...);
}

template <class T, std::size_t N>
constexpr inline T sum(const vector<T, N>& x) noexcept
{
	return sum(x, std::make_index_sequence<N>{});
}

template <std::size_t... Indices, class T, std::size_t N>
constexpr inline vector<T, sizeof...(Indices)> swizzle(const vector<T, N>& x) noexcept
{
	return {x[Indices]...};
}

/// @private
template <class T, std::size_t N, std::size_t... I>
constexpr inline vector<T, N> trunc(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {std::trunc(x[I])...};
}

template <class T, std::size_t N>
constexpr inline vector<T, N> trunc(const vector<T, N>& x)
{
	static_assert(std::is_floating_point<T>::value);
	return trunc(x, std::make_index_sequence<N>{});
}

} // namespace math

/// @copydoc math::add(const math::vector<T, N>&, const math::vector<T, N>&)
template <class T, std::size_t N>
constexpr inline math::vector<T, N> operator+(const math::vector<T, N>& x, const math::vector<T, N>& y) noexcept
{
	return math::add(x, y);
}

/// @copydoc math::add(const math::vector<T, N>&, T)
/// @{
template <class T, std::size_t N>
constexpr inline math::vector<T, N> operator+(const math::vector<T, N>& x, T y) noexcept
{
	return math::add(x, y);
}
template <class T, std::size_t N>
constexpr inline math::vector<T, N> operator+(T x, const math::vector<T, N>& y) noexcept
{
	return math::add(y, x);
}
/// @}

/// @copydoc math::div(const math::vector<T, N>&, const math::vector<T, N>&)
template <class T, std::size_t N>
constexpr inline math::vector<T, N> operator/(const math::vector<T, N>& x, const math::vector<T, N>& y) noexcept
{
	return math::div(x, y);
}

/// @copydoc math::div(const math::vector<T, N>&, T)
template <class T, std::size_t N>
constexpr inline math::vector<T, N> operator/(const math::vector<T, N>& x, T y) noexcept
{
	return math::div(x, y);
}

/// @copydoc math::div(T, const math::vector<T, N>&)
template <class T, std::size_t N>
constexpr inline math::vector<T, N> operator/(T x, const math::vector<T, N>& y) noexcept
{
	return math::div(x, y);
}

/// @copydoc math::mul(const math::vector<T, N>&, const math::vector<T, N>&)
template <class T, std::size_t N>
constexpr inline math::vector<T, N> operator*(const math::vector<T, N>& x, const math::vector<T, N>& y) noexcept
{
	return math::mul(x, y);
}

/// @copydoc math::mul(const math::vector<T, N>&, T)
/// @{
template <class T, std::size_t N>
constexpr inline math::vector<T, N> operator*(const math::vector<T, N>& x, T y) noexcept
{
	return math::mul(x, y);
}
template <class T, std::size_t N>
constexpr inline math::vector<T, N> operator*(T x, const math::vector<T, N>& y) noexcept
{
	return math::mul(y, x);
}
/// @}

/// @copydoc math::negate(const math::vector<T, N>&)
template <class T, std::size_t N>
constexpr inline math::vector<T, N> operator-(const math::vector<T, N>& x) noexcept
{
	return math::negate(x);
}

/// @copydoc math::sub(const math::vector<T, N>&, const math::vector<T, N>&)
template <class T, std::size_t N>
constexpr inline math::vector<T, N> operator-(const math::vector<T, N>& x, const math::vector<T, N>& y) noexcept
{
	return math::sub(x, y);
}

/// @copydoc math::sub(const math::vector<T, N>&, T)
template <class T, std::size_t N>
constexpr inline math::vector<T, N> operator-(const math::vector<T, N>& x, T y) noexcept
{
	return math::sub(x, y);
}

/// @copydoc math::sub(T, const math::vector<T, N>&)
template <class T, std::size_t N>
constexpr inline math::vector<T, N> operator-(T x, const math::vector<T, N>& y) noexcept
{
	return math::sub(x, y);
}

/**
 * Adds two values and stores the result in the first value.
 *
 * @param x First value.
 * @param y Second value.
 * @return Reference to the first value.
 */
/// @{
template <class T, std::size_t N>
constexpr inline math::vector<T, N>& operator+=(math::vector<T, N>& x, const math::vector<T, N>& y) noexcept
{
	return (x = x + y);
}
template <class T, std::size_t N>
constexpr inline math::vector<T, N>& operator+=(math::vector<T, N>& x, T y) noexcept
{
	return (x = x + y);
}
/// @}

/**
 * Subtracts the first value by the second value and stores the result in the first value.
 *
 * @param x First value.
 * @param y Second value.
 * @return Reference to the first value.
 */
/// @{
template <class T, std::size_t N>
constexpr inline math::vector<T, N>& operator-=(math::vector<T, N>& x, const math::vector<T, N>& y) noexcept
{
	return (x = x - y);
}
template <class T, std::size_t N>
constexpr inline math::vector<T, N>& operator-=(math::vector<T, N>& x, T y) noexcept
{
	return (x = x - y);
}
/// @}

/**
 * Multiplies two values and stores the result in the first value.
 *
 * @param x First value.
 * @param y Second value.
 * @return Reference to the first value.
 */
/// @{
template <class T, std::size_t N>
constexpr inline math::vector<T, N>& operator*=(math::vector<T, N>& x, const math::vector<T, N>& y) noexcept
{
	return (x = x * y);
}
template <class T, std::size_t N>
constexpr inline math::vector<T, N>& operator*=(math::vector<T, N>& x, T y) noexcept
{
	return (x = x * y);
}
/// @}

/**
 * Divides the first value by the second value and stores the result in the first value.
 *
 * @param x First value.
 * @param y Second value.
 * @return Reference to the first value.
 */
/// @{
template <class T, std::size_t N>
constexpr inline math::vector<T, N>& operator/=(math::vector<T, N>& x, const math::vector<T, N>& y) noexcept
{
	return (x = x / y);
}
template <class T, std::size_t N>
constexpr inline math::vector<T, N>& operator/=(math::vector<T, N>& x, T y) noexcept
{
	return (x = x / y);
}
/// @}

/**
 * Writes the elements of a vector to an output stream, with each element delimeted by a space.
 *
 * @param os Output stream.
 * @param x Vector.
 *
 * @return Output stream.
 */
template <class T, std::size_t N>
std::ostream& operator<<(std::ostream& os, const math::vector<T, N>& x)
{
	for (std::size_t i = 0; i < N; ++i)
	{
		if (i)
			os << ' ';
		os << x[i];
	}
	
	return os;
}

/**
 * Reads the elements of a vector from an input stream, with each element delimeted by a space.
 *
 * @param is Input stream.
 * @param x Vector.
 *
 * @return Input stream.
 */
template <class T, std::size_t N>
std::istream& operator>>(std::istream& is, math::vector<T, N>& x)
{
	for (std::size_t i = 0; i < N; ++i)
		is >> x[i];
	
	return is;
}

#endif // ANTKEEPER_MATH_VECTOR_HPP
