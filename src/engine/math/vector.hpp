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

#ifndef ANTKEEPER_MATH_VECTOR_HPP
#define ANTKEEPER_MATH_VECTOR_HPP

#include <algorithm>
#include <cstddef>
#include <cmath>
#include <concepts>
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
	/// Element type.
	typedef T element_type;
	
	/// Number of elements.
	static constexpr std::size_t element_count = N;
	
	/// Array of elements.
	element_type elements[N];
	
	/// @name Conversion
	/// @{
	
	/// @private
	template <class U, std::size_t... I>
	[[nodiscard]] inline constexpr vector<U, N> type_cast(std::index_sequence<I...>) const noexcept
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
	[[nodiscard]] inline constexpr explicit operator vector<U, N>() const noexcept
	{
		return type_cast<U>(std::make_index_sequence<N>{});
	}
	
	/// @private
	template <std::size_t M, std::size_t... I>
	[[nodiscard]] inline constexpr vector<T, M> size_cast(std::index_sequence<I...>) const noexcept
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
	[[nodiscard]] inline constexpr explicit operator vector<T, M>() const noexcept
	{
		return size_cast<M>(std::make_index_sequence<M>{});
	}
	
	/// @}
	
	/// @name Element access
	/// @{
	
	/**
	 * Returns a reference to the element at a given index.
	 *
	 * @param i Index of an element.
	 *
	 * @return Reference to the element at index @p i.
	 */
	/// @{
	[[nodiscard]] inline constexpr element_type& operator[](std::size_t i) noexcept
	{
		return elements[i];
	}
	[[nodiscard]] inline constexpr const element_type& operator[](std::size_t i) const noexcept
	{
		return elements[i];
	}
	/// @}
	
	/**
	 * Returns a reference to the first element.
	 */
	/// @{
	[[nodiscard]] inline constexpr element_type& front() noexcept
	{
		return elements[0];
	}
	[[nodiscard]] inline constexpr const element_type& front() const noexcept
	{
		return elements[0];
	}
	/// @}
	
	/**
	 * Returns a reference to the last element.
	 */
	/// @{
	[[nodiscard]] inline constexpr element_type& back() noexcept
	{
		return elements[N - 1];
	}
	[[nodiscard]] inline constexpr const element_type& back() const noexcept
	{
		return elements[N - 1];
	}
	/// @}
	
	/**
	 * Returns a pointer to the element array.
	 */
	/// @{
	[[nodiscard]] inline constexpr element_type* data() noexcept
	{
		return elements;
	};
	[[nodiscard]] inline constexpr const element_type* data() const noexcept
	{
		return elements;
	};
	/// @}
	
	/// Returns a reference to the first element.
	/// @{
	[[nodiscard]] inline constexpr element_type& x() noexcept
	{
		static_assert(N > 0, "Vector does not contain an x element.");
		return elements[0];
	}
	[[nodiscard]] inline constexpr const element_type& x() const noexcept
	{
		static_assert(N > 0, "Vector does not contain an x element.");
		return elements[0];
	}
	/// @}
	
	/**
	 * Returns a reference to the second element.
	 */
	/// @{
	[[nodiscard]] inline constexpr element_type& y() noexcept
	{
		static_assert(N > 1, "Vector does not contain a y element.");
		return elements[1];
	}
	[[nodiscard]] inline constexpr const element_type& y() const noexcept
	{
		static_assert(N > 1, "Vector does not contain a y element.");
		return elements[1];
	}
	/// @}
	
	/**
	 * Returns a reference to the third element.
	 */
	/// @{
	[[nodiscard]] inline constexpr element_type& z() noexcept
	{
		static_assert(N > 2, "Vector does not contain a z element.");
		return elements[2];
	}
	[[nodiscard]] inline constexpr const element_type& z() const noexcept
	{
		static_assert(N > 2, "Vector does not contain a z element.");
		return elements[2];
	}
	/// @}
	
	/// @}
	
	/// @name Iterators
	/// @{
	
	/**
	 * Returns an iterator to the first element.
	 */
	/// @{
	[[nodiscard]] inline constexpr element_type* begin() noexcept
	{
		return elements;
	}
	[[nodiscard]] inline constexpr const element_type* begin() const noexcept
	{
		return elements;
	}
	[[nodiscard]] inline constexpr const element_type* cbegin() const noexcept
	{
		return elements;
	}
	/// @}
	
	/**
	 * Returns an iterator to the element following the last element.
	 */
	/// @{
	[[nodiscard]] inline constexpr element_type* end() noexcept
	{
		return elements + N;
	}
	[[nodiscard]] inline constexpr const element_type* end() const noexcept
	{
		return elements + N;
	}
	[[nodiscard]] inline constexpr const element_type* cend() const noexcept
	{
		return elements + N;
	}
	/// @}
	
	/**
	 * Returns a reverse iterator to the first element of the reversed vector.
	 */
	/// @{
	[[nodiscard]] inline constexpr std::reverse_iterator<element_type*> rbegin() noexcept
	{
		return std::reverse_iterator<element_type*>(elements + N);
	}
	[[nodiscard]] inline constexpr std::reverse_iterator<const element_type*> rbegin() const noexcept
	{
		return std::reverse_iterator<const element_type*>(elements + N);
	}
	[[nodiscard]] inline constexpr std::reverse_iterator<const element_type*> crbegin() const noexcept
	{
		return std::reverse_iterator<const element_type*>(elements + N);
	}
	/// @}
	
	/**
	 * Returns a reverse iterator to the element following the last element of the reversed vector.
	 */
	/// @{
	[[nodiscard]] inline constexpr std::reverse_iterator<element_type*> rend() noexcept
	{
		return std::reverse_iterator<element_type*>(elements);
	}
	[[nodiscard]] inline constexpr std::reverse_iterator<const element_type*> rend() const noexcept
	{
		return std::reverse_iterator<const element_type*>(elements);
	}
	[[nodiscard]] inline constexpr std::reverse_iterator<const element_type*> crend() const noexcept
	{
		return std::reverse_iterator<const element_type*>(elements);
	}
	/// @}
	
	/// @}
	
	/// @name Capacity
	/// @{
	
	/**
	 * Returns the number of elements in the vector.
	 */
	[[nodiscard]] inline constexpr std::size_t size() const noexcept
	{
		return N;
	};
	
	/// @}
	
	/// @name Constants
	/// @{
	
	/**
	 * Returns a zero vector, where every element is equal to zero.
	 */
	[[nodiscard]] static constexpr vector zero() noexcept
	{
		return {};
	}
	
	/// @private
	template <std::size_t... I>
	[[nodiscard]] static constexpr vector one(std::index_sequence<I...>) noexcept
	{
		//return {T{1}...};
		
		// MSVC bug workaround (I must be referenced for parameter pack expansion)
		return {(I ? T{1} : T{1})...};
	}
	
	/**
	 * Returns a vector of ones, where every element is equal to one.
	 */
	[[nodiscard]] static constexpr vector one() noexcept
	{
		return one(std::make_index_sequence<N>{});
	}
	
	/// @}
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
 * @param x Input vector.
 *
 * @return Absolute values of input vector elements.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> abs(const vector<T, N>& x);

