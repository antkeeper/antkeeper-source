// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/simd/fvec3.hpp>
#include <engine/math/simd/fvec4.hpp>
#include <engine/utility/sized-types.hpp>
#include <engine/utility/alignment.hpp>
#include <engine/debug/contract.hpp>
#include <nmmintrin.h>

namespace engine::math::simd
{
	/// @name Vector construction
	/// @{

	template <usize N> requires (N == 3 || N == 4)
		[[nodiscard]] inline fvec<N> make_vector(float value) noexcept
	{
		return fvec<N>{_mm_set_ps1(value)};
	}

	[[nodiscard]] inline fvec3 make_vector(float a, float b, float c) noexcept
	{
		return fvec3{_mm_setr_ps(a, b, c, 0.0f)};
	}

	[[nodiscard]] inline fvec4 make_vector(float a, float b, float c, float d) noexcept
	{
		return fvec4{_mm_setr_ps(a, b, c, d)};
	}

	/// @}
}

namespace engine::math::simd::inline types
{
	/// @name Vector comparison operators
	/// @{

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline bool operator==(const fvec<N>& lhs, const fvec<N>& rhs) noexcept
	{
		return (_mm_movemask_ps(_mm_cmpeq_ps(lhs.m_data, rhs.m_data)) & fvec<N>::zero_mask) == fvec<N>::zero_mask;
	}

	/// @}

	/// @name Vector increment/decrement operators
	/// @{

	/// Increments all elements of a vector.
	/// @param[in,out] v Vector to increment.
	/// @return Reference to the incremented vector.
	template <usize N> requires (N == 3 || N == 4)
	inline fvec<N>& operator++(fvec<N>& v) noexcept
	{
		v.m_data = _mm_add_ps(v.m_data, _mm_set_ps1(1.0f));
		return v;
	}

	/// Increments all elements of a vector.
	/// @param[in,out] v Vector to increment.
	/// @return Copy of @p v before it was incremented.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline fvec<N> operator++(fvec<N>& v, int) noexcept
	{
		auto temp = v;
		v.m_data = _mm_add_ps(v.m_data, _mm_set_ps1(1.0f));
		return temp;
	}

	/// Decrements all elements of a vector.
	/// @param[in,out] v Vector to decrement.
	/// @return Reference to the decremented vector.
	template <usize N> requires (N == 3 || N == 4)
		inline fvec<N>& operator--(fvec<N>& v) noexcept
	{
		v.m_data = _mm_sub_ps(v.m_data, _mm_set_ps1(1.0f));
		return v;
	}

	/// Decrements all elements of a vector.
	/// @param[in,out] v Vector to decrement.
	/// @return Copy of @p v before it was decremented.
	template <usize N> requires (N == 3 || N == 4)
		[[nodiscard]] inline fvec<N> operator--(fvec<N>& v, int) noexcept
	{
		auto temp = v;
		v.m_data = _mm_sub_ps(v.m_data, _mm_set_ps1(1.0f));
		return temp;
	}

	/// @}

	/// @name Vector arithmetic operators
	/// @{

	/// Adds two vectors.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Vector on the right-hand side.
	/// @return Result of the addition.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline fvec<N> operator+(const fvec<N>& lhs, const fvec<N>& rhs) noexcept
	{
		return fvec<N>{_mm_add_ps(lhs.m_data, rhs.m_data)};
	}
	
	/// Adds a scalar to all elements of a vector.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Scalar on the right-hand side.
	/// @return Result of the addition.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline fvec<N> operator+(const fvec<N>& lhs, float rhs) noexcept
	{
		return fvec<N>{_mm_add_ps(lhs.m_data, _mm_set_ps1(rhs))};
	}

	/// Adds a scalar to all elements of a vector.
	/// @param lhs Scalar on the left-hand side.
	/// @param rhs Vector on the right-hand side.
	/// @return Result of the addition.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline fvec<N> operator+(float lhs, const fvec<N>& rhs) noexcept
	{
		return fvec<N>{_mm_add_ps(_mm_set_ps1(lhs), rhs.m_data)};
	}

	/// Negates a vector.
	/// @param[in] v Vector to negate.
	/// @return Negated copy of the vector.
	template <usize N> requires (N == 3 || N == 4)
		[[nodiscard]] inline fvec<N> operator-(const fvec<N>& v) noexcept
	{
		static const auto mask = _mm_set_ps1(-0.0f);
		return fvec<N>{_mm_xor_ps(v.m_data, mask)};
	}

