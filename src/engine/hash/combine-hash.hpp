// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>

namespace engine::hash
{
	/// Combines two hash values.
	/// @param x First hash value.
	/// @param y Second hash value.
	/// @return Combination of @p x and @p y.
	[[nodiscard]] inline constexpr u32 combine_hash(u32 x, u32 y) noexcept
	{
		// 0x9e3779b9 = 2^32 / Phi
		return x ^ (y + 0x9e3779b9 + (x << 6) + (x >> 2));
	}

	/// @copydoc combine_hash(u32, u32)
	[[nodiscard]] inline constexpr u64 combine_hash(u64 x, u64 y) noexcept
	{
		// 0x9e3779b97f4a7c16 = 2^64 / Phi
		return x ^ (y + 0x9e3779b97f4a7c16 + (x << 6) + (x >> 2));
	}
}