/**
 * Adds two values.
 *
 * @param x First value.
 * @param y Second value.
 *
 * @return Sum of the two values.
 */
/// @{
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> add(const vector<T, N>& x, const vector<T, N>& y) noexcept;
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> add(const vector<T, N>& x, T y) noexcept;
/// @}

/**
 * Checks if all elements of a boolean vector are `true`.
 *
 * @param x Vector to be tested for truth.
 *
 * @return `true` if all elements are `true`, `false` otherwise.
 */
template <std::size_t N>
[[nodiscard]] constexpr bool all(const vector<bool, N>& x) noexcept;

/**
 * Checks if any elements of a boolean vector are `true`.
 *
 * @param x Vector to be tested for truth.
 *
 * @return `true` if any elements are `true`, `false` otherwise.
 */
template <std::size_t N>
[[nodiscard]] constexpr bool any(const vector<bool, N>& x) noexcept;

/**
 * Performs a element-wise ceil operation.
 *
 * @param x Input vector.
 *
 * @return Component-wise ceil of input vector.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> ceil(const vector<T, N>& x);

/**
 * Clamps the values of a vector's elements.
 *
 * @param x Vector to clamp.
 * @param min Minimum value.
 * @param max Maximum value.
 *
 * @return Clamped vector.
 */
/// @{
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> clamp(const vector<T, N>& x, const vector<T, N>& min, const vector<T, N>& max);
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> clamp(const vector<T, N>& x, T min, T max);
/// @}

/**
 * Clamps the length of a vector.
 *
 * @param x Vector to clamp.
 * @param max_length Maximum length.
 *
 * @return Length-clamped vector.
 */
template <class T, std::size_t N>
[[nodiscard]] vector<T, N> clamp_length(const vector<T, N>& x, T max_length);

/**
 * Calculate the cross product of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 *
 * @return Cross product of the two vectors.
 */
template <class T>
[[nodiscard]] constexpr vector<T, 3> cross(const vector<T, 3>& x, const vector<T, 3>& y) noexcept;

