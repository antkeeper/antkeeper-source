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

#ifndef ANTKEEPER_UTILITY_HASH_COMBINE_HPP
#define ANTKEEPER_UTILITY_HASH_COMBINE_HPP

#include <cstdint>

namespace hash {

/**
 * Combines two hash values.
 *
 * @param x First hash value.
 * @param y Second hash value.
 *
 * @return Combination of @p x and @p y.
 */
/// @{
[[nodiscard]] inline std::uint32_t combine(std::uint32_t x, std::uint32_t y) noexcept
{
	// 0x9e3779b9 = 2^32 / Phi
	return x ^ (y + 0x9e3779b9 + (x << 6) + (x >> 2));
}

[[nodiscard]] inline std::uint64_t combine(std::uint64_t x, std::uint64_t y) noexcept
{
	// 0x9e3779b97f4a7c16 = 2^64 / Phi
	return x ^ (y + 0x9e3779b97f4a7c16 + (x << 6) + (x >> 2));
}
/// @}

} // namespace hash

#endif // ANTKEEPER_UTILITY_HASH_COMBINE_HPP
