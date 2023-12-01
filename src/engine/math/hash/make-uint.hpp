// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