/**
 * Calculates the distance between two points.
 *
 * @param p0 First of two points.
 * @param p1 Second of two points.
 *
 * @return Distance between the two points.
 */
template <class T, std::size_t N>
[[nodiscard]] T distance(const vector<T, N>& p0, const vector<T, N>& p1);

/**
 * Divides a vector by a value.
 *
 * @param x First value.
 * @param y Second value.
 *
 * @return Result of the division.
 */
/// @{
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> div(const vector<T, N>& x, const vector<T, N>& y) noexcept;
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> div(const vector<T, N>& x, T y) noexcept;
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> div(T x, const vector<T, N>& y) noexcept;
/// @}

/**
 * Calculates the dot product of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 *
 * @return Dot product of the two vectors.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr T dot(const vector<T, N>& x, const vector<T, N>& y) noexcept;

/**
 * Compares two vectors for equality
 *
 * @param x First vector.
 * @param y Second vector.
 *
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<bool, N> equal(const vector<T, N>& x, const vector<T, N>& y) noexcept;

/**
 * Performs a element-wise floor operation.
 *
 * @param x Input vector.
 *
 * @return Component-wise floor of input vector.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> floor(const vector<T, N>& x);

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
[[nodiscard]] constexpr vector<T, N> fma(const vector<T, N>& x, const vector<T, N>& y, const vector<T, N>& z);
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> fma(const vector<T, N>& x, T y, T z);
/// @}

/**
 * Returns a vector containing the fractional part of each element.
 *
 * @param x Input vector.
 *
 * @return Fractional parts of input vector.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> fract(const vector<T, N>& x);

/**
 * Extracts the Ith element from a vector.
 *
 * @tparam I Index of an element.
 * @tparam T Element type.
 * @tparam N Number of elements.
 *
 * @param v Vector from which to extract an element.
 *
 * @return Reference to the Ith element of @p v.
 */
/// @{
template<std::size_t I, class T, std::size_t N>
[[nodiscard]] constexpr T& get(math::vector<T, N>& v) noexcept;
template<std::size_t I, class T, std::size_t N>
[[nodiscard]] constexpr T&& get(math::vector<T, N>&& v) noexcept;
template<std::size_t I, class T, std::size_t N>
[[nodiscard]] constexpr const T& get(const math::vector<T, N>& v) noexcept;
template<std::size_t I, class T, std::size_t N>
[[nodiscard]] constexpr const T&& get(const math::vector<T, N>&& v) noexcept;
/// @}

/**
 * Performs a element-wise greater-than comparison of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 *
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<bool, N> greater_than(const vector<T, N>& x, const vector<T, N>& y) noexcept;

/**
 * Performs a element-wise greater-than or equal-to comparison of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 *
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<bool, N> greater_than_equal(const vector<T, N>& x, const vector<T, N>& y) noexcept;

/**
 * Calculates the inverse length of a vector.
 *
 * @param x Vector of which to calculate the inverse length.
 *
 * @return Inverse length of the vector.
 */
template <class T, std::size_t N>
[[nodiscard]] T inv_length(const vector<T, N>& x);

/**
 * Calculates the length of a vector.
 *
 * @param x Vector of which to calculate the length.
 *
 * @return Length of the vector.
 */
template <class T, std::size_t N>
[[nodiscard]] T length(const vector<T, N>& x);

/**
 * Performs a element-wise less-than comparison of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 *
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<bool, N> less_than(const vector<T, N>& x, const vector<T, N>& y) noexcept;

/**
 * Performs a element-wise less-than or equal-to comparison of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 *
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<bool, N> less_than_equal(const vector<T, N>& x, const vector<T, N>& y) noexcept;

/**
 * Returns a vector containing the maximum elements of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 *
 * @return Maximum elements of the two vectors.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> max(const vector<T, N>& x, const vector<T, N>& y);

/**
 * Returns the value of the greatest element in a vector.
 *
 * @param x Input vector.
 *
 * @return Value of the greatest element in the input vector.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr T max(const vector<T, N>& x);

/**
 * Returns a vector containing the minimum elements of two vectors.
 *
 * @param x First vector.
 * @param y Second vector.
 *
 * @return Minimum elements of the two vectors.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> min(const vector<T, N>& x, const vector<T, N>& y);

/**
 * Returns the value of the smallest element in a vector.
 *
 * @param x Input vector.
 *
 * @return Value of the smallest element in the input vector.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr T min(const vector<T, N>& x);

/**
 * Calculates the element-wise remainder of the division operation `x / y`.
 *
 * @param x First vector.
 * @param y Second vector.
 *
 * @return Remainders of `x / y`.
 */
