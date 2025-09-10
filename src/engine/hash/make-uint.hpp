// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>
#include <type_traits>

namespace engine::hash
{
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
		static_assert(sizeof(float) == sizeof(u32));
	
		/// Unsigned integer type of equivalent size to `float`.
		using type = u32;
	};

	/// Provides an unsigned integer type of equivalent size to `double`.
	template<>
	struct make_uint<double>
	{
		static_assert(sizeof(double) == sizeof(u64));
	
		/// Unsigned integer type of equivalent size to `double`.
		using type = u64;
	};

	/// Helper type for make_uint.
	template <class T>
	using make_uint_t = typename make_uint<T>::type;
}
