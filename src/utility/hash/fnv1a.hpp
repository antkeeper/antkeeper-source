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

#ifndef ANTKEEPER_UTILITY_HASH_FNV1A_HPP
#define ANTKEEPER_UTILITY_HASH_FNV1A_HPP

#include <concepts>
#include <cstddef>
#include <cstdint>

namespace hash {

/**
 * FNV-1a hash function.
 *
 * @tparam HashT Unsigned integral hash type.
 * @tparam CharT String character type.
 *
 * @param string String to hash.
 * @param length Number of characters in @p string.
 * @param offset FNV offset basis value.
 * @param prime FNV prime value.
 *
 * @return Hash value.
 *
 * @see https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 */
template <std::unsigned_integral HashT, std::integral CharT>
[[nodiscard]] constexpr HashT fnv1a(const CharT* string, std::size_t length, HashT offset, HashT prime) noexcept
{
	for (std::size_t i = 0; i < length; ++i)
	{
		if constexpr (sizeof(CharT) == 1)
		{
			offset ^= static_cast<HashT>(*string);
			offset *= prime;
		}
		else
		{
			/// @TODO `reinterpret_cast` is not supported in consteval. C++23 has `if consteval` which can selectively enable reinterpret_cast at runtime, and extract bytes manually at compile-time.
			for (std::size_t j = 0; j < sizeof(CharT); ++j)
			{
				offset ^= static_cast<HashT>(((*string) >> (j * 8)) & 255);
				offset *= prime;
			}
		}
		
		++string;
	}
	
	return offset;
}

/**
 * 32-bit FNV-1a hash function.
 *
 * @param string String to hash.
 * @param length Number of characters in @p string.
 *
 * @return 32-bit hash value.
 */
template <std::integral CharT>
[[nodiscard]] constexpr std::uint32_t fnv1a32(const CharT* string, std::size_t length) noexcept
{
	// 32-bit FNV offset basis value.
	constexpr std::uint32_t offset = 2166136261;
	
	// 32-bit FNV prime value.
	constexpr std::uint32_t prime = 16777619;
	
	return fnv1a<std::uint32_t, CharT>(string, length, offset, prime);
}

/**
 * 64-bit FNV-1a hash function.
 *
 * @param string String to hash.
 * @param length Number of characters in @p string.
 *
 * @return 64-bit hash value.
 */
template <std::integral CharT>
[[nodiscard]] constexpr std::uint64_t fnv1a64(const CharT* string, std::size_t length) noexcept
{
	// 64-bit FNV offset basis value.
	constexpr std::uint64_t offset = 14695981039346656037ULL;
	
	// 64-bit FNV prime value.
	constexpr std::uint64_t prime = 1099511628211ULL;
	
	return fnv1a<std::uint64_t>(string, length, offset, prime);
}

namespace literals {

/**
 * Hashes a string at compile-time using the 32-bit FNV-1a hash function.
 *
 * @param string String to hash.
 * @param length Number of characters in @p string.
 *
 * @return 32-bit hash value.
 */
/// @{
[[nodiscard]] consteval std::uint32_t operator"" _fnv1a32(const char* string, std::size_t length) noexcept
{
	return hash::fnv1a32(string, length);
}

[[nodiscard]] consteval std::uint32_t operator"" _fnv1a32(const wchar_t* string, std::size_t length) noexcept
{
	return hash::fnv1a32(string, length);
}

[[nodiscard]] consteval std::uint32_t operator"" _fnv1a32(const char8_t* string, std::size_t length) noexcept
{
	return hash::fnv1a32(string, length);
}

[[nodiscard]] consteval std::uint32_t operator"" _fnv1a32(const char16_t* string, std::size_t length) noexcept
{
	return hash::fnv1a32(string, length);
}

[[nodiscard]] consteval std::uint32_t operator"" _fnv1a32(const char32_t* string, std::size_t length) noexcept
{
	return hash::fnv1a32(string, length);
}
/// @}

/**
 * Hashes a string at compile-time using the 64-bit FNV-1a hash function.
 *
 * @param string String to hash.
 * @param length Number of characters in @p string.
 *
 * @return 64-bit hash value.
 */
/// @{
[[nodiscard]] consteval std::uint64_t operator"" _fnv1a64(const char* string, std::size_t length) noexcept
{
	return hash::fnv1a64(string, length);
}

[[nodiscard]] consteval std::uint64_t operator"" _fnv1a64(const wchar_t* string, std::size_t length) noexcept
{
	return hash::fnv1a64(string, length);
}

[[nodiscard]] consteval std::uint64_t operator"" _fnv1a64(const char8_t* string, std::size_t length) noexcept
{
	return hash::fnv1a64(string, length);
}

[[nodiscard]] consteval std::uint64_t operator"" _fnv1a64(const char16_t* string, std::size_t length) noexcept
{
	return hash::fnv1a64(string, length);
}

[[nodiscard]] consteval std::uint64_t operator"" _fnv1a64(const char32_t* string, std::size_t length) noexcept
{
	return hash::fnv1a64(string, length);
}
/// @}

} // namespace literals

} // namespace hash

#endif // ANTKEEPER_UTILITY_HASH_FNV1A_HPP
