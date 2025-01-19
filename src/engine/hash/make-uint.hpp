// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_HASH_MAKE_UINT_HPP
#define ANTKEEPER_HASH_MAKE_UINT_HPP

#include <cstdint>
#include <type_traits>

// export module hash.make_uint;
// import <cstdint>;
// import <type_traits>;

namespace hash {

/// Provides an unsigned integer type of equivalent size to type @p T.
template <class T>
struct make_uint
{
	static_assert(std::is_integral<T>::value);
	
	/// Unsigned integer type of equivalent size to type @p T.
	using type = std::make_unsigned<T>::type;
};

/// Provides an unsigned integer type of equivalent size to `float`.
template<>
struct make_uint<float>
{
	static_assert(sizeof(float) == sizeof(std::uint32_t));
	
	/// Unsigned integer type of equivalent size to `float`.
	using type = std::uint32_t;
};

/// Provides an unsigned integer type of equivalent size to `double`.
template<>
struct make_uint<double>
{
	static_assert(sizeof(double) == sizeof(std::uint64_t));
	
	/// Unsigned integer type of equivalent size to `double`.
	using type = std::uint64_t;
};

/// Helper type for make_uint.
template <class T>
using make_uint_t = typename make_uint<T>::type;

} // namespace hash

#endif // ANTKEEPER_HASH_MAKE_UINT_HPP
