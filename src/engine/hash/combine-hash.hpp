// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_HASH_COMBINE_HASH_HPP
#define ANTKEEPER_HASH_COMBINE_HASH_HPP

#include <cstdint>

// export module hash.combine_hash;
// import <cstdint>;

namespace hash {

/// Combines two hash values.
/// @param x First hash value.
/// @param y Second hash value.
/// @return Combination of @p x and @p y.
[[nodiscard]] inline constexpr std::uint32_t combine_hash(std::uint32_t x, std::uint32_t y) noexcept
{
	// 0x9e3779b9 = 2^32 / Phi
	return x ^ (y + 0x9e3779b9 + (x << 6) + (x >> 2));
}

/// @copydoc combine_hash(std::uint32_t, std::uint32_t)
[[nodiscard]] inline constexpr std::uint64_t combine_hash(std::uint64_t x, std::uint64_t y) noexcept
{
	// 0x9e3779b97f4a7c16 = 2^64 / Phi
	return x ^ (y + 0x9e3779b97f4a7c16 + (x << 6) + (x >> 2));
}

} // namespace hash

#endif // ANTKEEPER_HASH_COMBINE_HASH_HPP
