// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/simd/vector-type.hpp>
#include <engine/utility/sized-types.hpp>
#include <engine/utility/alignment.hpp>
#include <engine/debug/contract.hpp>
#include <type_traits>
#include <nmmintrin.h>

namespace engine::math::simd
{
	template <>
	struct vector_register<float, 4>
	{
		using type = __m128;
		static inline constexpr usize alignment = 16;
	};
}

namespace engine::math::simd::inline types
{
	template struct vector<float, 4>;

	static_assert(std::is_standard_layout_v<fvec4>);
	static_assert(std::is_trivially_default_constructible_v<fvec4>);
	static_assert(std::is_trivially_constructible_v<fvec4>);
	static_assert(std::is_trivially_copy_constructible_v<fvec4>);
	static_assert(std::is_trivially_move_constructible_v<fvec4>);
	static_assert(std::is_trivially_copy_assignable_v<fvec4>);
	static_assert(std::is_trivially_move_assignable_v<fvec4>);
	//static_assert(std::is_aggregate_v<fvec4>);

	template <>
	inline fvec4::vector(float value) noexcept
		: m_data{_mm_set_ps1(value)}
	{
	}

	template <>
	inline void fvec4::load(const_span elements) noexcept
	{
		debug::precondition(is_aligned<alignment>(elements.data()));
		m_data = _mm_load_ps(elements.data());
	}

	template <>
	inline void fvec4::load_unaligned(const_span elements) noexcept
	{
		m_data = _mm_loadu_ps(elements.data());
	}

	template <>
	inline void fvec4::store(span elements) const noexcept
	{
		debug::precondition(is_aligned<alignment>(elements.data()));
		_mm_store_ps(elements.data(), m_data);
	}

	template <>
	inline void fvec4::store_unaligned(span elements) const noexcept
	{
		_mm_storeu_ps(elements.data(), m_data);
	}

	template <>
	[[nodiscard]] inline const float fvec4::operator[](usize i) const noexcept
	{
		debug::precondition(i < size);

		switch (i)
		{
			default:
			case 0:
				return _mm_cvtss_f32(m_data);
			case 1:
				return _mm_cvtss_f32(_mm_shuffle_ps(m_data, m_data, _MM_SHUFFLE(1u, 1u, 1u, 1u)));
			case 2:
				return _mm_cvtss_f32(_mm_shuffle_ps(m_data, m_data, _MM_SHUFFLE(2u, 2u, 2u, 2u)));
			case 3:
				return _mm_cvtss_f32(_mm_shuffle_ps(m_data, m_data, _MM_SHUFFLE(3u, 3u, 3u, 3u)));
		}
	}
}
