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

#ifndef ANTKEEPER_MATH_VECTOR_TYPE_HPP
#define ANTKEEPER_MATH_VECTOR_TYPE_HPP

#include <cstddef>

namespace math {

/**
 * An `N`-dimensional Euclidean vector.
 *
 * @tparam T Scalar type.
 * @tparam N Number of dimensions.
 */
template <typename T, std::size_t N>
struct vector
{
	typedef T element_type;
	element_type elements[N];

	inline constexpr element_type& operator[](std::size_t i) noexcept { return elements[i]; }
	inline constexpr const element_type& operator[](std::size_t i) const noexcept { return elements[i]; }
	inline constexpr const element_type* data() const noexcept { return elements; };
	inline constexpr element_type* data() noexcept { return elements; };
	inline constexpr std::size_t size() const noexcept { return N; };
};

template <typename T>
struct vector<T, 1>
{
	typedef T element_type;
	
	union
	{
		element_type elements[1];
		
		struct
		{
			element_type x;
		};
	};

	inline constexpr element_type& operator[](std::size_t i) noexcept { return elements[i]; }
	inline constexpr const element_type& operator[](std::size_t i) const noexcept { return elements[i]; }
	inline constexpr const element_type* data() const noexcept { return elements; };
	inline constexpr element_type* data() noexcept { return elements; };
	inline constexpr std::size_t size() const noexcept { return 1; };
};

template <typename T>
struct vector<T, 2>
{
	typedef T element_type;
	
	union
	{
		element_type elements[2];
		
		struct
		{
			element_type x;
			element_type y;
		};
	};

	inline constexpr element_type& operator[](std::size_t i) noexcept { return elements[i]; }
	inline constexpr const element_type& operator[](std::size_t i) const noexcept { return elements[i]; }
	inline constexpr const element_type* data() const noexcept { return elements; };
	inline constexpr element_type* data() noexcept { return elements; };
	inline constexpr std::size_t size() const noexcept { return 2; };
};

template <typename T>
struct vector<T, 3>
{
	typedef T element_type;
	
	union
	{
		element_type elements[3];
		
		struct
		{
			element_type x;
			element_type y;
			element_type z;
		};
	};

	inline constexpr element_type& operator[](std::size_t i) noexcept { return elements[i]; }
	inline constexpr const element_type& operator[](std::size_t i) const noexcept { return elements[i]; }
	inline constexpr const element_type* data() const noexcept { return elements; };
	inline constexpr element_type* data() noexcept { return elements; };
	inline constexpr std::size_t size() const noexcept { return 3; };
};

template <typename T>
struct vector<T, 4>
{
	typedef T element_type;
	
	union
	{
		element_type elements[4];
		
		struct
		{
			element_type x;
			element_type y;
			element_type z;
			element_type w;
		};
	};

	inline constexpr element_type& operator[](std::size_t i) noexcept { return elements[i]; }
	inline constexpr const element_type& operator[](std::size_t i) const noexcept { return elements[i]; }
	inline constexpr const element_type* data() const noexcept { return elements; };
	inline constexpr element_type* data() noexcept { return elements; };
	inline constexpr std::size_t size() const noexcept { return 4; };
};

/// 2D vector.
template <typename T>
using vector2 = vector<T, 2>;

/// 3D vector.
template <typename T>
using vector3 = vector<T, 3>;

/// 4D vector.
template <typename T>
using vector4 = vector<T, 4>;

} // namespace math

#endif // ANTKEEPER_MATH_VECTOR_TYPE_HPP
