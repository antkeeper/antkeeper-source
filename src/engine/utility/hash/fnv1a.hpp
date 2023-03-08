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
#include <cstdint>
#include <string_view>
#include <utility>
#include <span>

namespace hash {

/**
 * FNV-1a hash function.
 *
 * @tparam HashT Unsigned integral hash type.
 * @tparam DataT Data element type.
 *
 * @param data Array of data to hash.
 * @param offset FNV offset basis value.
 * @param prime FNV prime value.
 *
 * @return Hash value.
 *
 * @see https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 */
template <std::unsigned_integral HashT, std::integral DataT>
[[nodiscard]] constexpr HashT fnv1a(std::span<const DataT> data, HashT offset, HashT prime) noexcept
{
	for (auto element: data)
	{
		if constexpr (sizeof(DataT) == 1)
		{
			offset ^= static_cast<HashT>(element);
			offset *= prime;
		}
		else
		{
			/// @TODO `reinterpret_cast` is not supported in consteval. C++23 has `if consteval` which can selectively enable reinterpret_cast at runtime, and extract bytes manually at compile-time.
			for (std::size_t j = 0; j < sizeof(DataT); ++j)
			{
				offset ^= static_cast<HashT>(((element) >> (j * 8)) & 255);
				offset *= prime;
			}
		}
	}
	
	return offset;
}

/**
 * 32-bit FNV-1a hash function.
 *
 * @tparam DataT Data element type.
 *
 * @param data Array of data to hash.
 *
 * @return 32-bit FNV-1a hash value.
 */
template <std::integral DataT>
[[nodiscard]] constexpr std::uint32_t fnv1a32(std::span<const DataT> data) noexcept
{
	// 32-bit FNV offset basis value.
	constexpr std::uint32_t offset = 2166136261;
	
	// 32-bit FNV prime value.
	constexpr std::uint32_t prime = 16777619;
	
	return fnv1a<std::uint32_t, DataT>(data, offset, prime);
}

/**
 * 64-bit FNV-1a hash function.
 *
 * @tparam DataT Data element type.
 *
 * @param data Array of data to hash.
 *
 * @return 64-bit FNV-1a hash value.
 */
template <std::integral DataT>
[[nodiscard]] constexpr std::uint64_t fnv1a64(std::span<const DataT> data) noexcept
{
	// 64-bit FNV offset basis value.
	constexpr std::uint64_t offset = 14695981039346656037ULL;
	
	// 64-bit FNV prime value.
	constexpr std::uint64_t prime = 1099511628211ULL;
	
	return fnv1a<std::uint64_t, DataT>(data, offset, prime);
}

namespace types {

/**
 * 32-bit FNV-1a hash value.
 */
struct fnv1a32_t
{
	/// Hash value type.
	using value_type = std::uint32_t;
	
	/**
	 * Constructs a 32-bit FNV-1a hash value.
	 *
	 * @param value 32-bit FNV-1a hash value.
	 */
	/// @{
	inline constexpr fnv1a32_t(value_type value) noexcept:
		value{value}
	{}
	fnv1a32_t() = default;
	/// @}
	
	/**
	 * Constructs a 32-bit FNV-1a hash value from a string.
	 *
	 * @param string String to hash.
	 */
	/// @{
	constexpr fnv1a32_t(const char* string) noexcept:
		value{fnv1a32<char>({string, std::string_view(string).length()})}
	{}
	constexpr fnv1a32_t(const wchar_t* string) noexcept:
		value{fnv1a32<wchar_t>({string, std::wstring_view(string).length()})}
	{}
	constexpr fnv1a32_t(const char8_t* string) noexcept:
		value{fnv1a32<char8_t>({string, std::u8string_view(string).length()})}
	{}
	constexpr fnv1a32_t(const char16_t* string) noexcept:
		value{fnv1a32<char16_t>({string, std::u16string_view(string).length()})}
	{}
	constexpr fnv1a32_t(const char32_t* string) noexcept:
		value{fnv1a32<char32_t>({string, std::u32string_view(string).length()})}
	{}
	/// @}
	
	/**
	 * Converts a 32-bit FNV-1a hash value to its underlying type.
	 *
	 * @return 32-bit FNV-1a hash value.
	 */
	[[nodiscard]] inline constexpr operator value_type() const noexcept
	{
		return value;
	}
	
	/// 32-bit FNV-1a hash value.
	value_type value;
};
static_assert(sizeof(fnv1a32_t) == sizeof(std::uint32_t));

/**
 * 64-bit FNV-1a hash value.
 */
struct fnv1a64_t
{
	/// Hash value type.
	using value_type = std::uint64_t;
	
	/**
	 * Constructs a 64-bit FNV-1a hash value.
	 */
	/// @{
	inline constexpr fnv1a64_t(value_type value) noexcept:
		value{value}
	{}
	fnv1a64_t() = default;
	/// @}
	