	/// Subtracts two vectors.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Vector on the right-hand side.
	/// @return Result of the subtraction.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline fvec<N> operator-(const fvec<N>& lhs, const fvec<N>& rhs) noexcept
	{
		return fvec<N>{_mm_sub_ps(lhs.m_data, rhs.m_data)};
	}

	/// Subtracts a scalar from all elements of a vector.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Scalar on the right-hand side.
	/// @return Result of the subtraction.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline fvec<N> operator-(const fvec<N>& lhs, float rhs) noexcept
	{
		return fvec<N>{_mm_sub_ps(lhs.m_data, _mm_set_ps1(rhs))};
	}

	/// Subtracts each element of a vector from a scalar.
	/// @param lhs Scalar on the left-hand side.
	/// @param rhs Vector on the right-hand side.
	/// @return Result of the subtraction.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline fvec<N> operator-(float lhs, const fvec<N>& rhs) noexcept
	{
		return fvec<N>{_mm_sub_ps(_mm_set_ps1(lhs), rhs.m_data)};
	}

	/// Multiplies the elements of two vectors.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Vector on the right-hand side.
	/// @return Result of the multiplication.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline fvec<N> operator*(const fvec<N>& lhs, const fvec<N>& rhs) noexcept
	{
		return fvec<N>{_mm_mul_ps(lhs.m_data, rhs.m_data)};
	}

	/// Multiplies each element of a vector by a scalar.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Scalar on the right-hand side.
	/// @return Result of the multiplication.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline fvec<N> operator*(const fvec<N>& lhs, float rhs) noexcept
	{
		return fvec<N>{_mm_mul_ps(lhs.m_data, _mm_set_ps1(rhs))};
	}

	/// Multiplies each element of a vector by a scalar.
	/// @param lhs Scalar on the left-hand side.
	/// @param rhs Vector on the right-hand side.
	/// @return Result of the multiplication.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline fvec<N> operator*(float lhs, const fvec<N>& rhs) noexcept
	{
		return fvec<N>{_mm_mul_ps(_mm_set_ps1(lhs), rhs.m_data)};
	}

	/// Divides the elements of one vector by the elements of another vector.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Vector on the right-hand side.
	/// @return Result of the division.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline fvec<N> operator/(const fvec<N>& lhs, const fvec<N>& rhs) noexcept
	{
		return fvec<N>{_mm_div_ps(lhs.m_data, rhs.m_data)};
	}

	/// Divides each element of a vector by a scalar.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Scalar on the right-hand side.
	/// @return Result of the division.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline fvec<N> operator/(const fvec<N>& lhs, float rhs) noexcept
	{
		return fvec<N>{_mm_div_ps(lhs.m_data, _mm_set_ps1(rhs))};
	}

	/// Divides a scalar by each element of a vector.
	/// @param lhs Scalar on the left-hand side.
	/// @param rhs Vector on the right-hand side.
	/// @return Result of the division.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline fvec<N> operator/(float lhs, const fvec<N>& rhs) noexcept
	{
		return fvec<N>{_mm_div_ps(_mm_set_ps1(lhs), rhs.m_data)};
	}

	/// @}

	/// @name Vector compound assignment operators
	/// @{

	/// Adds the elements of two vectors and stores the result in the first vector.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Vector on the right-hand side.
	/// @return Reference to the vector on the left-hand side.
	template <usize N> requires (N == 3 || N == 4)
	inline fvec<N>& operator+=(fvec<N>& lhs, const fvec<N>& rhs) noexcept
	{
		lhs.m_data = _mm_add_ps(lhs.m_data, rhs.m_data);
		return lhs;
	}

	/// Adds a scalar to each element of a vector and stores the result in the vector.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Scalar on the right-hand side.
	/// @return Reference to the vector on the left-hand side.
	template <usize N> requires (N == 3 || N == 4)
	inline fvec<N>& operator+=(fvec<N>& lhs, float rhs) noexcept
	{
		lhs.m_data = _mm_add_ps(lhs.m_data, _mm_set_ps1(rhs));
		return lhs;
	}

	/// Subtracts the elements of two vectors and stores the result in the first vector.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Vector on the right-hand side.
	/// @return Reference to the vector on the left-hand side.
	template <usize N> requires (N == 3 || N == 4)
	inline fvec<N>& operator-=(fvec<N>& lhs, const fvec<N>& rhs) noexcept
	{
		lhs.m_data = _mm_sub_ps(lhs.m_data, rhs.m_data);
		return lhs;
	}

