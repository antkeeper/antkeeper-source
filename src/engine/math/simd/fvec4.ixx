// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.math.simd.vector:fvec4;
import :type;
import engine.math.functions;
import engine.utility.sized_types;
import engine.utility.alignment;
import engine.debug.contract;
import <nmmintrin.h>;

export namespace engine::math::simd
{
	template <>
	struct vector_register<float, 4>
	{
		using type = __m128;
		static inline constexpr usize alignment = 16;
	};
}

export namespace engine::math::simd::inline types
{
	template struct vector<float, 4>;

	template <>
	inline fvec4::vector(float value) noexcept
		: m_data{_mm_set_ps1(value)}
	{
	}

	template <>
	inline void fvec4::load(const_span elements)
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
	inline void fvec4::store(span elements)
	{
		debug::precondition(is_aligned<alignment>(elements.data()));
		_mm_store_ps(elements.data(), m_data);
	}

	template <>
	inline void fvec4::store_unaligned(span elements) noexcept
	{
		_mm_storeu_ps(elements.data(), m_data);
	}

	template <>
	[[nodiscard]] inline const float fvec4::operator[](usize i) const noexcept
	{
		debug::precondition(i < 4);

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

	template <>
	inline fvec4& fvec4::operator++() noexcept
	{
		m_data = _mm_add_ps(m_data, _mm_set_ps1(1.0f));
		return *this;
	}

	template <>
	[[nodiscard]] inline fvec4 fvec4::operator++(int) noexcept
	{
		auto temp = *this;
		m_data = _mm_add_ps(m_data, _mm_set_ps1(1.0f));
		return temp;
	}

	template <>
	inline fvec4& fvec4::operator--() noexcept
	{
		m_data = _mm_sub_ps(m_data, _mm_set_ps1(1.0f));
		return *this;
	}

	template <>
	[[nodiscard]] inline fvec4 fvec4::operator--(int) noexcept
	{
		auto temp = *this;
		m_data = _mm_sub_ps(m_data, _mm_set_ps1(1.0f));
		return temp;
	}

	template <>
	[[nodiscard]] inline fvec4 fvec4::operator-() const noexcept
	{
		static const auto mask = _mm_set_ps1(-0.0f);
		return fvec4{_mm_xor_ps(m_data, mask)};
	}

	[[nodiscard]] inline fvec4 operator+(const fvec4& lhs, const fvec4& rhs) noexcept
	{
		return fvec4{_mm_add_ps(lhs.m_data, rhs.m_data)};
	}

	[[nodiscard]] inline fvec4 operator+(const fvec4& lhs, float rhs) noexcept
	{
		return fvec4{_mm_add_ps(lhs.m_data, _mm_set_ps1(rhs))};
	}

	[[nodiscard]] inline fvec4 operator+(float lhs, const fvec4& rhs) noexcept
	{
		return fvec4{_mm_add_ps(_mm_set_ps1(lhs), rhs.m_data)};
	}

	[[nodiscard]] inline fvec4 operator-(const fvec4& lhs, const fvec4& rhs) noexcept
	{
		return fvec4{_mm_sub_ps(lhs.m_data, rhs.m_data)};
	}

	[[nodiscard]] inline fvec4 operator-(const fvec4& lhs, float rhs) noexcept
	{
		return fvec4{_mm_sub_ps(lhs.m_data, _mm_set_ps1(rhs))};
	}

	[[nodiscard]] inline fvec4 operator-(float lhs, const fvec4& rhs) noexcept
	{
		return fvec4{_mm_sub_ps(_mm_set_ps1(lhs), rhs.m_data)};
	}

	[[nodiscard]] inline fvec4 operator*(const fvec4& lhs, const fvec4& rhs) noexcept
	{
		return fvec4{_mm_mul_ps(lhs.m_data, rhs.m_data)};
	}

	[[nodiscard]] inline fvec4 operator*(const fvec4& lhs, float rhs) noexcept
	{
		return fvec4{_mm_mul_ps(lhs.m_data, _mm_set_ps1(rhs))};
	}

	[[nodiscard]] inline fvec4 operator*(float lhs, const fvec4& rhs) noexcept
	{
		return fvec4{_mm_mul_ps(_mm_set_ps1(lhs), rhs.m_data)};
	}

	[[nodiscard]] inline fvec4 operator/(const fvec4& lhs, const fvec4& rhs) noexcept
	{
		return fvec4{_mm_div_ps(lhs.m_data, rhs.m_data)};
	}

	[[nodiscard]] inline fvec4 operator/(const fvec4& lhs, float rhs) noexcept
	{
		return fvec4{_mm_div_ps(lhs.m_data, _mm_set_ps1(rhs))};
	}

	[[nodiscard]] inline fvec4 operator/(float lhs, const fvec4& rhs) noexcept
	{
		return fvec4{_mm_div_ps(_mm_set_ps1(lhs), rhs.m_data)};
	}

	inline fvec4& operator+=(fvec4& lhs, const fvec4& rhs) noexcept
	{
		lhs.m_data = _mm_add_ps(lhs.m_data, rhs.m_data);
		return lhs;
	}

	inline fvec4& operator+=(fvec4& lhs, float rhs) noexcept
	{
		lhs.m_data = _mm_add_ps(lhs.m_data, _mm_set_ps1(rhs));
		return lhs;
	}

	inline fvec4& operator-=(fvec4& lhs, const fvec4& rhs) noexcept
	{
		lhs.m_data = _mm_sub_ps(lhs.m_data, rhs.m_data);
		return lhs;
	}

	inline fvec4& operator-=(fvec4& lhs, float rhs) noexcept
	{
		lhs.m_data = _mm_sub_ps(lhs.m_data, _mm_set_ps1(rhs));
		return lhs;
	}

	inline fvec4& operator*=(fvec4& lhs, const fvec4& rhs) noexcept
	{
		lhs.m_data = _mm_mul_ps(lhs.m_data, rhs.m_data);
		return lhs;
	}

	inline fvec4& operator*=(fvec4& lhs, float rhs) noexcept
	{
		lhs.m_data = _mm_mul_ps(lhs.m_data, _mm_set_ps1(rhs));
		return lhs;
	}

	inline fvec4& operator/=(fvec4& lhs, const fvec4& rhs) noexcept
	{
		lhs.m_data = _mm_div_ps(lhs.m_data, rhs.m_data);
		return lhs;
	}

	inline fvec4& operator/=(fvec4& lhs, float rhs) noexcept
	{
		lhs.m_data = _mm_div_ps(lhs.m_data, _mm_set_ps1(rhs));
		return lhs;
	}
}

export namespace engine::math::inline functions
{
	[[nodiscard]] inline float dot(const simd::fvec4& lhs, const simd::fvec4& rhs) noexcept
	{
		return _mm_cvtss_f32(_mm_dp_ps(lhs.m_data, rhs.m_data, 0xf1));
	}

