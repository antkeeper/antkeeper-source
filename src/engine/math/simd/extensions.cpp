// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/simd/extensions.hpp>
#include <SDL3/SDL.h>

namespace engine::math::simd
{
	bool has_sse() noexcept
	{
		return SDL_HasSSE();
	}

	bool has_sse2() noexcept
	{
		return SDL_HasSSE2();
	}

	bool has_sse3() noexcept
	{
		return SDL_HasSSE3();
	}

	bool has_sse41() noexcept
	{
		return SDL_HasSSE41();
	}

	bool has_sse42() noexcept
	{
		return SDL_HasSSE42();
	}

	bool has_avx() noexcept
	{
		return SDL_HasAVX();
	}

	bool has_avx2() noexcept
	{
		return SDL_HasAVX2();
	}

	bool has_avx512f() noexcept
	{
		return SDL_HasAVX512F();
	}
}