/// @{
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> mod(const vector<T, N>& x, const vector<T, N>& y);
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> mod(const vector<T, N>& x, T y);
/// @}

/**
 * Multiplies two values.
 *
 * @param x First value.
 * @param y Second value.
 *
 * @return Product of the two values.
 */
/// @{
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> mul(const vector<T, N>& x, const vector<T, N>& y) noexcept;
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> mul(const vector<T, N>& x, T y) noexcept;
/// @}

/**
 * Negates a vector.
 *
 * @param x Vector to negate.
 *
 * @return Negated vector.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> negate(const vector<T, N>& x) noexcept;

/**
 * Calculates the unit vector in the same direction as the original vector.
 *
 * @param x Vector to normalize.
 *
 * @return Normalized vector.
 */
template <class T, std::size_t N>
[[nodiscard]] vector<T, N> normalize(const vector<T, N>& x);

/**
 * Logically inverts a boolean vector.
 *
 * @param x Vector to be inverted.
 *
 * @return Logically inverted vector.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<bool, N> logical_not(const vector<T, N>& x) noexcept;

/**
 * Compares two vectors for inequality
 *
 * @param x First vector.
 * @param y Second vector.
 *
 * @return Boolean vector containing the result of the element comparisons.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<bool, N> not_equal(const vector<T, N>& x, const vector<T, N>& y) noexcept;

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
[[nodiscard]] vector<T, N> pow(const vector<T, N>& x, const vector<T, N>& y);
template <class T, std::size_t N>
[[nodiscard]] vector<T, N> pow(const vector<T, N>& x, T y);
/// @}

/**
 * Performs a element-wise round operation.
 *
 * @param x Input vector
 *
 * @return Component-wise round of input vector.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> round(const vector<T, N>& x);

/**
 * Returns a vector containing the signs of each element.
 *
 * @param x Input vector
 * @return Signs of input vector elements.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> sign(const vector<T, N>& x);

/**
 * Calculates the square distance between two points. The square distance can be calculated faster than the distance because a call to `std::sqrt` is saved.
 *
 * @param p0 First of two points.
 * @param p1 Second of two points.
 *
 * @return Square distance between the two points.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr T sqr_distance(const vector<T, N>& p0, const vector<T, N>& p1) noexcept;

/**
 * Calculates the square length of a vector. The square length can be calculated faster than the length because a call to `std::sqrt` is saved.
 *
 * @param x Vector of which to calculate the square length.
 *
 * @return Square length of the vector.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr T sqr_length(const vector<T, N>& x) noexcept;

/**
 * Takes the square root of each element.
 *
 * @param x Input vector
 *
 * @return Square roots of the input vector elements.
 */
template <class T, std::size_t N>
[[nodiscard]] vector<T, N> sqrt(const vector<T, N>& x);

/**
 * Subtracts a value by another value.
 *
 * @param x First value.
 * @param y Second value.
 *
 * @return Difference between the two values.
 */
/// @{
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> sub(const vector<T, N>& x, const vector<T, N>& y) noexcept;
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> sub(const vector<T, N>& x, T y) noexcept;
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> sub(T x, const vector<T, N>& y) noexcept;
/// @}

/**
 * Calculates the sum of all elements in a vector.
 *
 * @param x Vector to sum.
 * @return Sum of the vector's elements.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr T sum(const vector<T, N>& x) noexcept;

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
[[nodiscard]] constexpr vector<T, sizeof...(Indices)> swizzle(const vector<T, N>& x) noexcept;

/**
 * Performs a element-wise trunc operation.
 *
 * @param x Input vector
 * @return Component-wise trunc of input vector.
 */
template <class T, std::size_t N>
[[nodiscard]] constexpr vector<T, N> trunc(const vector<T, N>& x);

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> abs(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {std::abs(x[I])...};
}

template <class T, std::size_t N>
inline constexpr vector<T, N> abs(const vector<T, N>& x)
{
	return abs(x, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> add(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] + y[I])...};
}

