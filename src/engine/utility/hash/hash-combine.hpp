// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_UTILITY_HASH_COMBINE_HPP
#define ANTKEEPER_UTILITY_HASH_COMBINE_HPP

#include <cstdint>

/**
 * Combines two hash values.
 *
 * @param x First hash value.
 * @param y Second hash value.
 *
 * @return Combination of @p x and @p y.
 */
/// @{
[[nodiscard]] inline constexpr std::uint32_t hash_combine(std::uint32_t x, std::uint32_t y) noexcept
{
	// 0x9e3779b9 = 2^32 / Phi
	return x ^ (y + 0x9e3779b9 + (x << 6) + (x >> 2));
}

[[nodiscard]] inline constexpr std::uint64_t hash_combine(std::uint64_t x, std::uint64_t y) noexcept
{
	// 0x9e3779b97f4a7c16 = 2^64 / Phi
	return x ^ (y + 0x9e3779b97f4a7c16 + (x << 6) + (x >> 2));
}
/// @}

#endif // ANTKEEPER_UTILITY_HASH_COMBINE_HPP