	/// Subtracts a scalar from each element of a vector and stores the result in the vector.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Scalar on the right-hand side.
	/// @return Reference to the vector on the left-hand side.
	template <usize N> requires (N == 3 || N == 4)
	inline fvec<N>& operator-=(fvec<N>& lhs, float rhs) noexcept
	{
		lhs.m_data = _mm_sub_ps(lhs.m_data, _mm_set_ps1(rhs));
		return lhs;
	}

	/// Multiplies the elements of two vectors and stores the result in the first vector.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Vector on the right-hand side.
	/// @return Reference to the vector on the left-hand side.
	template <usize N> requires (N == 3 || N == 4)
	inline fvec<N>& operator*=(fvec<N>& lhs, const fvec<N>& rhs) noexcept
	{
		lhs.m_data = _mm_mul_ps(lhs.m_data, rhs.m_data);
		return lhs;
	}

	/// Multiplies each element of a vector by a scalar and stores the result in the vector.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Scalar on the right-hand side.
	/// @return Reference to the vector on the left-hand side.
	template <usize N> requires (N == 3 || N == 4)
	inline fvec<N>& operator*=(fvec<N>& lhs, float rhs) noexcept
	{
		lhs.m_data = _mm_mul_ps(lhs.m_data, _mm_set_ps1(rhs));
		return lhs;
	}

	/// Divides the elements of one vector by the elements of another vector and stores the result in the first vector.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Vector on the right-hand side.
	/// @return Reference to the vector on the left-hand side.
	template <usize N> requires (N == 3 || N == 4)
	inline fvec<N>& operator/=(fvec<N>& lhs, const fvec<N>& rhs) noexcept
	{
		lhs.m_data = _mm_div_ps(lhs.m_data, rhs.m_data);
		return lhs;
	}

	/// Divides each element of a vector by a scalar and stores the result in the vector.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Scalar on the right-hand side.
	/// @return Reference to the vector on the left-hand side.
	template <usize N> requires (N == 3 || N == 4)
	inline fvec<N>& operator/=(fvec<N>& lhs, float rhs) noexcept
	{
		lhs.m_data = _mm_div_ps(lhs.m_data, _mm_set_ps1(rhs));
		return lhs;
	}

	/// @ 
}

namespace engine::math::inline functions
{
	/// @name Vector algebra
	/// @{

	/// Calculates the dot product of two vectors.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Vector on the right-hand side.
	/// @return Dot product of the two vectors.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline float dot(const simd::fvec<N>& lhs, const simd::fvec<N>& rhs) noexcept
	{
		return _mm_cvtss_f32(_mm_dp_ps(lhs.m_data, rhs.m_data, simd::fvec<N>::dot_mask));
	}

	/// Calculates the cross product of two 3-dimensional vectors.
	/// @param lhs Vector on the left-hand side.
	/// @param rhs Vector on the right-hand side.
	/// @return Cross product of the two vectors.
	[[nodiscard]] inline simd::fvec3 cross(const simd::fvec3& lhs, const simd::fvec3& rhs) noexcept
	{
		const auto lhs_yzx = _mm_shuffle_ps(lhs.m_data, lhs.m_data, _MM_SHUFFLE(3, 0, 2, 1));
		const auto lhs_zxy = _mm_shuffle_ps(lhs.m_data, lhs.m_data, _MM_SHUFFLE(3, 1, 0, 2));
		const auto rhs_zxy = _mm_shuffle_ps(rhs.m_data, rhs.m_data, _MM_SHUFFLE(3, 1, 0, 2));
		const auto rhs_yzx = _mm_shuffle_ps(rhs.m_data, rhs.m_data, _MM_SHUFFLE(3, 0, 2, 1));
		return simd::fvec3{_mm_sub_ps(_mm_mul_ps(lhs_yzx, rhs_zxy), _mm_mul_ps(lhs_zxy, rhs_yzx))};
	}

	/// Calculates the scalar triple product of three 3-dimensional vectors.
	/// @param x First vector.
	/// @param y Second vector.
	/// @param z Third vector.
	/// @return Scalar triple product of the three vectors.
	template <class T>
	[[nodiscard]] inline T triple(const simd::vec3<T>& x, const simd::vec3<T>& y, const simd::vec3<T>& z) noexcept
	{
		return dot(x, cross(y, z));
	}

