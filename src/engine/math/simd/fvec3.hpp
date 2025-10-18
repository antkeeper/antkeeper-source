// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/simd/vector-type.hpp>
#include <engine/math/simd/fvec4.hpp>
#include <engine/utility/sized-types.hpp>
#include <engine/utility/alignment.hpp>
#include <engine/debug/contract.hpp>
#include <cstring>
#include <type_traits>
#include <nmmintrin.h>

namespace engine::math::simd
{
	template <>
	struct vector_register<float, 3>
	{
		using type = __m128;
		static inline constexpr usize alignment = 16;
	};
}

namespace engine::math::simd::inline types
{
	template struct vector<float, 3>;

	static_assert(std::is_standard_layout_v<fvec3>);
	static_assert(std::is_trivially_default_constructible_v<fvec3>);
	static_assert(std::is_trivially_constructible_v<fvec3>);
	static_assert(std::is_trivially_copy_constructible_v<fvec3>);
	static_assert(std::is_trivially_move_constructible_v<fvec3>);
	static_assert(std::is_trivially_copy_assignable_v<fvec3>);
	static_assert(std::is_trivially_move_assignable_v<fvec3>);
	//static_assert(std::is_aggregate_v<fvec3>);

	template <>
	inline fvec3::vector(float value) noexcept
		: m_data{_mm_set_ps1(value)}
	{
	}

	template <>
	inline void fvec3::load(const_span elements) noexcept
	{
		debug::precondition(is_aligned<alignment>(elements.data()));

		load_unaligned(std::move(elements));

		// AVX-512F:
		//m_data = _mm_maskz_load_ps(zero_mask, elements.data());
	}

	template <>
	inline void fvec3::load_unaligned(const_span elements) noexcept
	{
		m_data = _mm_setr_ps(elements[0], elements[1], elements[2], 0.0f);

		// AVX-512F:
		//m_data = _mm_maskz_loadu_ps(zero_mask, elements.data());
	}

	template <>
	inline void fvec3::store(span elements) const noexcept
	{
		debug::precondition(is_aligned<alignment>(elements.data()));

		store_unaligned(std::move(elements));

		// AVX-512F:
		//_mm_mask_store_ps(elements.data(), zero_mask, m_data);
	}

	template <>
	inline void fvec3::store_unaligned(span elements) const noexcept
	{
		alignas(alignment) float temp[4];
		_mm_store_ps(temp, m_data);
		std::memcpy(elements.data(), temp, size * sizeof(element_type));

		// AVX-512F:
		//_mm_mask_storeu_ps(elements.data(), zero_mask, m_data);
	}

	template <>
	[[nodiscard]] inline const float fvec3::operator[](usize i) const noexcept
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
		}
	}
}