	/**
	 * Constructs a 64-bit FNV-1a hash value from a string.
	 *
	 * @param string String to hash.
	 */
	/// @{
	constexpr fnv1a64_t(const char* string) noexcept:
		value{fnv1a64<char>({string, std::string_view(string).length()})}
	{}
	constexpr fnv1a64_t(const wchar_t* string) noexcept:
		value{fnv1a64<wchar_t>({string, std::wstring_view(string).length()})}
	{}
	constexpr fnv1a64_t(const char8_t* string) noexcept:
		value{fnv1a64<char8_t>({string, std::u8string_view(string).length()})}
	{}
	constexpr fnv1a64_t(const char16_t* string) noexcept:
		value{fnv1a64<char16_t>({string, std::u16string_view(string).length()})}
	{}
	constexpr fnv1a64_t(const char32_t* string) noexcept:
		value{fnv1a64<char32_t>({string, std::u32string_view(string).length()})}
	{}
	/// @}
	
	/**
	 * Converts a 64-bit FNV-1a hash value to its underlying type.
	 *
	 * @return 64-bit FNV-1a hash value.
	 */
	[[nodiscard]] inline constexpr operator value_type() const noexcept
	{
		return value;
	}
	
	/// 64-bit FNV-1a hash value.
	value_type value;
};
static_assert(sizeof(fnv1a64_t) == sizeof(std::uint64_t));

} // namespace types

// Bring hash::types namespace into hash namespace
using namespace hash::types;

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
	return hash::fnv1a32<char>({string, length});
}

[[nodiscard]] consteval std::uint32_t operator"" _fnv1a32(const wchar_t* string, std::size_t length) noexcept
{
	return hash::fnv1a32<wchar_t>({string, length});
}

[[nodiscard]] consteval std::uint32_t operator"" _fnv1a32(const char8_t* string, std::size_t length) noexcept
{
	return hash::fnv1a32<char8_t>({string, length});
}

[[nodiscard]] consteval std::uint32_t operator"" _fnv1a32(const char16_t* string, std::size_t length) noexcept
{
	return hash::fnv1a32<char16_t>({string, length});
}

[[nodiscard]] consteval std::uint32_t operator"" _fnv1a32(const char32_t* string, std::size_t length) noexcept
{
	return hash::fnv1a32<char32_t>({string, length});
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
	return hash::fnv1a64<char>({string, length});
}

[[nodiscard]] consteval std::uint64_t operator"" _fnv1a64(const wchar_t* string, std::size_t length) noexcept
{
	return hash::fnv1a64<wchar_t>({string, length});
}

[[nodiscard]] consteval std::uint64_t operator"" _fnv1a64(const char8_t* string, std::size_t length) noexcept
{
	return hash::fnv1a64<char8_t>({string, length});
}

[[nodiscard]] consteval std::uint64_t operator"" _fnv1a64(const char16_t* string, std::size_t length) noexcept
{
	return hash::fnv1a64<char16_t>({string, length});
}

[[nodiscard]] consteval std::uint64_t operator"" _fnv1a64(const char32_t* string, std::size_t length) noexcept
{
	return hash::fnv1a64<char32_t>({string, length});
}
/// @}

} // namespace literals

// Bring hash::literals namespace into hash namespace
using namespace hash::literals;

} // namespace hash

/**
 * Compares two FNV-1a hash values for equality.
 */
/// @{
[[nodiscard]] inline constexpr bool operator==(const ::hash::fnv1a32_t& lhs, const ::hash::fnv1a32_t& rhs) noexcept
{
	return lhs.value == rhs.value;
}
[[nodiscard]] inline constexpr bool operator==(const ::hash::fnv1a64_t& lhs, const ::hash::fnv1a64_t& rhs) noexcept
{
	return lhs.value == rhs.value;
}
/// @}

/**
 * Checks if one FNV-1a hash value is less than another.
 */
/// @{
[[nodiscard]] inline constexpr bool operator<(const ::hash::fnv1a32_t& lhs, const ::hash::fnv1a32_t& rhs) noexcept
{
	return lhs.value < rhs.value;
}
[[nodiscard]] inline constexpr bool operator<(const ::hash::fnv1a64_t& lhs, const ::hash::fnv1a64_t& rhs) noexcept
{
	return lhs.value < rhs.value;
}
/// @}

namespace std {

	template <>
	struct hash<::hash::fnv1a32_t>
	{
		[[nodiscard]] inline constexpr std::size_t operator()(const ::hash::fnv1a32_t& hash) const noexcept
		{
			return static_cast<std::size_t>(hash.value);
		}
	};
	
	template <>
	struct hash<::hash::fnv1a64_t>
	{
		[[nodiscard]] inline constexpr std::size_t operator()(const ::hash::fnv1a64_t& hash) const noexcept
		{
			return static_cast<std::size_t>(hash.value);
		}
	};

} // namespace std

#endif // ANTKEEPER_UTILITY_HASH_FNV1A_HPP
