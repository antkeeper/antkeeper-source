// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.utility.alignment;
import engine.utility.sized_types;
import <bit>;

export namespace engine
{
	/// Checks whether a pointer is aligned to a given byte boundary.
	/// @tparam N Alignment boundary, in bytes.
	/// @tparam T Pointer type.
	/// @param ptr Pointer to check.
	/// @return `true` if the pointer is aligned to the given boundary, `false` otherwise.
	template <usize N, class T>
	[[nodiscard]] inline constexpr bool is_aligned(const T* ptr) noexcept
	{
		return std::bit_cast<usize>(ptr) % N == 0;
	}
}