template <class T, std::size_t N>
inline constexpr vector<T, N> add(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return add(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> add(const vector<T, N>& x, T y, std::index_sequence<I...>) noexcept
{
	return {(x[I] + y)...};
}

template <class T, std::size_t N>
inline constexpr vector<T, N> add(const vector<T, N>& x, T y) noexcept
{
	return add(x, y, std::make_index_sequence<N>{});
}

/// @private
template <std::size_t N, std::size_t... I>
inline constexpr bool all(const vector<bool, N>& x, std::index_sequence<I...>) noexcept
{
	return (x[I] && ...);
}

template <std::size_t N>
inline constexpr bool all(const vector<bool, N>& x) noexcept
{
	return all(x, std::make_index_sequence<N>{});
}

/// @private
template <std::size_t N, std::size_t... I>
inline constexpr bool any(const vector<bool, N>& x, std::index_sequence<I...>) noexcept
{
	return (x[I] || ...);
}

template <std::size_t N>
inline constexpr bool any(const vector<bool, N>& x) noexcept
{
	return any(x, std::make_index_sequence<N>{});
}

/// @private
template <std::floating_point T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> ceil(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {std::ceil(x[I])...};
}

template <std::floating_point T, std::size_t N>
inline constexpr vector<T, N> ceil(const vector<T, N>& x)
{
	return ceil(x, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> clamp(const vector<T, N>& x, const vector<T, N>& min_val, const vector<T, N>& max_val, std::index_sequence<I...>)
{
	return {std::min<T>(max_val[I], std::max<T>(min_val[I], x[I]))...};
}

template <class T, std::size_t N>
inline constexpr vector<T, N> clamp(const vector<T, N>& x, const vector<T, N>& min, const vector<T, N>& max)
{
	return clamp(x, min, max, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> clamp(const vector<T, N>& x, T min, T max, std::index_sequence<I...>)
{
	return {std::min<T>(max, std::max<T>(min, x[I]))...};
}

template <class T, std::size_t N>
inline constexpr vector<T, N> clamp(const vector<T, N>& x, T min, T max)
{
	return clamp(x, min, max, std::make_index_sequence<N>{});
}

template <std::floating_point T, std::size_t N>
vector<T, N> clamp_length(const vector<T, N>& x, T max_length)
{
	T length2 = sqr_length(x);
	return (length2 > max_length * max_length) ? (x * (max_length / std::sqrt(length2))) : x;
}

template <class T>
inline constexpr vector<T, 3> cross(const vector<T, 3>& x, const vector<T, 3>& y) noexcept
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
	return length(sub(p0, p1));
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> div(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] / y[I])...};
}

template <class T, std::size_t N>
inline constexpr vector<T, N> div(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return div(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> div(const vector<T, N>& x, T y, std::index_sequence<I...>) noexcept
{
	return {(x[I] / y)...};
}

template <class T, std::size_t N>
inline constexpr vector<T, N> div(const vector<T, N>& x, T y) noexcept
{
	return div(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> div(T x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x / y[I])...};
}

template <class T, std::size_t N>
inline constexpr vector<T, N> div(T x, const vector<T, N>& y) noexcept
{
	return div(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr T dot(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return ((x[I] * y[I]) + ...);
}

template <class T, std::size_t N>
inline constexpr T dot(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return dot(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<bool, N> equal(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] == y[I])...};
}

template <class T, std::size_t N>
inline constexpr vector<bool, N> equal(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return equal(x, y, std::make_index_sequence<N>{});
}

/// @private
template <std::floating_point T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> floor(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {std::floor(x[I])...};
}

template <std::floating_point T, std::size_t N>
inline constexpr vector<T, N> floor(const vector<T, N>& x)
{
	return floor(x, std::make_index_sequence<N>{});
}

/// @private
template <std::floating_point T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> fma(const vector<T, N>& x, const vector<T, N>& y, const vector<T, N>& z, std::index_sequence<I...>)
{
	return {std::fma(x[I], y[I], z[I])...};
}

template <std::floating_point T, std::size_t N>
inline constexpr vector<T, N> fma(const vector<T, N>& x, const vector<T, N>& y, const vector<T, N>& z)
{
	return fma(x, y, z, std::make_index_sequence<N>{});
}

/// @private
template <std::floating_point T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> fma(const vector<T, N>& x, T y, T z, std::index_sequence<I...>)
{
	return {std::fma(x[I], y, z)...};
}

template <std::floating_point T, std::size_t N>
inline constexpr vector<T, N> fma(const vector<T, N>& x, T y, T z)
{
	return fma(x, y, z, std::make_index_sequence<N>{});
}

/// @private
template <std::floating_point T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> fract(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {x[I] - std::floor(x[I])...};
}

template <std::floating_point T, std::size_t N>
inline constexpr vector<T, N> fract(const vector<T, N>& x)
{
	return fract(x, std::make_index_sequence<N>{});
}

template<std::size_t I, class T, std::size_t N>
inline constexpr T& get(math::vector<T, N>& v) noexcept
{
	static_assert(I < N);
	return v.elements[I];
}

template<std::size_t I, class T, std::size_t N>
inline constexpr T&& get(math::vector<T, N>&& v) noexcept
{
	static_assert(I < N);
	return std::move(v.elements[I]);
}

template<std::size_t I, class T, std::size_t N>
inline constexpr const T& get(const math::vector<T, N>& v) noexcept
{
	static_assert(I < N);
	return v.elements[I];
}

template<std::size_t I, class T, std::size_t N>
inline constexpr const T&& get(const math::vector<T, N>&& v) noexcept
{
	static_assert(I < N);
	return std::move(v.elements[I]);
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<bool, N> greater_than(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] > y[I])...};
}

template <class T, std::size_t N>
inline constexpr vector<bool, N> greater_than(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return greater_than(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<bool, N> greater_than_equal(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] >= y[I])...};
}

template <class T, std::size_t N>
inline constexpr vector<bool, N> greater_than_equal(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return greater_than_equal(x, y, std::make_index_sequence<N>{});
}

template <std::floating_point T, std::size_t N>
inline T inv_length(const vector<T, N>& x)
{
	return T{1} / length(x);
}

template <std::floating_point T, std::size_t N>
inline T length(const vector<T, N>& x)
{
	return std::sqrt(sqr_length(x));
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<bool, N> less_than(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] < y[I])...};
}

template <class T, std::size_t N>
inline constexpr vector<bool, N> less_than(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return less_than(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<bool, N> less_than_equal(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] <= y[I])...};
}

