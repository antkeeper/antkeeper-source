// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "test.hpp"
#include <engine/math/math.hpp>
#include <engine/math/simd/simd.hpp>
#include <print>

using namespace engine::math::simd::types;
using namespace engine::math::constants;
using namespace engine::math::functions;
using namespace engine::sized_types;

template <std::floating_point T>
void assert_near(T a, T b, T tolerance = 1e-6f)
{
	const auto d = std::fabs(b - a);
	if (d > tolerance || std::isnan(a) != std::isnan(b))
	{
		throw std::runtime_error("Assertion failed: a ~= b.");
	}
}

template <class T>
void assert_eq(const T& a, const T& b)
{
	if (!(a == b))
	{
		throw std::runtime_error("Assertion failed: a == b.");
	}
}

template <class T>
void assert_ne(const T& a, const T& b)
{
	if (!(a != b))
	{
		throw std::runtime_error("Assertion failed: a != b.");
	}
}

template <class T, usize N>
void assert_vec_near(const vector<T, N>& a, const vector<T, N>& b, T tolerance = 1e-6f)
{
	for (usize i = 0; i < N; ++i)
	{
		const auto d = std::fabs(b[i] - a[i]);
		if (d > tolerance || std::isnan(a[i]) != std::isnan(b[i]))
		{
			throw std::runtime_error("Assertion failed: vectors are not approximately equal.");
		}
	}
}