	/// Calculates the squared length of a vector.
	/// @param v Vector whose squared length is to be calculated.
	/// @return Squared length of the vector.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline float sqr_length(const simd::fvec<N>& v) noexcept
	{
		return _mm_cvtss_f32(_mm_dp_ps(v.m_data, v.m_data, simd::fvec<N>::dot_mask));
	}

	/// Calculates the length of a vector.
	/// @param v Vector whose length is to be calculated.
	/// @return Length of the vector.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline float length(const simd::fvec<N>& v) noexcept
	{
		return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(v.m_data, v.m_data, simd::fvec<N>::dot_mask)));
	}

	/// Normalizes a vector.
	/// @param v Vector to normalize.
	/// @return Normalized copy of the vector.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> normalize(const simd::fvec<N>& v) noexcept
	{
		const auto length = _mm_sqrt_ss(_mm_dp_ps(v.m_data, v.m_data, simd::fvec<N>::dot_mask));
		return simd::fvec<N>{_mm_div_ps(v.m_data, _mm_shuffle_ps(length, length, 0u))};
	}

	/// Calculates the squared Cartesian distance between two points.
	/// @param a First point.
	/// @param b Second point.
	/// @return Squared Cartesian distance between the two points.
	template <class T, usize N>
	[[nodiscard]] inline T sqr_distance(const simd::vec<T, N>& a, const simd::vec<T, N>& b) noexcept
	{
		return sqr_length(b - a);
	}

	/// Calculates the Cartesian between two points.
	/// @param a First point.
	/// @param b Second point.
	/// @return Cartesian distance between the two points.
	template <class T, usize N>
	[[nodiscard]] inline T distance(const simd::vec<T, N>& a, const simd::vec<T, N>& b) noexcept
	{
		return length(b - a);
	}

	/// @}

	/// @name Vector projection
	/// @{

	/// Projects one vector onto another.
	/// @param a Vector to be projected.
	/// @param b Vector onto which to project.
	/// @return Projection of @p a onto @p b.
	template <class T, usize N>
	[[nodiscard]] inline vec<T, N> project(const simd::vec<T, N>& a, const simd::vec<T, N>& b) noexcept
	{
		const T sqr_len_b = sqr_length(b);
		return (sqr_len_b > T{0}) ? b * (dot(a, b) / sqr_len_b) : vec<T, N>{};
	}

	/// @}

	/// @name Vector sign functions
	/// @{

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> abs(const simd::fvec<N>& v) noexcept
	{
		static const auto mask = _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff));
		return simd::fvec<N>{_mm_and_ps(v.m_data, mask)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> copysign(float magnitude, const simd::fvec<N>& v) noexcept
	{
		const auto sign_bits = _mm_and_ps(v.m_data, _mm_set_ps1(-0.0f));
		const auto abs_magnitude = _mm_and_ps(_mm_set_ps1(magnitude), _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff)));
		return simd::fvec<N>{_mm_or_ps(abs_magnitude, sign_bits)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> copysign(const simd::fvec<N>& magnitude, const simd::fvec<N>& v) noexcept
	{
		const auto sign_bits = _mm_and_ps(v.m_data, _mm_set_ps1(-0.0f));
		const auto abs_magnitude = _mm_and_ps(magnitude.m_data, _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff)));
		return simd::fvec<N>{_mm_or_ps(abs_magnitude, sign_bits)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> sign(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_or_ps(_mm_and_ps(v.m_data, _mm_set_ps1(-0.0f)), _mm_set_ps1(1.0f))};
	}

	/// @}

	/// @name Vector min/max functions
	/// @{

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> min(const simd::fvec<N>& lhs, const simd::fvec<N>& rhs) noexcept
	{
		return simd::fvec<N>{_mm_min_ps(lhs.m_data, rhs.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> max(const simd::fvec<N>& lhs, const simd::fvec<N>& rhs) noexcept
	{
		return simd::fvec<N>{_mm_max_ps(lhs.m_data, rhs.m_data)};
	}

	[[nodiscard]] inline float min_element(const simd::fvec3& v) noexcept
	{
		const auto min1 = _mm_min_ps(v.m_data, _mm_shuffle_ps(v.m_data, v.m_data, _MM_SHUFFLE(3, 0, 2, 1)));
		const auto min2 = _mm_min_ps(min1, _mm_shuffle_ps(min1, min1, _MM_SHUFFLE(3, 0, 2, 1)));
		return _mm_cvtss_f32(min2);
	}

	[[nodiscard]] inline float min_element(const simd::fvec4& v) noexcept
	{
		const auto shuffle = _mm_movehdup_ps(v.m_data);
		const auto min = _mm_min_ps(v.m_data, shuffle);
		return _mm_cvtss_f32(_mm_min_ss(min, _mm_movehl_ps(shuffle, min)));
	}

	[[nodiscard]] inline float max_element(const simd::fvec3& v) noexcept
	{
		const auto min1 = _mm_max_ps(v.m_data, _mm_shuffle_ps(v.m_data, v.m_data, _MM_SHUFFLE(3, 0, 2, 1)));
		const auto min2 = _mm_max_ps(min1, _mm_shuffle_ps(min1, min1, _MM_SHUFFLE(3, 0, 2, 1)));
		return _mm_cvtss_f32(min2);
	}

	[[nodiscard]] inline float max_element(const simd::fvec4& v) noexcept
	{
		const auto shuffle = _mm_movehdup_ps(v.m_data);
		const auto max = _mm_max_ps(v.m_data, shuffle);
		return _mm_cvtss_f32(_mm_max_ss(max, _mm_movehl_ps(shuffle, max)));
	}

	template <usize N>
	[[nodiscard]] inline simd::fvec<N> clamp(const simd::fvec<N>& v, const simd::fvec<N>& min, const simd::fvec<N>& max) noexcept
	{
		return math::min(math::max(v, min), max);
	}

	template <usize N> requires (N == 3 || N == 4)
		[[nodiscard]] inline simd::fvec<N> clamp(const simd::fvec<N>& v, float min, float max) noexcept
	{
		return simd::fvec<N>{_mm_min_ps(_mm_max_ps(v.m_data, _mm_set_ps1(min)), _mm_set_ps1(max))};
	}

	template <usize N>
	[[nodiscard]] inline simd::fvec<N> clamp_length(const simd::fvec<N>& v, float max_length) noexcept
	{
		const auto sqr_l = sqr_length(v);
		return (sqr_l > sqr(max_length)) ? (v * (max_length / sqrt(sqr_l))) : v;
	}

	/// @}

	/// @name Vector summation functions
	/// @{

	[[nodiscard]] inline float sum(const simd::fvec3& v) noexcept
	{
		const auto xyz = _mm_blend_ps(v.m_data, _mm_setzero_ps(), 0b1000);
		const auto shuffle = _mm_movehdup_ps(xyz);
		const auto sums = _mm_add_ps(xyz, shuffle);
		return _mm_cvtss_f32(_mm_add_ss(sums, _mm_movehl_ps(shuffle, sums)));
	}

	[[nodiscard]] inline float sum(const simd::fvec4& v) noexcept
	{
		const auto shuffle = _mm_movehdup_ps(v.m_data);
		const auto sums = _mm_add_ps(v.m_data, shuffle);
		return _mm_cvtss_f32(_mm_add_ss(sums, _mm_movehl_ps(shuffle, sums)));
	}

	/// @}

	/// @name Vector rounding functions
	/// @{

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> floor(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_floor_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> ceil(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_ceil_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] simd::fvec<N> round(const simd::fvec<N>& v) noexcept
	{
		const auto sign_bits = _mm_and_ps(v.m_data, _mm_set_ps1(-0.0f));
		const auto sign = _mm_or_ps(sign_bits, _mm_set_ps1(1.0f));
		const auto i = _mm_trunc_ps(v.m_data);
		const auto abs_f = _mm_and_ps(_mm_sub_ps(v.m_data, i), _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff)));
		const auto ge_half = _mm_cmpge_ps(abs_f, _mm_set_ps1(0.5f));
		return simd::fvec<N>{_mm_add_ps(i, _mm_or_ps(_mm_and_ps(ge_half, sign), sign_bits))};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> roundeven(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_round_ps(v.m_data, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> fract(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_sub_ps(v.m_data, _mm_floor_ps(v.m_data))};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> trunc(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_trunc_ps(v.m_data)};
	}

	/// @}

	/// @name Vector fused operations
	/// @{

	/// Performs a fused multiply-add operation on three vectors.
	/// @param a First multiplicand.
	/// @param b Second multiplicand.
	/// @param c Addend.
	/// @return Result of the operation `a * b + c`.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> fma(const simd::fvec<N>& a, const simd::fvec<N>& b, const simd::fvec<N>& c) noexcept
	{
		return simd::fvec<N>{_mm_fmadd_ps(a.m_data, b.m_data, c.m_data)};
	}

	/// Performs a fused multiply-add operation on a vector and two scalars.
	/// @param a First multiplicand vector.
	/// @param b Second multiplicand scalar.
	/// @param c Addend scalar.
	/// @return Result of the operation `a * b + c`.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> fma(const simd::fvec<N>& a, float b, float c) noexcept
	{
		return simd::fvec<N>{_mm_fmadd_ps(a.m_data, _mm_set_ps1(b), _mm_set_ps1(c))};
	}

	/// Performs a fused multiply-subtract operation on three vectors.
	/// @param a First multiplicand.
	/// @param b Second multiplicand.
	/// @param c Subtrahend.
	/// @return Result of the operation `a * b - c`.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> fms(const simd::fvec<N>& a, const simd::fvec<N>& b, const simd::fvec<N>& c) noexcept
	{
		return simd::fvec<N>{_mm_fmsub_ps(a.m_data, b.m_data, c.m_data)};
	}

	/// Performs a fused multiply-subtract operation on a vector and two scalars.
	/// @param a First multiplicand vector.
	/// @param b Second multiplicand scalar.
	/// @param c Subtrahend scalar.
	/// @return Result of the operation `a * b - c`.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> fms(const simd::fvec<N>& a, float b, float c) noexcept
	{
		return simd::fvec<N>{_mm_fmsub_ps(a.m_data, _mm_set_ps1(b), _mm_set_ps1(c))};
	}

	/// Performs a fused negative multiply-add operation on three vectors.
	/// @param a First multiplicand.
	/// @param b Second multiplicand.
	/// @param c Addend.
	/// @return Result of the operation `-(a * b) + c`.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> fnma(const simd::fvec<N>& a, const simd::fvec<N>& b, const simd::fvec<N>& c) noexcept
	{
		return simd::fvec<N>{_mm_fnmadd_ps(a.m_data, b.m_data, c.m_data)};
	}

	/// Performs a fused negative multiply-add operation on a vector and two scalars.
	/// @param a First multiplicand vector.
	/// @param b Second multiplicand scalar.
	/// @param c Addend scalar.
	/// @return Result of the operation `-(a * b) + c`.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> fnma(const simd::fvec<N>& a, float b, float c) noexcept
	{
		return simd::fvec<N>{_mm_fnmadd_ps(a.m_data, _mm_set_ps1(b), _mm_set_ps1(c))};
	}

	/// Performs a fused negative multiply-subtract operation on three vectors.
	/// @param a First multiplicand.
	/// @param b Second multiplicand.
	/// @param c Subtrahend.
	/// @return Result of the operation `-(a * b) - c`.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> fnms(const simd::fvec<N>& a, const simd::fvec<N>& b, const simd::fvec<N>& c) noexcept
	{
		return simd::fvec<N>{_mm_fnmsub_ps(a.m_data, b.m_data, c.m_data)};
	}

	/// Performs a fused negative multiply-subtract operation on a vector and two scalars.
	/// @param a First multiplicand vector.
	/// @param b Second multiplicand scalar.
	/// @param c Subtrahend scalar.
	/// @return Result of the operation `-(a * b) - c`.
	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> fnms(const simd::fvec<N>& a, float b, float c) noexcept
	{
		return simd::fvec<N>{_mm_fnmsub_ps(a.m_data, _mm_set_ps1(b), _mm_set_ps1(c))};
	}

	/// @}

	/// @name Vector reciprocal functions
	/// @{

	template <usize N>
	[[nodiscard]] inline simd::fvec<N> rcp(const simd::fvec<N>& v) noexcept
	{
		return 1.0f / v;
	}

	template <usize N>
	[[nodiscard]] inline float rcp_length(const simd::fvec<N>& v) noexcept
	{
		return rcp(length(v));
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> rcp_sqrt(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_invsqrt_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> rcp_cbrt(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_invcbrt_ps(v.m_data)};
	}

	/// @}

	/// @name Vector power/root functions
	/// @{

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> sqr(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_mul_ps(v.m_data, v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> cube(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_mul_ps(_mm_mul_ps(v.m_data, v.m_data), v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> sqrt(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_sqrt_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> cbrt(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_cbrt_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> pow(const simd::fvec<N>& base, const simd::fvec<N>& exponent) noexcept
	{
		return simd::fvec<N>{_mm_pow_ps(base.m_data, exponent.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> pow(const simd::fvec<N>& base, float exponent) noexcept
	{
		return simd::fvec<N>{_mm_pow_ps(base.m_data, _mm_set_ps1(exponent))};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> pow(float base, const simd::fvec<N>& exponent) noexcept
	{
		return simd::fvec<N>{_mm_pow_ps(_mm_set_ps1(base), exponent.m_data)};
	}

	/// @}

	/// @name Vector trigonometric functions
	/// @{

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> sin(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_sin_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> cos(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_cos_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> tan(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_tan_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> asin(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_asin_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> acos(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_acos_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> atan(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_atan_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> atan(const simd::fvec<N>& y, const simd::fvec<N>& x) noexcept
	{
		return simd::fvec<N>{_mm_atan2_ps(y.m_data, x.m_data)};
	}

	/// Calculates the angle between two vectors.
	/// @param from First direction vector.
	/// @param to Second direction vector.
	/// @return Angle, in radians, between the two vectors.
	template <class T, usize N>
	[[nodiscard]] inline float angle(const simd::vec<T, N>& from, const simd::vec<T, N>& to) noexcept
	{
		return acos(clamp(dot(from, to), T{-1}, T{1}));
	}

	/// Calculates the signed angle between two 3-dimensional vectors about an axis.
	/// @param from First direction vector.
	/// @param to Second direction vector.
	/// @param axis Axis about which to measure the angle.
	/// @return Signed angle, in radians, between the two vectors about the given axis.
	template <class T>
	[[nodiscard]] inline T signed_angle(const simd::vec3<T>& from, const simd::vec3<T>& to, const simd::vec3<T>& axis) noexcept
	{
		return atan(triple(axis, from, to), dot(from, to));
	}

	/// @}

	/// @name Vector exponential/logarithmic functions
	/// @{

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> exp(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_exp_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> expm1(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_expm1_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> exp2(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_exp2_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> exp10(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_exp10_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> log(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_log_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> log2(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_log2_ps(v.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> log10(const simd::fvec<N>& v) noexcept
	{
		return simd::fvec<N>{_mm_log10_ps(v.m_data)};
	}

	/// @}

	/// @name Vector modulo functions
	/// @{

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> mod(const simd::fvec<N>& lhs, const simd::fvec<N>& rhs) noexcept
	{
		return simd::fvec<N>{_mm_fmod_ps(lhs.m_data, rhs.m_data)};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> mod(const simd::fvec<N>& lhs, float rhs) noexcept
	{
		return simd::fvec<N>{_mm_fmod_ps(lhs.m_data, _mm_set_ps1(rhs))};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> mod(float lhs, const simd::fvec<N>& rhs) noexcept
	{
		return simd::fvec<N>{_mm_fmod_ps(_mm_set_ps1(lhs), rhs.m_data)};
	}

	/// @}

	/// @name Vector interpolation
	/// @{

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> lerp(const simd::fvec<N>& a, const simd::fvec<N>& b, const simd::fvec<N>& t) noexcept
	{
		// Imprecise:
		// return simd::fvec<N>{_mm_fmadd_ps(_mm_sub_ps(b.m_data, a.m_data), t.m_data, a.m_data)};

		// Precise:
		return simd::fvec<N>{_mm_fmadd_ps(a.m_data, _mm_sub_ps(_mm_set_ps1(1.0f), t.m_data), _mm_mul_ps(b.m_data, t.m_data))};
	}

	template <usize N> requires (N == 3 || N == 4)
	[[nodiscard]] inline simd::fvec<N> lerp(const simd::fvec<N>& a, const simd::fvec<N>& b, float t) noexcept
	{
		// Imprecise:
		// return simd::fvec<N>{_mm_fmadd_ps(_mm_sub_ps(b.m_data, a.m_data), _mm_set_ps1(t), a.m_data)};

		// Precise:
		return simd::fvec<N>{_mm_fmadd_ps(a.m_data, _mm_set_ps1(1.0f - t), _mm_mul_ps(b.m_data, _mm_set_ps1(t)))};
	}

	/// @}
}