template <class T, std::size_t N>
inline constexpr vector<bool, N> less_than_equal(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return less_than_equal(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> max(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {std::max<T>(x[I], y[I])...};
}

template <class T, std::size_t N>
constexpr vector<T, N> max(const vector<T, N>& x, const vector<T, N>& y)
{
	return max(x, y, std::make_index_sequence<N>{});
}

template <class T, std::size_t N>
inline constexpr T max(const vector<T, N>& x)
{
	return *std::max_element(x.elements, x.elements + N);
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> min(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {std::min<T>(x[I], y[I])...};
}

template <class T, std::size_t N>
constexpr vector<T, N> min(const vector<T, N>& x, const vector<T, N>& y)
{
	return min(x, y, std::make_index_sequence<N>{});
}

template <class T, std::size_t N>
inline constexpr T min(const vector<T, N>& x)
{
	return *std::min_element(x.elements, x.elements + N);
}

/// @private
template <std::floating_point T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> mod(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {std::fmod(x[I], y[I])...};
}

template <std::floating_point T, std::size_t N>
inline constexpr vector<T, N> mod(const vector<T, N>& x, const vector<T, N>& y)
{
	return mod(x, y, std::make_index_sequence<N>{});
}

/// @private
template <std::floating_point T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> mod(const vector<T, N>& x, T y, std::index_sequence<I...>)
{
	return {std::fmod(x[I], y)...};
}

template <std::floating_point T, std::size_t N>
inline constexpr vector<T, N> mod(const vector<T, N>& x, T y)
{
	return mod(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> mul(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] * y[I])...};
}

template <class T, std::size_t N>
inline constexpr vector<T, N> mul(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return mul(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> mul(const vector<T, N>& x, T y, std::index_sequence<I...>) noexcept
{
	return {(x[I] * y)...};
}

template <class T, std::size_t N>
inline constexpr vector<T, N> mul(const vector<T, N>& x, T y) noexcept
{
	return mul(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> negate(const vector<T, N>& x, std::index_sequence<I...>) noexcept
{
	return {(-x[I])...};
}

template <class T, std::size_t N>
inline constexpr vector<T, N> negate(const vector<T, N>& x) noexcept
{
	return negate(x, std::make_index_sequence<N>{});
}

template <std::floating_point T, std::size_t N>
inline vector<T, N> normalize(const vector<T, N>& x)
{
	return mul(x, inv_length(x));
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<bool, N> logical_not(const vector<T, N>& x, std::index_sequence<I...>) noexcept
{
	return {!x[I]...};
}

template <class T, std::size_t N>
inline constexpr vector<bool, N> logical_not(const vector<T, N>& x) noexcept
{
	return logical_not(x, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<bool, N> not_equal(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] != y[I])...};
}

template <class T, std::size_t N>
inline constexpr vector<bool, N> not_equal(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return not_equal(x, y, std::make_index_sequence<N>{});
}

/// @private
template <std::floating_point T, std::size_t N, std::size_t... I>
inline vector<T, N> pow(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>)
{
	return {std::pow(x[I], y[I])...};
}

template <std::floating_point T, std::size_t N>
inline vector<T, N> pow(const vector<T, N>& x, const vector<T, N>& y)
{
	return pow(x, y, std::make_index_sequence<N>{});
}

/// @private
template <std::floating_point T, std::size_t N, std::size_t... I>
inline vector<T, N> pow(const vector<T, N>& x, T y, std::index_sequence<I...>)
{
	return {std::pow(x[I], y)...};
}

template <std::floating_point T, std::size_t N>
inline vector<T, N> pow(const vector<T, N>& x, T y)
{
	return pow(x, y, std::make_index_sequence<N>{});
}

/// @private
template <std::floating_point T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> round(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {std::round(x[I])...};
}

template <std::floating_point T, std::size_t N>
inline constexpr vector<T, N> round(const vector<T, N>& x)
{
	return round(x, std::make_index_sequence<N>{});
}

/// @private
template <std::floating_point T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> sign(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {std::copysign(T{1}, x[I])...};
}

template <std::floating_point T, std::size_t N>
inline constexpr vector<T, N> sign(const vector<T, N>& x)
{
	return sign(x, std::make_index_sequence<N>{});
}

template <class T, std::size_t N>
inline constexpr T sqr_distance(const vector<T, N>& p0, const vector<T, N>& p1) noexcept
{
	return sqr_length(sub(p0, p1));
}

template <class T, std::size_t N>
inline constexpr T sqr_length(const vector<T, N>& x) noexcept
{
	return dot(x, x);
}

/// @private
template <std::floating_point T, std::size_t N, std::size_t... I>
inline vector<T, N> sqrt(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {std::sqrt(x[I])...};
}

template <std::floating_point T, std::size_t N>
inline vector<T, N> sqrt(const vector<T, N>& x, const vector<T, N>& y)
{
	return sqrt(x, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> sub(const vector<T, N>& x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x[I] - y[I])...};
}

template <class T, std::size_t N>
inline constexpr vector<T, N> sub(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return sub(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> sub(const vector<T, N>& x, T y, std::index_sequence<I...>) noexcept
{
	return {(x[I] - y)...};
}

template <class T, std::size_t N>
inline constexpr vector<T, N> sub(const vector<T, N>& x, T y) noexcept
{
	return sub(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> sub(T x, const vector<T, N>& y, std::index_sequence<I...>) noexcept
{
	return {(x - y[I])...};
}

template <class T, std::size_t N>
inline constexpr vector<T, N> sub(T x, const vector<T, N>& y) noexcept
{
	return sub(x, y, std::make_index_sequence<N>{});
}

/// @private
template <class T, std::size_t N, std::size_t... I>
inline constexpr T sum(const vector<T, N>& x, std::index_sequence<I...>) noexcept
{
	return (x[I] + ...);
}

template <class T, std::size_t N>
inline constexpr T sum(const vector<T, N>& x) noexcept
{
	return sum(x, std::make_index_sequence<N>{});
}

template <std::size_t... Indices, class T, std::size_t N>
inline constexpr vector<T, sizeof...(Indices)> swizzle(const vector<T, N>& x) noexcept
{
	return {x[Indices]...};
}

/// @private
template <std::floating_point T, std::size_t N, std::size_t... I>
inline constexpr vector<T, N> trunc(const vector<T, N>& x, std::index_sequence<I...>)
{
	return {std::trunc(x[I])...};
}

template <std::floating_point T, std::size_t N>
inline constexpr vector<T, N> trunc(const vector<T, N>& x)
{
	return trunc(x, std::make_index_sequence<N>{});
}

namespace operators {

/// @copydoc add(const vector<T, N>&, const vector<T, N>&)
template <class T, std::size_t N>
inline constexpr vector<T, N> operator+(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return add(x, y);
}

/// @copydoc add(const vector<T, N>&, T)
/// @{
template <class T, std::size_t N>
inline constexpr vector<T, N> operator+(const vector<T, N>& x, T y) noexcept
{
	return add(x, y);
}
template <class T, std::size_t N>
inline constexpr vector<T, N> operator+(T x, const vector<T, N>& y) noexcept
{
	return add(y, x);
}
/// @}

/// @copydoc div(const vector<T, N>&, const vector<T, N>&)
template <class T, std::size_t N>
inline constexpr vector<T, N> operator/(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return div(x, y);
}

/// @copydoc div(const vector<T, N>&, T)
template <class T, std::size_t N>
inline constexpr vector<T, N> operator/(const vector<T, N>& x, T y) noexcept
{
	return div(x, y);
}

/// @copydoc div(T, const vector<T, N>&)
template <class T, std::size_t N>
inline constexpr vector<T, N> operator/(T x, const vector<T, N>& y) noexcept
{
	return div(x, y);
}

/// @copydoc mul(const vector<T, N>&, const vector<T, N>&)
template <class T, std::size_t N>
inline constexpr vector<T, N> operator*(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return mul(x, y);
}

/// @copydoc mul(const vector<T, N>&, T)
/// @{
template <class T, std::size_t N>
inline constexpr vector<T, N> operator*(const vector<T, N>& x, T y) noexcept
{
	return mul(x, y);
}
template <class T, std::size_t N>
inline constexpr vector<T, N> operator*(T x, const vector<T, N>& y) noexcept
{
	return mul(y, x);
}
/// @}

/// @copydoc negate(const vector<T, N>&)
template <class T, std::size_t N>
inline constexpr vector<T, N> operator-(const vector<T, N>& x) noexcept
{
	return negate(x);
}

/// @copydoc sub(const vector<T, N>&, const vector<T, N>&)
template <class T, std::size_t N>
inline constexpr vector<T, N> operator-(const vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return sub(x, y);
}

/// @copydoc sub(const vector<T, N>&, T)
template <class T, std::size_t N>
inline constexpr vector<T, N> operator-(const vector<T, N>& x, T y) noexcept
{
	return sub(x, y);
}

/// @copydoc sub(T, const vector<T, N>&)
template <class T, std::size_t N>
inline constexpr vector<T, N> operator-(T x, const vector<T, N>& y) noexcept
{
	return sub(x, y);
}

/**
 * Adds two values and stores the result in the first value.
 *
 * @param x First value.
 * @param y Second value.
 *
 * @return Reference to the first value.
 */
/// @{
template <class T, std::size_t N>
inline constexpr vector<T, N>& operator+=(vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return (x = x + y);
}
template <class T, std::size_t N>
inline constexpr vector<T, N>& operator+=(vector<T, N>& x, T y) noexcept
{
	return (x = x + y);
}
/// @}

/**
 * Subtracts the first value by the second value and stores the result in the first value.
 *
 * @param x First value.
 * @param y Second value.
 *
 * @return Reference to the first value.
 */
/// @{
template <class T, std::size_t N>
inline constexpr vector<T, N>& operator-=(vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return (x = x - y);
}
template <class T, std::size_t N>
inline constexpr vector<T, N>& operator-=(vector<T, N>& x, T y) noexcept
{
	return (x = x - y);
}
/// @}

/**
 * Multiplies two values and stores the result in the first value.
 *
 * @param x First value.
 * @param y Second value.
 *
 * @return Reference to the first value.
 */
/// @{
template <class T, std::size_t N>
inline constexpr vector<T, N>& operator*=(vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return (x = x * y);
}
template <class T, std::size_t N>
inline constexpr vector<T, N>& operator*=(vector<T, N>& x, T y) noexcept
{
	return (x = x * y);
}
/// @}

/**
 * Divides the first value by the second value and stores the result in the first value.
 *
 * @param x First value.
 * @param y Second value.
 *
 * @return Reference to the first value.
 */
/// @{
template <class T, std::size_t N>
inline constexpr vector<T, N>& operator/=(vector<T, N>& x, const vector<T, N>& y) noexcept
{
	return (x = x / y);
}
template <class T, std::size_t N>
inline constexpr vector<T, N>& operator/=(vector<T, N>& x, T y) noexcept
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
std::ostream& operator<<(std::ostream& os, const vector<T, N>& x)
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
std::istream& operator>>(std::istream& is, vector<T, N>& x)
{
	for (std::size_t i = 0; i < N; ++i)
		is >> x[i];
	
	return is;
}

} // namespace operators

} // namespace math

// Bring vector operators into global namespace
using namespace math::operators;

// Structured binding support
namespace std
{
	/**
	 * Provides access to the number of elements in a math::vector as a compile-time constant expression.
	 *
	 * @tparam T Element type.
	 * @tparam N Number of elements.
	 */
	template<class T, std::size_t N>
	struct tuple_size<math::vector<T, N>>
	{
		/// Number of elements in the vector. 
		static constexpr std::size_t value = math::vector<T, N>::element_count;
	};
	
	/**
	 * Provides compile-time indexed access to the type of the elements in a math::vector using a tuple-like interface.
	 *
	 * @tparam I Index of an element.
	 * @tparam T Element type.
	 * @tparam N Number of elements.
	 */
	template<std::size_t I, class T, std::size_t N>
	struct tuple_element<I, math::vector<T, N>>
	{
		/// Type of elements in the vector.
		using type = math::vector<T, N>::element_type;
	};
}

#endif // ANTKEEPER_MATH_VECTOR_HPP