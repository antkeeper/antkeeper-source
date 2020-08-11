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

#ifndef ANTKEEPER_MATH_VECTOR_TYPE_HPP
#define ANTKEEPER_MATH_VECTOR_TYPE_HPP

#include <array>
#include <cstddef>

namespace math {

/// @addtogroup vector
/// @{

/**
 * An `N`-dimensional Euclidean vector.
 *
 * @tparam T Vector component type.
 * @tparam N Number of dimensions.
 */
template <typename T, std::size_t N>
struct vector
{
	typedef T scalar_type;
	typedef std::array<T, N> array_type;
	scalar_type components[N];

	inline constexpr scalar_type& operator[](std::size_t i) noexcept { return components[i]; }
	inline constexpr const scalar_type& operator[](std::size_t i) const noexcept { return components[i]; }
	inline constexpr operator array_type&() noexcept { return reinterpret_cast<array_type&>(components[0]); }
	inline constexpr operator const array_type&() const noexcept { return reinterpret_cast<const array_type&>(components[0]); }
	inline constexpr const scalar_type* data() const noexcept { return components; };
	inline constexpr scalar_type* data() noexcept { return components; };
	inline constexpr std::size_t size() const noexcept { return N; };
};

template <typename T>
struct vector<T, 1>
{
	typedef T scalar_type;
	typedef std::array<T, 1> array_type;
	scalar_type x;

	inline constexpr scalar_type& operator[](std::size_t i) noexcept { return *((&x) + i); }
	inline constexpr const scalar_type& operator[](std::size_t i) const noexcept { return *((&x) + i); }
	inline constexpr operator array_type&() noexcept { return reinterpret_cast<array_type&>(x); }
	inline constexpr operator const array_type&() const noexcept { return reinterpret_cast<const array_type&>(x); }
	inline constexpr const scalar_type* data() const noexcept { return &x; };
	inline constexpr scalar_type* data() noexcept { return &x; };
	inline constexpr std::size_t size() const noexcept { return 1; };
};

template <typename T>
struct vector<T, 2>
{
	typedef T scalar_type;
	typedef std::array<T, 2> array_type;
	scalar_type x;
	scalar_type y;

	inline constexpr scalar_type& operator[](std::size_t i) noexcept { return *((&x) + i); }
	inline constexpr const scalar_type& operator[](std::size_t i) const noexcept { return *((&x) + i); }
	inline constexpr operator array_type&() noexcept { return reinterpret_cast<array_type&>(x); }
	inline constexpr operator const array_type&() const noexcept { return reinterpret_cast<const array_type&>(x); }
	inline constexpr const scalar_type* data() const noexcept { return &x; };
	inline constexpr scalar_type* data() noexcept { return &x; };
	inline constexpr std::size_t size() const noexcept { return 2; };
};

template <typename T>
struct vector<T, 3>
{
	typedef T scalar_type;
	typedef std::array<T, 3> array_type;
	scalar_type x;
	scalar_type y;
	scalar_type z;

	inline constexpr scalar_type& operator[](std::size_t i) noexcept { return *((&x) + i); }
	inline constexpr const scalar_type& operator[](std::size_t i) const noexcept { return *((&x) + i); }
	inline constexpr operator array_type&() noexcept { return reinterpret_cast<array_type&>(x); }
	inline constexpr operator const array_type&() const noexcept { return reinterpret_cast<const array_type&>(x); }
	inline constexpr const scalar_type* data() const noexcept { return &x; };
	inline constexpr scalar_type* data() noexcept { return &x; };
	inline constexpr std::size_t size() const noexcept { return 3; };

};

template <typename T>
struct vector<T, 4>
{
	typedef T scalar_type;
	typedef std::array<T, 4> array_type;
	scalar_type x;
	scalar_type y;
	scalar_type z;
	scalar_type w;

	inline constexpr scalar_type& operator[](std::size_t i) noexcept { return *((&x) + i); }
	inline constexpr const scalar_type& operator[](std::size_t i) const noexcept { return *((&x) + i); }
	inline constexpr operator array_type&() noexcept { return reinterpret_cast<array_type&>(x); }
	inline constexpr operator const array_type&() const noexcept { return reinterpret_cast<const array_type&>(x); }
	inline constexpr const scalar_type* data() const noexcept { return &x; };
	inline constexpr scalar_type* data() noexcept { return &x; };
	inline constexpr std::size_t size() const noexcept { return 4; };
};

static_assert(std::is_trivial<vector<float, 4>>::value);

/// @}

} // namespace math

#endif // ANTKEEPER_MATH_VECTOR_TYPE_HPP

