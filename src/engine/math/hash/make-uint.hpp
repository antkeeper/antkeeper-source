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

#ifndef ANTKEEPER_MATH_HASH_MAKE_UINT_HPP
#define ANTKEEPER_MATH_HASH_MAKE_UINT_HPP

#include <cstdint>
#include <type_traits>

namespace math {
namespace hash {

/**
 * Provides an unsigned integer type of equivalent size to type @p T.
 */
template <class T>
struct make_uint
{
	static_assert(std::is_integral<T>::value);
	
	/// Unsigned integer type of equivalent size to type @p T.
	typedef typename std::make_unsigned<T>::type type;
};

/// Provides an unsigned integer type of equivalent size to `float`.
template<>
struct make_uint<float>
{
	static_assert(sizeof(float) == sizeof(std::uint32_t));
	
	/// Unsigned integer type of equivalent size to `float`.
	typedef std::uint32_t type;
};

/// Provides an unsigned integer type of equivalent size to `double`.
template<>
struct make_uint<double>
{
	static_assert(sizeof(double) == sizeof(std::uint64_t));
	
	/// Unsigned integer type of equivalent size to `double`.
	typedef std::uint64_t type;
};

/// Helper type for make_uint.
template <class T>
using make_uint_t = typename make_uint<T>::type;

} // namespace hash
} // namespace math

#endif // ANTKEEPER_MATH_HASH_MAKE_UINT_HPP
