// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace engine::math::simd
{
	/// Checks whether the CPU supports the SSE instruction set.
	/// @returns `true` if SSE is supported; `false` otherwise.
	[[nodiscard]] bool has_sse() noexcept;

	/// Checks whether the CPU supports the SSE2 instruction set.
	/// @returns `true` if SSE2 is supported; `false` otherwise.
	[[nodiscard]] bool has_sse2() noexcept;

	/// Checks whether the CPU supports the SSE3 instruction set.
	/// @returns `true` if SSE3 is supported; `false` otherwise.
	[[nodiscard]] bool has_sse3() noexcept;

	/// Checks whether the CPU supports the SSE4.1 instruction set.
	/// @returns `true` if SSE4.1 is supported; `false` otherwise.
	[[nodiscard]] bool has_sse41() noexcept;

	/// Checks whether the CPU supports the SSE4.2 instruction set.
	/// @returns `true` if SSE4.2 is supported; `false` otherwise.
	[[nodiscard]] bool has_sse42() noexcept;

	/// Checks whether the CPU supports the AVX instruction set.
	/// @returns `true` if AVX is supported; `false` otherwise.
	[[nodiscard]] bool has_avx() noexcept;

	/// Checks whether the CPU supports the AVX2 instruction set.
	/// @returns `true` if AVX2 is supported; `false` otherwise.
	[[nodiscard]] bool has_avx2() noexcept;

	/// Checks whether the CPU supports the AVX-512F (foundation) instruction set.
	/// @returns `true` if AVX-512F is supported; `false` otherwise.
	[[nodiscard]] bool has_avx512f() noexcept;
}
