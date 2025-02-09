// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_HASH_FNV_HPP
#define ANTKEEPER_HASH_FNV_HPP

#include <concepts>
#include <cstdint>
#include <utility>
#include <span>

// export module hash.fnv1a;
// import <concepts>;
// import <cstdint>;
// import <utility>;
// import <span>;

namespace hash {

/// 32-bit FNV hash value.
enum class fnv32_t: std::uint32_t{};

/// 64-bit FNV hash value.
enum class fnv64_t: std::uint64_t{};

namespace types
{
	using hash::fnv32_t;
	using hash::fnv64_t;
} // namespace types

/// FNV-1a hash function.
/// @tparam HashT Unsigned integral hash type.
/// @tparam DataT Data element type.
/// @param data Array of data to hash.
/// @param offset FNV offset basis value.
/// @param prime FNV prime value.
/// @return FNV-1a hash value.
/// @see https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
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
			for (std::size_t j = 0; j < sizeof(DataT); ++j)
			{
				offset ^= static_cast<HashT>(((element) >> (j * 8)) & 255);
				offset *= prime;
			}
		}
	}
	
	return offset;
}

/// 32-bit FNV-1a hash function.
/// @tparam DataT Data element type.
/// @param data Array of data to hash.
/// @return 32-bit FNV-1a hash value.
template <std::integral DataT>
[[nodiscard]] constexpr fnv32_t fnv1a32(std::span<const DataT> data) noexcept
{
	using underlying_t = std::underlying_type_t<fnv32_t>;

	// 32-bit FNV offset basis value.
	constexpr underlying_t offset = 2166136261;
	
	// 32-bit FNV prime value.
	constexpr underlying_t prime = 16777619;
	
	return static_cast<fnv32_t>(fnv1a<underlying_t, DataT>(data, offset, prime));
}

/// 64-bit FNV-1a hash function.
/// @tparam DataT Data element type.
/// @param data Array of data to hash.
/// @return 64-bit FNV-1a hash value.
template <std::integral DataT>
[[nodiscard]] constexpr fnv64_t fnv1a64(std::span<const DataT> data) noexcept
{
	using underlying_t = std::underlying_type_t<fnv64_t>;

	// 64-bit FNV offset basis value.
	constexpr underlying_t offset = 14695981039346656037ULL;
	
	// 64-bit FNV prime value.
	constexpr underlying_t prime = 1099511628211ULL;
	
	return static_cast<fnv64_t>(fnv1a<underlying_t, DataT>(data, offset, prime));
}

namespace literals {

/// Hashes a string at compile-time using the 32-bit FNV-1a hash function.
/// @param string String to hash.
/// @param length Number of characters in @p string.
/// @return 32-bit hash value.
[[nodiscard]] consteval fnv32_t operator"" _fnv1a32(const char* string, std::size_t length) noexcept
{
	return hash::fnv1a32<char>({string, length});
}

/// Hashes a wide string at compile-time using the 32-bit FNV-1a hash function.
/// @param string String to hash.
/// @param length Number of characters in @p string.
/// @return 32-bit hash value.
[[nodiscard]] consteval fnv32_t operator"" _fnv1a32(const wchar_t* string, std::size_t length) noexcept
{
	return hash::fnv1a32<wchar_t>({string, length});
}

/// Hashes a UTF-8 string at compile-time using the 32-bit FNV-1a hash function.
/// @param string String to hash.
/// @param length Number of characters in @p string.
/// @return 32-bit hash value.
[[nodiscard]] consteval fnv32_t operator"" _fnv1a32(const char8_t* string, std::size_t length) noexcept
{
	return hash::fnv1a32<char8_t>({string, length});
}

/// Hashes a UTF-16 string at compile-time using the 32-bit FNV-1a hash function.
/// @param string String to hash.
/// @param length Number of characters in @p string.
/// @return 32-bit hash value.
[[nodiscard]] consteval fnv32_t operator"" _fnv1a32(const char16_t* string, std::size_t length) noexcept
{
	return hash::fnv1a32<char16_t>({string, length});
}

/// Hashes a UTF-32 string at compile-time using the 32-bit FNV-1a hash function.
/// @param string String to hash.
/// @param length Number of characters in @p string.
/// @return 32-bit hash value.
[[nodiscard]] consteval fnv32_t operator"" _fnv1a32(const char32_t* string, std::size_t length) noexcept
{
	return hash::fnv1a32<char32_t>({string, length});
}

/// Hashes a string at compile-time using the 64-bit FNV-1a hash function.
/// @param string String to hash.
/// @param length Number of characters in @p string.
/// @return 64-bit hash value.
[[nodiscard]] consteval fnv64_t operator"" _fnv1a64(const char* string, std::size_t length) noexcept
{
	return hash::fnv1a64<char>({string, length});
}

/// Hashes a wide string at compile-time using the 64-bit FNV-1a hash function.
/// @param string String to hash.
/// @param length Number of characters in @p string.
/// @return 64-bit hash value.
[[nodiscard]] consteval fnv64_t operator"" _fnv1a64(const wchar_t* string, std::size_t length) noexcept
{
	return hash::fnv1a64<wchar_t>({string, length});
}

/// Hashes a UTF-8 string at compile-time using the 64-bit FNV-1a hash function.
/// @param string String to hash.
/// @param length Number of characters in @p string.
/// @return 64-bit hash value.
[[nodiscard]] consteval fnv64_t operator"" _fnv1a64(const char8_t* string, std::size_t length) noexcept
{
	return hash::fnv1a64<char8_t>({string, length});
}

/// Hashes a UTF-16 string at compile-time using the 64-bit FNV-1a hash function.
/// @param string String to hash.
/// @param length Number of characters in @p string.
/// @return 64-bit hash value.
[[nodiscard]] consteval fnv64_t operator"" _fnv1a64(const char16_t* string, std::size_t length) noexcept
{
	return hash::fnv1a64<char16_t>({string, length});
}

/// Hashes a UTF-32 string at compile-time using the 64-bit FNV-1a hash function.
/// @param string String to hash.
/// @param length Number of characters in @p string.
/// @return 64-bit hash value.
[[nodiscard]] consteval fnv64_t operator"" _fnv1a64(const char32_t* string, std::size_t length) noexcept
{
	return hash::fnv1a64<char32_t>({string, length});
}

} // namespace literals

// Bring hash::literals namespace into hash namespace
using namespace hash::literals;

} // namespace hash

namespace std
{

	/// Hash specialization for 32-bit FNV hash values.
	template <>
	struct hash<::hash::fnv32_t>
	{
		[[nodiscard]] inline constexpr std::size_t operator()(const ::hash::fnv32_t& h) const noexcept
		{
			return static_cast<std::size_t>(h);
		}
	};
	
	/// Hash specialization for 64-bit FNV hash values.
	template <>
	struct hash<::hash::fnv64_t>
	{
		[[nodiscard]] inline constexpr std::size_t operator()(const ::hash::fnv64_t& h) const noexcept
		{
			return static_cast<std::size_t>(h);
		}
	};

} // namespace std

#endif // ANTKEEPER_HASH_FNV_HPP