int main(int, char*[])
{
	if (!engine::math::simd::has_sse42())
	{
		std::println("SSE4.2 not supported. Skipping SIMD test.");
		return 0;
	}

	test_suite suite;

	suite.tests.emplace_back("simd::fvec4 load/store", [ ]()
	{
		fvec4 a{};

		alignas(fvec4::alignment) const float aligned_in[4] = {1.0f, 2.0f, 3.0f, 4.0f};
		a.load(aligned_in);
		assert_vec_near(a, {1.0f, 2.0f, 3.0f, 4.0f});

		alignas(fvec4::alignment) float aligned_out[4] = {};
		a.store(aligned_out);
		assert_near(aligned_out[0], 1.0f);
		assert_near(aligned_out[1], 2.0f);
		assert_near(aligned_out[2], 3.0f);
		assert_near(aligned_out[3], 4.0f);

		const float unaligned_in[4] = {5.0f, 6.0f, 7.0f, 8.0f};
		a.load_unaligned(unaligned_in);
		assert_vec_near(a, {5.0f, 6.0f, 7.0f, 8.0f});

		float unaligned_out[4] = {};
		a.store_unaligned(unaligned_out);
		assert_near(unaligned_out[0], 5.0f);
		assert_near(unaligned_out[1], 6.0f);
		assert_near(unaligned_out[2], 7.0f);
		assert_near(unaligned_out[3], 8.0f);
	});

	suite.tests.emplace_back("simd::fvec3 load/store", []()
	{
		fvec3 a{};

		alignas(fvec3::alignment) const float aligned_in[3] = {1.0f, 2.0f, 3.0f};
		a.load(aligned_in);
		assert_vec_near(a, {1.0f, 2.0f, 3.0f});

		alignas(fvec3::alignment) float aligned_out[3] = {};
		a.store(aligned_out);
		assert_near(aligned_out[0], 1.0f);
		assert_near(aligned_out[1], 2.0f);
		assert_near(aligned_out[2], 3.0f);

		const float unaligned_in[3] = {5.0f, 6.0f, 7.0f};
		a.load_unaligned(unaligned_in);
		assert_vec_near(a, {5.0f, 6.0f, 7.0f});

		float unaligned_out[3] = {};
		a.store_unaligned(unaligned_out);
		assert_near(unaligned_out[0], 5.0f);
		assert_near(unaligned_out[1], 6.0f);
		assert_near(unaligned_out[2], 7.0f);
	});

	suite.tests.emplace_back("simd::fvec4 initialization", [ ]()
	{
		const fvec4 a{};
		const fvec4 b = {7.0f, -3.0f, 200.0f, 0.333f};
		const fvec4 c{1.33, 5u, -3, 2.0f};
		const fvec4 d = fvec4(8.0f);

		assert_vec_near(a, {0.0f, 0.0f, 0.0f, 0.0f});
		assert_vec_near(b, {7.0f, -3.0f, 200.0f, 0.333f});
		assert_vec_near(c, {1.33f, 5.0f, -3.0f, 2.0f});
		assert_vec_near(d, {8.0f, 8.0f, 8.0f, 8.0f});
	});

	suite.tests.emplace_back("simd::fvec3 initialization", []()
	{
		const fvec3 a{};
		const fvec3 b = {7.0f, -3.0f, 200.0f};
		const fvec3 c{1.33, 5u, -3};
		const fvec3 d = fvec3(8.0f);

		assert_vec_near(a, {0.0f, 0.0f, 0.0f});
		assert_vec_near(b, {7.0f, -3.0f, 200.0f});
		assert_vec_near(c, {1.33f, 5.0f, -3.0f});
		assert_vec_near(d, {8.0f, 8.0f, 8.0f});
	});

	suite.tests.emplace_back("simd::fvec4 comparison operators", [ ]()
	{
		const fvec4 a{1.0f, 2.0f, 3.0f, 4.0f};
		const fvec4 b{1.0f, 2.0f, 3.0f, 4.0f};
		const fvec4 c{4.0f, 3.0f, 2.0f, 1.0f};

		assert_eq(a, a);
		assert_eq(a, b);
		assert_eq(b, a);
		assert_ne(a, c);
		assert_ne(c, a);
	});

	suite.tests.emplace_back("simd::fvec3 comparison operators", [ ]()
	{
		const fvec3 a{1.0f, 2.0f, 3.0f};
		const fvec3 b{1.0f, 2.0f, 3.0f};
		const fvec3 c{4.0f, 3.0f, 2.0f};

		assert_eq(a, a);
		assert_eq(a, b);
		assert_eq(b, a);
		assert_ne(a, c);
		assert_ne(c, a);

		fvec3 d = {0.0f, 1.0f, 2.0f};
		assert_ne(d, a);
		++d;
		assert_eq(d, a);
	});

	suite.tests.emplace_back("simd::fvec4 increment/decrement operators", [ ]()
	{
		const fvec4 a{1.0f, 2.0f, 3.0f, 4.0f};
		const fvec4 b{4.0f, 3.0f, 2.0f, 1.0f};

		fvec4 c = a;
		fvec4 d = ++c;
		assert_vec_near(c, {2.0f, 3.0f, 4.0f, 5.0f});
		assert_vec_near(d, {2.0f, 3.0f, 4.0f, 5.0f});

		c = a;
		d = c++;
		assert_vec_near(c, {2.0f, 3.0f, 4.0f, 5.0f});
		assert_vec_near(d, {1.0f, 2.0f, 3.0f, 4.0f});

		c = a;
		d = --c;
		assert_vec_near(c, {0.0f, 1.0f, 2.0f, 3.0f});
		assert_vec_near(d, {0.0f, 1.0f, 2.0f, 3.0f});

		c = a;
		d = c--;
		assert_vec_near(c, {0.0f, 1.0f, 2.0f, 3.0f});
		assert_vec_near(d, {1.0f, 2.0f, 3.0f, 4.0f});
	});

	suite.tests.emplace_back("simd::fvec3 increment/decrement operators", [ ]()
	{
		const fvec3 a{1.0f, 2.0f, 3.0f};
		const fvec3 b{4.0f, 3.0f, 2.0f};

		fvec3 c = a;
		fvec3 d = ++c;
		assert_vec_near(c, {2.0f, 3.0f, 4.0f});
		assert_vec_near(d, {2.0f, 3.0f, 4.0f});

		c = a;
		d = c++;
		assert_vec_near(c, {2.0f, 3.0f, 4.0f});
		assert_vec_near(d, {1.0f, 2.0f, 3.0f});

		c = a;
		d = --c;
		assert_vec_near(c, {0.0f, 1.0f, 2.0f});
		assert_vec_near(d, {0.0f, 1.0f, 2.0f});

		c = a;
		d = c--;
		assert_vec_near(c, {0.0f, 1.0f, 2.0f});
		assert_vec_near(d, {1.0f, 2.0f, 3.0f});
	});

	suite.tests.emplace_back("simd::fvec4 arithmetic operators", [ ]()
	{
		const fvec4 a{1.0f, 2.0f, 3.0f, 4.0f};
		const fvec4 b{4.0f, 3.0f, 2.0f, 1.0f};

		assert_vec_near(a + b, {5.0f, 5.0f, 5.0f, 5.0f});
		assert_vec_near(a + 7.5f, {8.5f, 9.5f, 10.5f, 11.5f});
		assert_vec_near(-a, {-1.0f, -2.0f, -3.0f, -4.0f});
		assert_vec_near(a - b, {-3.0f, -1.0f, 1.0f, 3.0f});
		assert_vec_near(a - 3.5f, {-2.5f, -1.5f, -0.5f, 0.5f});
		assert_vec_near(a* b, {4.0f, 6.0f, 6.0f, 4.0f});
		assert_vec_near(a * 2.0f, {2.0f, 4.0f, 6.0f, 8.0f});
		assert_vec_near(a / b, {0.25f, 2.0f / 3.0f, 1.5f, 4.0f});
		assert_vec_near(a / 2.0f, {0.5f, 1.0f, 1.5f, 2.0f});
	});

	suite.tests.emplace_back("simd::fvec3 arithmetic operators", []()
	{
		const fvec3 a{1.0f, 2.0f, 3.0f};
		const fvec3 b{4.0f, 3.0f, 2.0f};

		assert_vec_near(a + b, {5.0f, 5.0f, 5.0f});
		assert_vec_near(a + 7.5f, {8.5f, 9.5f, 10.5f});
		assert_vec_near(-a, {-1.0f, -2.0f, -3.0f});
		assert_vec_near(a - b, {-3.0f, -1.0f, 1.0f});
		assert_vec_near(a - 3.5f, {-2.5f, -1.5f, -0.5f});
		assert_vec_near(a * b, {4.0f, 6.0f, 6.0f});
		assert_vec_near(a * 2.0f, {2.0f, 4.0f, 6.0f});
		assert_vec_near(a / b, {0.25f, 2.0f / 3.0f, 1.5f});
		assert_vec_near(a / 2.0f, {0.5f, 1.0f, 1.5f});
	});

	suite.tests.emplace_back("simd::fvec4 compound assignment operators", [ ]()
	{
		const fvec4 a{1.0f, 2.0f, 3.0f, 4.0f};
		const fvec4 b{4.0f, 3.0f, 2.0f, 1.0f};

		fvec4 c = a;
		c += b;
		assert_vec_near(c, {5.0f, 5.0f, 5.0f, 5.0f});

		c = a;
		c += 2.0f;
		assert_vec_near(c, {3.0f, 4.0f, 5.0f, 6.0f});

		c = a;
		c -= b;
		assert_vec_near(c, {-3.0f, -1.0f, 1.0f, 3.0f});

		c = a;
		c -= 1.5f;
		assert_vec_near(c, {-0.5f, 0.5f, 1.5f, 2.5f});

		c = a;
		c *= b;
		assert_vec_near(c, {4.0f, 6.0f, 6.0f, 4.0f});

		c = a;
		c *= 3.0f;
		assert_vec_near(c, {3.0f, 6.0f, 9.0f, 12.0f});

		c = a;
		c /= b;
		assert_vec_near(c, {0.25f, 2.0f / 3.0f, 1.5f, 4.0f});

		c = a;
		c /= 2.0f;
		assert_vec_near(c, {0.5f, 1.0f, 1.5f, 2.0f});
	});

	suite.tests.emplace_back("simd::fvec3 compound assignment operators", [ ]()
	{
		const fvec3 a{1.0f, 2.0f, 3.0f};
		const fvec3 b{4.0f, 3.0f, 2.0f};

		fvec3 c = a;
		c += b;
		assert_vec_near(c, {5.0f, 5.0f, 5.0f});

		c = a;
		c += 2.0f;
		assert_vec_near(c, {3.0f, 4.0f, 5.0f});

		c = a;
		c -= b;
		assert_vec_near(c, {-3.0f, -1.0f, 1.0f});

		c = a;
		c -= 1.5f;
		assert_vec_near(c, {-0.5f, 0.5f, 1.5f});

		c = a;
		c *= b;
		assert_vec_near(c, {4.0f, 6.0f, 6.0f});

		c = a;
		c *= 3.0f;
		assert_vec_near(c, {3.0f, 6.0f, 9.0f});

		c = a;
		c /= b;
		assert_vec_near(c, {0.25f, 2.0f / 3.0f, 1.5f});

		c = a;
		c /= 2.0f;
		assert_vec_near(c, {0.5f, 1.0f, 1.5f});
	});

	suite.tests.emplace_back("simd::fvec3 cross product", []()
	{
		const fvec3 a{1.0f, 0.0f, 0.0f};
		const fvec3 b{0.0f, 1.0f, 0.0f};

		assert_vec_near(cross(a, b), {0.0f, 0.0f, 1.0f});
		assert_vec_near(cross(b, a), {0.0f, 0.0f, -1.0f});
		assert_vec_near(cross(a, a), {0.0f, 0.0f, 0.0f});
		assert_vec_near(cross(a, zero<fvec3>), {0.0f, 0.0f, 0.0f});
	});

	suite.tests.emplace_back("simd::fvec3 triple product", []()
	{
		const fvec3 a{1.0f, 2.0f, 3.0f};
		const fvec3 b{4.0f, 5.0f, 6.0f};
		const fvec3 c{7.0f, 8.0f, 9.0f};
		const fvec3 d{1.0f, 0.0f, 0.0f};
		const fvec3 e{0.0f, 1.0f, 0.0f};
		const fvec3 f{0.0f, 0.0f, 1.0f};

		assert_near(triple(a, b, c), 0.0f);
		assert_near(triple(d, e, f), 1.0f);
		assert_near(triple(e, f, d), 1.0f);
		assert_near(triple(f, d, e), 1.0f);
		assert_near(triple(d, f, e), -1.0f);
		assert_near(triple(f, e, d), -1.0f);
		assert_near(triple(e, d, f), -1.0f);
	});

	suite.tests.emplace_back("simd::fvec4 dot/length/normalize/distance", [ ]()
	{
		const fvec4 a{1.0f, 2.0f, 3.0f, 4.0f};
		const fvec4 b{4.0f, 3.0f, 2.0f, 1.0f};

		assert_near(dot(a, b), 20.0f);
		assert_near(sqr_length(a), 30.0f);
		assert_near(length(a), sqrt(30.0f));
		assert_near(rcp_length(a), 1.0f / sqrt(30.0f));
		assert_near(length(normalize(a)), 1.0f);
		assert_near(sqr_distance(a, b), sqr_length(a - b));
		assert_near(distance(a, b), length(a - b));
	});

	suite.tests.emplace_back("simd::fvec3 dot/length/normalize/distance", []()
	{
		const fvec3 a{2.0f, 3.0f, 4.0f};
		const fvec3 b{3.0f, 2.0f, 1.0f};

		assert_near(dot(a, b), 16.0f);
		assert_near(sqr_length(a), 29.0f);
		assert_near(length(a), sqrt(29.0f));
		assert_near(rcp_length(a), 1.0f / sqrt(29.0f));
		assert_near(length(normalize(a)), 1.0f);
		assert_near(sqr_distance(a, b), sqr_length(a - b));
		assert_near(distance(a, b), length(a - b));
	});

	suite.tests.emplace_back("simd::fvec4 sign functions", [ ]()
	{
		const fvec4 a{-1.0f, 2.0f, -3.0f, 4.0f};
		const fvec4 b{4.1f, 5.2f, 6.3f, 7.4f};

		assert_vec_near(abs(a), {1.0f, 2.0f, 3.0f, 4.0f});
		assert_vec_near(copysign(b, a), {-4.1f, 5.2f, -6.3f, 7.4f});
		assert_vec_near(copysign(5.0f, a), {-5.0f, 5.0f, -5.0f, 5.0f});
		assert_vec_near(sign(a), {-1.0f, 1.0f, -1.0f, 1.0f});
	});

	suite.tests.emplace_back("simd::fvec3 sign functions", []()
	{
		const fvec3 a{-1.0f, 2.0f, -3.0f};
		const fvec3 b{4.1f, 5.2f, 6.3f};

		assert_vec_near(abs(a), {1.0f, 2.0f, 3.0f});
		assert_vec_near(copysign(b, a), {-4.1f, 5.2f, -6.3f});
		assert_vec_near(copysign(5.0f, a), {-5.0f, 5.0f, -5.0f});
		assert_vec_near(sign(a), {-1.0f, 1.0f, -1.0f});
	});

	suite.tests.emplace_back("simd::fvec4 min/max/clamp", [ ]()
	{
		const fvec4 a{1.0f, 5.0f, 3.0f, 7.0f};
		const fvec4 b{2.0f, 4.0f, 6.0f, 0.0f};

		assert_vec_near(min(a, b), {1.0f, 4.0f, 3.0f, 0.0f});
		assert_vec_near(max(a, b), {2.0f, 5.0f, 6.0f, 7.0f});
		assert_vec_near(clamp(a, b, fvec4(6.0f)), {2.0f, 5.0f, 6.0f, 6.0f});
		assert_vec_near(clamp(a, 2.0f, 6.0f), {2.0f, 5.0f, 3.0f, 6.0f});
		assert_near(length(clamp_length(a, 3.0f)), 3.0f);
		assert_vec_near(clamp_length(a, 3.0f) * (length(a) / 3.0f), a);
	});

	suite.tests.emplace_back("simd::fvec3 min/max/clamp", []()
	{
		const fvec3 a{1.0f, 5.0f, 3.0f};
		const fvec3 b{2.0f, 4.0f, 6.0f};

		assert_vec_near(min(a, b), {1.0f, 4.0f, 3.0f});
		assert_vec_near(max(a, b), {2.0f, 5.0f, 6.0f});
		assert_vec_near(clamp(a, b, fvec3(6.0f)), {2.0f, 5.0f, 6.0f});
		assert_vec_near(clamp(a, 2.0f, 6.0f), {2.0f, 5.0f, 3.0f});
		assert_near(length(clamp_length(a, 3.0f)), 3.0f);
		assert_vec_near(clamp_length(a, 3.0f)* (length(a) / 3.0f), a);
	});

	suite.tests.emplace_back("simd::fvec4 sum/min_element/max_element", [ ]()
	{
		const fvec4 a{1.0f, 2.0f, 3.0f, 4.0f};
		const fvec4 b{4.0f, 1.0f, 2.0f, 3.0f};
		const fvec4 c{3.0f, 4.0f, 1.0f, 2.0f};
		const fvec4 d{2.0f, 3.0f, 4.0f, 1.0f};

		assert_near(sum(a), 10.0f);
		assert_near(min_element(a), 1.0f);
		assert_near(min_element(b), 1.0f);
		assert_near(min_element(c), 1.0f);
		assert_near(min_element(d), 1.0f);
		assert_near(max_element(a), 4.0f);
		assert_near(max_element(b), 4.0f);
		assert_near(max_element(c), 4.0f);
		assert_near(max_element(d), 4.0f);
	});

	suite.tests.emplace_back("simd::fvec3 sum/min_element/max_element", []()
	{
		const fvec3 a{1.0f, 2.0f, 3.0f};
		const fvec3 b{3.0f, 1.0f, 2.0f};
		const fvec3 c{2.0f, 3.0f, 1.0f};

		fvec3 d = a;
		++d;
		assert_near(sum(d), 9.0f);

		assert_near(min_element(a), 1.0f);
		assert_near(min_element(b), 1.0f);
		assert_near(min_element(c), 1.0f);
		assert_near(max_element(a), 3.0f);
		assert_near(max_element(b), 3.0f);
		assert_near(max_element(c), 3.0f);
	});

	suite.tests.emplace_back("simd::fvec4 rounding functions", [ ]()
	{
		const fvec4 a{1.1f, 2.5f, 3.5f, -4.2f};

		assert_vec_near(floor(a), {1.0f, 2.0f, 3.0f, -5.0f});
		assert_vec_near(ceil(a), {2.0f, 3.0f, 4.0f, -4.0f});
		assert_vec_near(round(a), {1.0f, 3.0f, 4.0f, -4.0f});
		assert_vec_near(roundeven(a), {1.0f, 2.0f, 4.0f, -4.0f});
		assert_vec_near(fract(a), {0.1f, 0.5f, 0.5f, 0.8f});
		assert_vec_near(trunc(a), {1.0f, 2.0f, 3.0f, -4.0f});
	});

	suite.tests.emplace_back("simd::fvec3 rounding functions", []()
	{
		const fvec3 a{1.1f, 2.5f, -3.5f};

		assert_vec_near(floor(a), {1.0f, 2.0f, -4.0f});
		assert_vec_near(ceil(a), {2.0f, 3.0f, -3.0f});
		assert_vec_near(round(a), {1.0f, 3.0f, -4.0f});
		assert_vec_near(roundeven(a), {1.0f, 2.0f, -4.0f});
		assert_vec_near(fract(a), {0.1f, 0.5f, 0.5f});
		assert_vec_near(trunc(a), {1.0f, 2.0f, -3.0f});
	});

	suite.tests.emplace_back("simd::fvec4 rcp/fma/fms/fnma/fnms", [ ]()
	{
		const fvec4 a{2.0f, 4.0f, 8.0f, 16.0f};
		const fvec4 b{1.0f, 2.0f, 3.0f, 4.0f};
		const fvec4 c{0.5f, 1.0f, 1.5f, 2.0f};

		assert_vec_near(rcp(a), {0.5f, 0.25f, 0.125f, 0.0625f});
		assert_vec_near(fma(a, b, c), {2.5f, 9.0f, 25.5f, 66.0f});
		assert_vec_near(fms(a, b, c), {1.5f, 7.0f, 22.5f, 62.0f});
		assert_vec_near(fnma(a, b, c), {-1.5f, -7.0f, -22.5f, -62.0f});
		assert_vec_near(fnms(a, b, c), {-2.5f, -9.0f, -25.5f, -66.0f});
	});

	suite.tests.emplace_back("simd::fvec3 rcp/fma/fms/fnma/fnms", []()
	{
		const fvec3 a{2.0f, 4.0f, 8.0f};
		const fvec3 b{1.0f, 2.0f, 3.0f};
		const fvec3 c{0.5f, 1.0f, 1.5f};

		assert_vec_near(rcp(a), {0.5f, 0.25f, 0.125f});
		assert_vec_near(fma(a, b, c), {2.5f, 9.0f, 25.5f});
		assert_vec_near(fms(a, b, c), {1.5f, 7.0f, 22.5f});
		assert_vec_near(fnma(a, b, c), {-1.5f, -7.0f, -22.5f});
		assert_vec_near(fnms(a, b, c), {-2.5f, -9.0f, -25.5f});
	});

	suite.tests.emplace_back("simd::fvec4 sqr/cube/sqrt/rcp_sqrt/cbrt/rcp_cbrt", [ ]()
	{
		const fvec4 a{2.0f, 4.0f, 8.0f, 16.0f};

		assert_vec_near(sqr(a), {4.0f, 16.0f, 64.0f, 256.0f});
		assert_vec_near(sqrt(a), {sqrt(2.0f), 2.0f, sqrt(8.0f), 4.0f});
		assert_vec_near(rcp_sqrt(a), {rcp(sqrt(2.0f)), 0.5f, rcp(sqrt(8.0f)), 0.25f});
		assert_vec_near(cube(a), {8.0f, 64.0f, 512.0f, 4096.0f});
		assert_vec_near(cbrt(a), {cbrt(2.0f), cbrt(4.0f), cbrt(8.0f), cbrt(16.0f)});
		assert_vec_near(rcp_cbrt(a), {rcp(cbrt(2.0f)), rcp(cbrt(4.0f)), 0.5f, rcp(cbrt(16.0f))});
	});

	suite.tests.emplace_back("simd::fvec3 sqr/cube/sqrt/rcp_sqrt/cbrt/rcp_cbrt", []()
	{
		const fvec3 a{2.0f, 4.0f, 8.0f};

		assert_vec_near(sqr(a), {4.0f, 16.0f, 64.0f});
		assert_vec_near(sqrt(a), {sqrt(2.0f), 2.0f, sqrt(8.0f)});
		assert_vec_near(rcp_sqrt(a), {rcp(sqrt(2.0f)), 0.5f, rcp(sqrt(8.0f))});
		assert_vec_near(cube(a), {8.0f, 64.0f, 512.0f});
		assert_vec_near(cbrt(a), {cbrt(2.0f), cbrt(4.0f), cbrt(8.0f)});
		assert_vec_near(rcp_cbrt(a), {rcp(cbrt(2.0f)), rcp(cbrt(4.0f)), 0.5f});
	});

	suite.tests.emplace_back("simd::fvec4 trig/exp/log/pow/mod", [ ]()
	{
		const fvec4 a{0.0f, 0.5f, 1.0f, -1.0f};
		const fvec4 b{1.0f, 2.0f, 3.0f, 0.0f};

		assert_vec_near(sin(a), {0.0f, sin(0.5f), sin(1.0f), -sin(1.0f)});
		assert_vec_near(cos(a), {1.0f, cos(0.5f), cos(1.0f), cos(1.0f)});
		assert_vec_near(tan(a), {0.0f, tan(0.5f), tan(1.0f), -tan(1.0f)});
		assert_vec_near(asin(a), {0.0f, asin(0.5f), half_pi<float>, -half_pi<float>});
		assert_vec_near(acos(a), {half_pi<float>, acos(0.5f), 0.0f, pi<float>});
		assert_vec_near(atan(a), {0.0f, atan(0.5f), atan(1.0f), -atan(1.0f)});
		assert_vec_near(atan(b, a), {half_pi<float>, atan(4.0f), atan(3.0f), pi<float>});
		assert_vec_near(exp(a), {1.0f, exp(0.5f), exp(1.0f), exp(-1.0f)});
		assert_vec_near(expm1(a), {0.0f, expm1(0.5f), expm1(1.0f), expm1(-1.0f)});
		assert_vec_near(exp2(a), {1.0f, exp2(0.5f), 2.0f, 0.5f});
		assert_vec_near(exp10(a), {1.0f, exp10(0.5f), 10.0f, 0.1f});
		assert_vec_near(log(a + 2.0f), {log(2.0f), log(2.5f), log(3.0f), 0.0f});
		assert_vec_near(log2(a + 2.0f), {1.0f, log2(2.5f), log2(3.0f), 0.0f});
		assert_vec_near(log10(a + 2.0f), {log10(2.0f), log10(2.5f), log10(3.0f), 0.0f});
		assert_vec_near(pow(a, b), {0.0f, 0.25f, 1.0f, 1.0f});
		assert_vec_near(pow(a + 1.0f, 2.0f), {1.0f, 2.25f, 4.0f, 0.0f});
		assert_vec_near(pow(3.0f, a), {1.0f, sqrt_3<float>, 3.0f, 1.0f / 3.0f});
		assert_vec_near(mod(a, b + 1.0f), {fmod(0.0f, 2.0f), fmod(0.5f, 3.0f), fmod(1.0f, 4.0f), fmod(-1.0f, 1.0f)});
		assert_vec_near(mod(a + 1.0f, 1.5f), {fmod(1.0f, 1.5f), fmod(1.5f, 1.5f), fmod(2.0f, 1.5f), fmod(0.0f, 1.5f)});
		assert_vec_near(mod(6.0f, b + 1.0f), {fmod(6.0f, 2.0f), fmod(6.0f, 3.0f), fmod(6.0f, 4.0f), fmod(6.0f, 1.0f)});
	});

	suite.tests.emplace_back("simd::fvec3 trig/exp/log/pow/mod", []()
	{
		const fvec3 a{0.0f, 0.5f, 1.0f};
		const fvec3 b{1.0f, 2.0f, 3.0f};

		assert_vec_near(sin(a), {0.0f, sin(0.5f), sin(1.0f)});
		assert_vec_near(cos(a), {1.0f, cos(0.5f), cos(1.0f)});
		assert_vec_near(tan(a), {0.0f, tan(0.5f), tan(1.0f)});
		assert_vec_near(asin(a), {0.0f, asin(0.5f), half_pi<float>});
		assert_vec_near(acos(a), {half_pi<float>, acos(0.5f), 0.0f});
		assert_vec_near(atan(a), {0.0f, atan(0.5f), atan(1.0f)});
		assert_vec_near(atan(b, a), {half_pi<float>, atan(4.0f), atan(3.0f)});
		assert_vec_near(exp(a), {1.0f, exp(0.5f), exp(1.0f)});
		assert_vec_near(expm1(a), {0.0f, expm1(0.5f), expm1(1.0f)});
		assert_vec_near(exp2(a), {1.0f, exp2(0.5f), 2.0f});
		assert_vec_near(exp10(a), {1.0f, exp10(0.5f), 10.0f});
		assert_vec_near(log(a + 2.0f), {log(2.0f), log(2.5f), log(3.0f)});
		assert_vec_near(log2(a + 2.0f), {1.0f, log2(2.5f), log2(3.0f)});
		assert_vec_near(log10(a + 2.0f), {log10(2.0f), log10(2.5f), log10(3.0f)});
		assert_vec_near(pow(a, b), {0.0f, 0.25f, 1.0f});
		assert_vec_near(pow(a + 1.0f, 2.0f), {1.0f, 2.25f, 4.0f});
		assert_vec_near(pow(3.0f, a), {1.0f, sqrt_3<float>, 3.0f});
		assert_vec_near(mod(a, b + 1.0f), {fmod(0.0f, 2.0f), fmod(0.5f, 3.0f), fmod(1.0f, 4.0f)});
		assert_vec_near(mod(a + 1.0f, 1.5f), {fmod(1.0f, 1.5f), fmod(1.5f, 1.5f), fmod(2.0f, 1.5f)});
		assert_vec_near(mod(6.0f, b + 1.0f), {fmod(6.0f, 2.0f), fmod(6.0f, 3.0f), fmod(6.0f, 4.0f)});
	});

	suite.tests.emplace_back("simd::fvec4 angle", [ ]()
	{
		const fvec4 a{1.0f, 0.0f, 0.0f, 0.0f};
		const fvec4 b{0.0f, 1.0f, 0.0f, 0.0f};

		assert_near(angle(a, b), half_pi<float>);
	});

	suite.tests.emplace_back("simd::fvec3 angle", []()
	{
		const fvec3 a{1.0f, 0.0f, 0.0f};
		const fvec3 b{0.0f, 1.0f, 0.0f};

		assert_near(angle(a, b), half_pi<float>);
	});

	suite.tests.emplace_back("simd::fvec4 lerp", []()
	{
		const fvec4 a{0.0f, 1.0f, 2.0f, 3.0f};
		const fvec4 b{10.0f, 11.0f, 12.0f, 13.0f};
		const fvec4 t{0.0f, 0.5f, 1.0f, 0.25f};

		assert_vec_near(lerp(a, b, t), {0.0f, 6.0f, 12.0f, 5.5f});
		assert_vec_near(lerp(a, b, 0.25f), {2.5f, 3.5f, 4.5f, 5.5f});
	});

	suite.tests.emplace_back("simd::fvec3 lerp", []()
	{
		const fvec3 a{0.0f, 1.0f, 2.0f};
		const fvec3 b{10.0f, 11.0f, 12.0f};
		const fvec3 t{0.0f, 0.5f, 1.0f};

		assert_vec_near(lerp(a, b, t), {0.0f, 6.0f, 12.0f});
		assert_vec_near(lerp(a, b, 0.25f), {2.5f, 3.5f, 4.5f});
	});

	return suite.run();
}