	[[nodiscard]] inline float sqr_length(const simd::fvec4& v) noexcept
	{
		return _mm_cvtss_f32(_mm_dp_ps(v.m_data, v.m_data, 0xf1));
	}

	[[nodiscard]] inline float length(const simd::fvec4& v) noexcept
	{
		return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(v.m_data, v.m_data, 0xf1)));
	}

	[[nodiscard]] inline float rcp_length(const simd::fvec4& v) noexcept
	{
		return 1.0f / length(v);
	}

	[[nodiscard]] inline simd::fvec4 normalize(const simd::fvec4& v) noexcept
	{
		const auto length = _mm_sqrt_ss(_mm_dp_ps(v.m_data, v.m_data, 0xf1));
		return simd::fvec4{_mm_div_ps(v.m_data, _mm_shuffle_ps(length, length, 0u))};
	}

	[[nodiscard]] inline float sqr_distance(const simd::fvec4& p0, const simd::fvec4& p1) noexcept
	{
		return sqr_length(p0 - p1);
	}

	[[nodiscard]] inline float distance(const simd::fvec4& p0, const simd::fvec4& p1) noexcept
	{
		return length(p0 - p1);
	}

    [[nodiscard]] inline simd::fvec4 abs(const simd::fvec4& v) noexcept  
	{
		static const auto mask = _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff));
		return simd::fvec4{_mm_and_ps(v.m_data, mask)};
	}

	[[nodiscard]] inline simd::fvec4 copysign(float magnitude, const simd::fvec4& v) noexcept
	{
		const auto sign_bits = _mm_and_ps(v.m_data, _mm_set1_ps(-0.0f));
		const auto abs_magnitude = _mm_and_ps(_mm_set_ps1(magnitude), _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff)));
		return simd::fvec4{_mm_or_ps(abs_magnitude, sign_bits)};
	}

	[[nodiscard]] inline simd::fvec4 copysign(const simd::fvec4& magnitude, const simd::fvec4& v) noexcept
	{
		const auto sign_bits = _mm_and_ps(v.m_data, _mm_set1_ps(-0.0f));
		const auto abs_magnitude = _mm_and_ps(magnitude.m_data, _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff)));
		return simd::fvec4{_mm_or_ps(abs_magnitude, sign_bits)};
	}

	[[nodiscard]] inline simd::fvec4 sign(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_or_ps(_mm_and_ps(v.m_data, _mm_set1_ps(-0.0f)), _mm_set1_ps(1.0f))};
	}

	[[nodiscard]] inline simd::fvec4 min(const simd::fvec4& lhs, const simd::fvec4& rhs) noexcept
	{
		return simd::fvec4{_mm_min_ps(lhs.m_data, rhs.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 max(const simd::fvec4& lhs, const simd::fvec4& rhs) noexcept
	{
		return simd::fvec4{_mm_max_ps(lhs.m_data, rhs.m_data)};
	}

	[[nodiscard]] inline float sum(const simd::fvec4& v) noexcept
	{
		const auto shuffle = _mm_movehdup_ps(v.m_data);
		const auto sums = _mm_add_ps(v.m_data, shuffle);
		return _mm_cvtss_f32(_mm_add_ss(sums, _mm_movehl_ps(shuffle, sums)));
	}

	[[nodiscard]] inline simd::fvec4 clamp(const simd::fvec4& v, const simd::fvec4& min, const simd::fvec4& max) noexcept
	{
		return math::min(math::max(v, min), max);
	}

	[[nodiscard]] inline simd::fvec4 clamp(const simd::fvec4& v, float min, float max) noexcept
	{
		return simd::fvec4{_mm_min_ps(_mm_max_ps(v.m_data, _mm_set1_ps(min)), _mm_set1_ps(max))};
	}

	[[nodiscard]] inline simd::fvec4 clamp_length(const simd::fvec4& v, float max_length) noexcept
	{
		const auto sqr_l = sqr_length(v);
		return (sqr_l > sqr(max_length)) ? (v * (max_length / sqrt(sqr_l))) : v;
	}

	[[nodiscard]] inline float min_element(const simd::fvec4& v) noexcept
	{
		const auto shuffle = _mm_movehdup_ps(v.m_data);
		const auto min = _mm_min_ps(v.m_data, shuffle);
		return _mm_cvtss_f32(_mm_min_ss(min, _mm_movehl_ps(shuffle, min)));
	}

	[[nodiscard]] inline float max_element(const simd::fvec4& v) noexcept
	{
		const auto shuffle = _mm_movehdup_ps(v.m_data);
		const auto max = _mm_max_ps(v.m_data, shuffle);
		return _mm_cvtss_f32(_mm_max_ss(max, _mm_movehl_ps(shuffle, max)));
	}

	[[nodiscard]] inline simd::fvec4 floor(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_floor_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 ceil(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_ceil_ps(v.m_data)};
	}

	[[nodiscard]] simd::fvec4 round(const simd::fvec4& v) noexcept
	{
		const auto sign_bits = _mm_and_ps(v.m_data, _mm_set1_ps(-0.0f));
		const auto sign = _mm_or_ps(sign_bits, _mm_set1_ps(1.0f));
		const auto i = _mm_trunc_ps(v.m_data);
		const auto abs_f = _mm_and_ps(_mm_sub_ps(v.m_data, i), _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff)));
		const auto ge_half = _mm_cmpge_ps(abs_f, _mm_set1_ps(0.5f));
		return simd::fvec4{_mm_add_ps(i, _mm_or_ps(_mm_and_ps(ge_half, sign), sign_bits))};
	}

	[[nodiscard]] inline simd::fvec4 roundeven(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_round_ps(v.m_data, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)};
	}

	[[nodiscard]] inline simd::fvec4 fract(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_sub_ps(v.m_data, _mm_floor_ps(v.m_data))};
	}

	[[nodiscard]] inline simd::fvec4 trunc(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_trunc_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 rcp(const simd::fvec4& v) noexcept
	{
		return 1.0f / v;
	}

	[[nodiscard]] inline simd::fvec4 fma(const simd::fvec4& a, const simd::fvec4& b, const simd::fvec4& c) noexcept
	{
		return simd::fvec4{_mm_fmadd_ps(a.m_data, b.m_data, c.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 fma(const simd::fvec4& a, float b, float c) noexcept
	{
		return simd::fvec4{_mm_fmadd_ps(a.m_data, _mm_set_ps1(b), _mm_set_ps1(c))};
	}

	[[nodiscard]] inline simd::fvec4 fms(const simd::fvec4& a, const simd::fvec4& b, const simd::fvec4& c) noexcept
	{
		return simd::fvec4{_mm_fmsub_ps(a.m_data, b.m_data, c.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 fms(const simd::fvec4& a, float b, float c) noexcept
	{
		return simd::fvec4{_mm_fmsub_ps(a.m_data, _mm_set_ps1(b), _mm_set_ps1(c))};
	}

	[[nodiscard]] inline simd::fvec4 fnma(const simd::fvec4& a, const simd::fvec4& b, const simd::fvec4& c) noexcept
	{
		return simd::fvec4{_mm_fnmadd_ps(a.m_data, b.m_data, c.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 fnma(const simd::fvec4& a, float b, float c) noexcept
	{
		return simd::fvec4{_mm_fnmadd_ps(a.m_data, _mm_set_ps1(b), _mm_set_ps1(c))};
	}

	[[nodiscard]] inline simd::fvec4 fnms(const simd::fvec4& a, const simd::fvec4& b, const simd::fvec4& c) noexcept
	{
		return simd::fvec4{_mm_fnmsub_ps(a.m_data, b.m_data, c.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 fnms(const simd::fvec4& a, float b, float c) noexcept
	{
		return simd::fvec4{_mm_fnmsub_ps(a.m_data, _mm_set_ps1(b), _mm_set_ps1(c))};
	}

	[[nodiscard]] inline simd::fvec4 sqr(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_mul_ps(v.m_data, v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 cube(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_mul_ps(_mm_mul_ps(v.m_data, v.m_data), v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 sqrt(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_sqrt_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 rcp_sqrt(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_invsqrt_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 cbrt(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_cbrt_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 rcp_cbrt(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_invcbrt_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 sin(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_sin_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 cos(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_cos_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 tan(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_tan_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 asin(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_asin_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 acos(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_acos_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 atan(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_atan_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 atan(const simd::fvec4& y, const simd::fvec4& x) noexcept
	{
		return simd::fvec4{_mm_atan2_ps(y.m_data, x.m_data)};
	}

	[[nodiscard]] inline float angle(const simd::fvec4& from, const simd::fvec4& to) noexcept
	{
		return acos(clamp(dot(from, to), -1.0f, 1.0f));
	}

	[[nodiscard]] inline simd::fvec4 exp(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_exp_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 expm1(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_expm1_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 exp2(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_exp2_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 exp10(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_exp10_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 log(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_log_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 log2(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_log2_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 log10(const simd::fvec4& v) noexcept
	{
		return simd::fvec4{_mm_log10_ps(v.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 pow(const simd::fvec4& base, const simd::fvec4& exponent) noexcept
	{
		return simd::fvec4{_mm_pow_ps(base.m_data, exponent.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 pow(const simd::fvec4& base, float exponent) noexcept
	{
		return simd::fvec4{_mm_pow_ps(base.m_data, _mm_set_ps1(exponent))};
	}

	[[nodiscard]] inline simd::fvec4 pow(float base, const simd::fvec4& exponent) noexcept
	{
		return simd::fvec4{_mm_pow_ps(_mm_set_ps1(base), exponent.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 mod(const simd::fvec4& lhs, const simd::fvec4& rhs) noexcept
	{
		return simd::fvec4{_mm_fmod_ps(lhs.m_data, rhs.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 mod(const simd::fvec4& lhs, float rhs) noexcept
	{
		return simd::fvec4{_mm_fmod_ps(lhs.m_data, _mm_set_ps1(rhs))};
	}

	[[nodiscard]] inline simd::fvec4 mod(float lhs, const simd::fvec4& rhs) noexcept
	{
		return simd::fvec4{_mm_fmod_ps(_mm_set_ps1(lhs), rhs.m_data)};
	}

	[[nodiscard]] inline simd::fvec4 lerp(const simd::fvec4& a, const simd::fvec4& b, const simd::fvec4& t) noexcept
	{
		// Imprecise:
		// return simd::fvec4{_mm_fmadd_ps(_mm_sub_ps(b.m_data, a.m_data), t.m_data, a.m_data)};

		// Precise:
		return simd::fvec4{_mm_fmadd_ps(a.m_data, _mm_sub_ps(_mm_set_ps1(1.0f), t.m_data), _mm_mul_ps(b.m_data, t.m_data))};
	}

	[[nodiscard]] inline simd::fvec4 lerp(const simd::fvec4& a, const simd::fvec4& b, float t) noexcept
	{
		// Imprecise:
		// return simd::fvec4{_mm_fmadd_ps(_mm_sub_ps(b.m_data, a.m_data), _mm_set_ps1(t), a.m_data)};

		// Precise:
		return simd::fvec4{_mm_fmadd_ps(a.m_data, _mm_set_ps1(1.0f - t), _mm_mul_ps(b.m_data, _mm_set_ps1(t)))};
	}
}
