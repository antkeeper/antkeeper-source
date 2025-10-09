// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "test.hpp"
#include <engine/math/math.hpp>

using namespace engine::math;

int main(int, char*[])
{
	test_suite suite;

	suite.tests.emplace_back("vector initialization", []()
	{
		ivec3 a{};
		ASSERT_EQ(a.x(), 0);
		ASSERT_EQ(a.y(), 0);
		ASSERT_EQ(a.z(), 0);

		ivec3 b{1, 2, 3};
		ASSERT_EQ(b.x(), 1);
		ASSERT_EQ(b.y(), 2);
		ASSERT_EQ(b.z(), 3);
	});

	suite.tests.emplace_back("vector conversion", []()
	{
		ivec2 a{-1, 200};
		ivec4 b{600, 700, 800, 900};
		fvec3 c{0.5f, 156.3f, -333.0f};

		// Size cast
		ivec4 d = ivec4(a);
		ivec3 e = ivec3(ivec2(ivec4(a)));
		ivec4 f = ivec4(ivec2(ivec3(b)));

		ASSERT_EQ(d[0], -1);
		ASSERT_EQ(d[1], 200);
		ASSERT_EQ(d[2], 0);
		ASSERT_EQ(d[3], 0);

		ASSERT_EQ(e[0], -1);
		ASSERT_EQ(e[1], 200);
		ASSERT_EQ(e[2], 0);

		ASSERT_EQ(f[0], 600);
		ASSERT_EQ(f[1], 700);
		ASSERT_EQ(f[2], 0);
		ASSERT_EQ(f[3], 0);

		// Type cast
		uvec2 g = uvec2(a);
		ivec3 h = ivec3(c);

		ASSERT_EQ(g[0], std::numeric_limits<unsigned int>::max());
		ASSERT_EQ(g[1], 200);
		ASSERT_EQ(h[0], 0);
		ASSERT_EQ(h[1], 156);
		ASSERT_EQ(h[2], -333);
	});

	suite.tests.emplace_back("vector element access", []()
	{
		ivec3 a{999, -7, 12};

		ASSERT_EQ(a[0], 999);
		ASSERT_EQ(a[1], -7);
		ASSERT_EQ(a[2], 12);

		ASSERT_EQ(a.front(), 999);
		ASSERT_EQ(a.back(), 12);

		ASSERT_EQ(*a.data(), 999);
		ASSERT_EQ(*(a.data() + 1), -7);
		ASSERT_EQ(*(a.data() + 2), 12);

		ASSERT_EQ(a.x(), 999);
		ASSERT_EQ(a.y(), -7);
		ASSERT_EQ(a.z(), 12);

		a[0] = -2048;
		a[1] = -123;
		a[2] = 360;

		ASSERT_EQ(a.x(), -2048);
		ASSERT_EQ(a.y(), -123);
		ASSERT_EQ(a.z(), 360);

		a.x() = 485;
		a.y() = -47;
		a.z() = 32;

		ASSERT_EQ(a[0], 485);
		ASSERT_EQ(a[1], -47);
		ASSERT_EQ(a[2], 32);

		a.front() = 80;
		a.back() = 3;

		ASSERT_EQ(a[0], 80);
		ASSERT_EQ(a[1], -47);
		ASSERT_EQ(a[2], 3);
	});

	suite.tests.emplace_back("vector capacity", []()
	{
		ivec2 v2;
		ivec3 v3;
		ivec4 v4;

		ASSERT(!v2.empty());
		ASSERT(!v3.empty());
		ASSERT(!v4.empty());

		STATIC_ASSERT_EQ(v2.size(), 2);
		STATIC_ASSERT_EQ(v3.size(), 3);
		STATIC_ASSERT_EQ(v4.size(), 4);

		STATIC_ASSERT_EQ(v2.max_size(), 2);
		STATIC_ASSERT_EQ(v3.max_size(), 3);
		STATIC_ASSERT_EQ(v4.max_size(), 4);
	});

	suite.tests.emplace_back("vector iterators", []()
	{
		ivec4 a{97, -3746, 3, -834};

		ASSERT_NE(a.begin(), a.end());
		ASSERT_LT(a.begin(), a.end());
		ASSERT_GT(a.end(), a.begin());
		ASSERT_EQ(a.begin(), a.data());
		ASSERT_EQ(a.end(), a.data() + 4);

		auto it = a.begin();
		ASSERT_EQ(it, a.begin());
		ASSERT_EQ(it, a.data());
		ASSERT_EQ(*it, 97);

		++it;
		ASSERT_EQ(*it, -3746);

		--it;
		ASSERT_EQ(*it, 97);

		it += 3;
		ASSERT_EQ(*it, -834);

		it -= 1;
		ASSERT_EQ(*it, 3);

		it += 2;
		ASSERT_EQ(it, a.end());

		ASSERT_NE(a.rbegin(), a.rend());

		auto rit = a.rbegin();
		ASSERT_EQ(*rit, -834);

		++rit;
		ASSERT_EQ(*rit, 3);

		rit += 2;
		ASSERT_EQ(*rit, 97);

		--rit;
		ASSERT_EQ(*rit, -3746);

		rit += 2;
		ASSERT_EQ(rit, a.rend());
	});

	suite.tests.emplace_back("vector operations", []()
	{
		ivec3 a;
		ivec3 b;

		a.fill(8);
		b.fill(-500);

		ASSERT_EQ(a.x(), 8);
		ASSERT_EQ(a.y(), 8);
		ASSERT_EQ(a.z(), 8);
		ASSERT_EQ(b.x(), -500);
		ASSERT_EQ(b.y(), -500);
		ASSERT_EQ(b.z(), -500);

		a.swap(b);

		ASSERT_EQ(a.x(), -500);
		ASSERT_EQ(a.y(), -500);
		ASSERT_EQ(a.z(), -500);
		ASSERT_EQ(b.x(), 8);
		ASSERT_EQ(b.y(), 8);
		ASSERT_EQ(b.z(), 8);
	});

	suite.tests.emplace_back("vector comparison", []()
	{
		ivec3 a{1, 2, 3};
		ivec3 b{1, 2, 4};
		ivec3 c{1, 2, 3};

		ASSERT_EQ(a, c);
		ASSERT_NE(a, b);
		ASSERT_LT(a, b);
		ASSERT_LE(a, b);
		ASSERT_LE(a, c);
		ASSERT_GT(b, a);
		ASSERT_GE(b, a);
		ASSERT_GE(a, c);
	});

	suite.tests.emplace_back("vector tuple-like interface", []()
	{
		ivec3 a{1, 2, 3};

		auto [x, y, z] = a;

		ASSERT_EQ(x, 1);
		ASSERT_EQ(y, 2);
		ASSERT_EQ(z, 3);

		ASSERT_EQ(get<0>(a), 1);
		ASSERT_EQ(get<1>(a), 2);
		ASSERT_EQ(get<2>(a), 3);

		auto& [rx, ry, rz] = a;
		rx = 945;
		ry = 62;
		rz = -3025;

		ASSERT_EQ(a.x(), 945);
		ASSERT_EQ(a.y(), 62);
		ASSERT_EQ(a.z(), -3025);

		ASSERT_NE(x, rx);
		ASSERT_NE(y, ry);
		ASSERT_NE(z, rz);
	});

	suite.tests.emplace_back("vector formatter", []()
	{
		ivec3 a{152, 333, -4000};
		fvec4 b{-0.32f, 0.0f, 2.0f / 3.0f, std::numeric_limits<float>::infinity()};

		auto str = std::format("{}", a);
		ASSERT_EQ(str, "{152, 333, -4000}");

		str = std::format("{:x}", a);
		ASSERT_EQ(str, "{98, 14d, -fa0}");

		str = std::format("{:.3f}", b);
		ASSERT_EQ(str, "{-0.320, 0.000, 0.667, inf}");
	});

	suite.tests.emplace_back("matrix initialization", []()
	{
		ivec3 a0{1, 2, 3};
		ivec3 a1{4, 5, 6};
		ivec3 a2{7, 8, 9};
		imat3 a{a0, a1, a2};

		ivec2 b0{1, 2};
		ivec2 b1{3, 4};
		ivec2 b2{5, 6};
		imat3x2 b{b0, b1, b2};

		imat3x2 c{1, 2, 3, 4, 5, 6};

		ASSERT_EQ(a[0], a0);
		ASSERT_EQ(a[1], a1);
		ASSERT_EQ(a[2], a2);

		ASSERT_EQ(b[0], b0);
		ASSERT_EQ(b[1], b1);
		ASSERT_EQ(b[2], b2);

		ASSERT_EQ(c[0], b0);
		ASSERT_EQ(c[1], b1);
		ASSERT_EQ(c[2], b2);
	});

	suite.tests.emplace_back("matrix conversion", []()
	{
		imat2 a{9, -1, 674, 1000};

		// Size cast
		imat3x4 b = imat3x4(a);
		ASSERT_EQ(b[0][0], 9);
		ASSERT_EQ(b[0][1], -1);
		ASSERT_EQ(b[0][2], 0);
		ASSERT_EQ(b[0][3], 0);
		ASSERT_EQ(b[1][0], 674);
		ASSERT_EQ(b[1][1], 1000);
		ASSERT_EQ(b[1][2], 0);
		ASSERT_EQ(b[1][3], 0);
		ASSERT_EQ(b[2][0], 0);
		ASSERT_EQ(b[2][1], 0);
		ASSERT_EQ(b[2][2], 1);
		ASSERT_EQ(b[2][3], 0);

		imat2x3 c = imat2x3(b);
		ASSERT_EQ(c[0][0], 9);
		ASSERT_EQ(c[0][1], -1);
		ASSERT_EQ(c[0][2], 0);
		ASSERT_EQ(c[1][0], 674);
		ASSERT_EQ(c[1][1], 1000);
		ASSERT_EQ(c[1][2], 0);

		// Type cast
		umat2 d = umat2(a);
		ASSERT_EQ(d[0][0], 9);
		ASSERT_EQ(d[0][1], std::numeric_limits<unsigned int>::max());
		ASSERT_EQ(d[1][0], 674);
		ASSERT_EQ(d[1][1], 1000);
	});

	suite.tests.emplace_back("matrix column access", []()
	{
		ivec2 a0{90, 239};
		ivec2 a1{12, -4};
		ivec2 a2{0, 9876};
		imat3x2 a{a0, a1, a2};

		ASSERT_EQ(a[0], a0);
		ASSERT_EQ(a[1], a1);
		ASSERT_EQ(a[2], a2);
		ASSERT_EQ(a[0], a.column(0));
		ASSERT_EQ(a[1], a.column(1));
		ASSERT_EQ(a[2], a.column(2));
		ASSERT_EQ(a.front(), a0);
		ASSERT_EQ(a.back(), a2);
		ASSERT_EQ(&a[0], a.data());

		a.front() = ivec2{200, -7};
		a[1] = ivec2{33, 11};
		a.column(2) = a0;
		(*(a.data() + 1))[1] = 22;

		ASSERT_EQ(a[0][1], -7);
		ASSERT_EQ(a[1][0], 33);
		ASSERT_EQ(a[1][1], 22);
		ASSERT_EQ(a[2], a0);
	});

	suite.tests.emplace_back("matrix element access", []()
	{
		imat2x3 a{80, 3, -102, 45, 677, 99};
		imat4x4 b{10, 23, 9578, 3209, 3094, 29, -193, 37, 395, 0, 42, 40, 4985, 23908, 20, 12};

		ASSERT_EQ(a.element(0), a[0][0]);
		ASSERT_EQ(a.element(1), a[0][1]);
		ASSERT_EQ(a.element(5), a[1][2]);

		ASSERT_EQ(a.element(0), 80);
		ASSERT_EQ(a.element(1), 3);
		ASSERT_EQ(a.element(2), -102);
		ASSERT_EQ(a.element(3), 45);
		ASSERT_EQ(a.element(4), 677);
		ASSERT_EQ(a.element(5), 99);

		ASSERT_EQ(b.element(0), 10);
		ASSERT_EQ(b.element(1), 23);
		ASSERT_EQ(b.element(2), 9578);
		ASSERT_EQ(b.element(3), 3209);
		ASSERT_EQ(b.element(4), 3094);
		ASSERT_EQ(b.element(5), 29);
		ASSERT_EQ(b.element(6), -193);
		ASSERT_EQ(b.element(7), 37);
		ASSERT_EQ(b.element(8), 395);
		ASSERT_EQ(b.element(9), 0);
		ASSERT_EQ(b.element(10), 42);
		ASSERT_EQ(b.element(11), 40);
		ASSERT_EQ(b.element(12), 4985);
		ASSERT_EQ(b.element(13), 23908);
		ASSERT_EQ(b.element(14), 20);
		ASSERT_EQ(b.element(15), 12);
	});

	suite.tests.emplace_back("matrix iterators", []()
	{
		ivec3 a0{-11, 39, 4985};
		ivec3 a1{391, 980, -2};
		imat2x3 a{a0, a1};

		ASSERT_NE(a.begin(), a.end());
		ASSERT_LT(a.begin(), a.end());
		ASSERT_GT(a.end(), a.begin());
		ASSERT_EQ(a.begin(), a.data());
		ASSERT_EQ(a.end(), a.data() + 2);

		auto it = a.begin();
		ASSERT_EQ(it, a.begin());
		ASSERT_EQ(it, a.data());
		ASSERT_EQ(*it, a0);

		++it;
		ASSERT_EQ(*it, a1);

		--it;
		ASSERT_EQ(*it, a0);

		it += 2;
		ASSERT_EQ(it, a.end());

		ASSERT_NE(a.rbegin(), a.rend());

		auto rit = a.rbegin();
		ASSERT_EQ(*rit, a1);

		++rit;
		ASSERT_EQ(*rit, a0);

		--rit;
		ASSERT_EQ(*rit, a1);

		rit += 2;
		ASSERT_EQ(rit, a.rend());
	});

	suite.tests.emplace_back("matrix capacity", []()
	{
		imat2x3 a{};
		imat3x2 b{};

		ASSERT(!a.empty());
		ASSERT(!b.empty());

		STATIC_ASSERT_EQ(a.size(), 2);
		STATIC_ASSERT_EQ(b.size(), 3);

		STATIC_ASSERT_EQ(a.max_size(), 2);
		STATIC_ASSERT_EQ(b.max_size(), 3);

		STATIC_ASSERT_EQ(a.size_columns(), 2);
		STATIC_ASSERT_EQ(b.size_columns(), 3);

		STATIC_ASSERT_EQ(a.size_rows(), 3);
		STATIC_ASSERT_EQ(b.size_rows(), 2);

		STATIC_ASSERT_EQ(a.size_elements(), 6);
		STATIC_ASSERT_EQ(b.size_elements(), 6);
	});

	suite.tests.emplace_back("matrix operations", []()
	{
		imat2 a;
		imat2 b;

		a.fill(3);
		b.fill(7);

		ASSERT_EQ(a[0][0], 3);
		ASSERT_EQ(a[0][1], 3);
		ASSERT_EQ(a[1][0], 3);
		ASSERT_EQ(a[1][1], 3);

		a.swap(b);

		ASSERT_EQ(a[0][0], 7);
		ASSERT_EQ(a[0][1], 7);
		ASSERT_EQ(a[1][0], 7);
		ASSERT_EQ(a[1][1], 7);
		ASSERT_EQ(b[0][0], 3);
		ASSERT_EQ(b[0][1], 3);
		ASSERT_EQ(b[1][0], 3);
		ASSERT_EQ(b[1][1], 3);
	});

	suite.tests.emplace_back("matrix comparison", []()
	{
		imat2 a{1, 2, 3, 4};
		imat2 b{1, 2, 3, 5};
		imat2 c{1, 2, 3, 4};

		ASSERT_EQ(a, c);
		ASSERT_NE(a, b);
		ASSERT_LT(a, b);
		ASSERT_LE(a, b);
		ASSERT_LE(a, c);
		ASSERT_GT(b, a);
		ASSERT_GE(b, a);
		ASSERT_GE(a, c);
	});

	suite.tests.emplace_back("matrix tuple-like interface", []()
	{
		imat2 a{1, 2, 3, 4};

		auto [a0, a1] = a;

		ASSERT_EQ(a0[0], 1);
		ASSERT_EQ(a0[1], 2);
		ASSERT_EQ(a1[0], 3);
		ASSERT_EQ(a1[1], 4);

		ASSERT_EQ(get<0>(a)[0], 1);
		ASSERT_EQ(get<1>(a)[1], 4);

		auto& [ra0, ra1] = a;
		ra0 = {5, 6};
		ra1 = {7, 8};

		ASSERT_EQ(a[0][1], 6);
		ASSERT_EQ(a[1][0], 7);

		ASSERT_NE(a0, ra0);
		ASSERT_NE(a1, ra1);
	});

	suite.tests.emplace_back("matrix formatter", []()
	{
		imat3x2 a{-1, 2000, 3, 27, 999, -40};
		fmat2x3 b{-0.47f, 0.0f, 2.0f / 3.0f, std::numeric_limits<float>::infinity(), 1000.345678f, -0.0f};

		auto str = std::format("{}", a);
		ASSERT_EQ(str, "{{-1, 2000}, {3, 27}, {999, -40}}");

		str = std::format("{:x}", a);
		ASSERT_EQ(str, "{{-1, 7d0}, {3, 1b}, {3e7, -28}}");

		str = std::format("{:.4f}", b);
		ASSERT_EQ(str, "{{-0.4700, 0.0000, 0.6667}, {inf, 1000.3457, -0.0000}}");
	});

	suite.tests.emplace_back("quaternion initialization", []()
	{
		fquat a{};
		ASSERT_EQ(a.w(), 0.0f);
		ASSERT_EQ(a.x(), 0.0f);
		ASSERT_EQ(a.y(), 0.0f);
		ASSERT_EQ(a.z(), 0.0f);

		fquat b{1.0f, 2.0f, 3.0f, 4.0f};
		ASSERT_EQ(b.w(), 1.0f);
		ASSERT_EQ(b.x(), 2.0f);
		ASSERT_EQ(b.y(), 3.0f);
		ASSERT_EQ(b.z(), 4.0f);

		fquat c{5.0f, {6.0f, 7.0f, 8.0f}};
		ASSERT_EQ(c.w(), 5.0f);
		ASSERT_EQ(c.x(), 6.0f);
		ASSERT_EQ(c.y(), 7.0f);
		ASSERT_EQ(c.z(), 8.0f);

		fvec3 di{-2.0f, -3.0f, -4.0f};
		fquat d{-1.0f, di};
		ASSERT_EQ(d.w(), -1.0f);
		ASSERT_EQ(d.x(), -2.0f);
		ASSERT_EQ(d.y(), -3.0f);
		ASSERT_EQ(d.z(), -4.0f);
	});

	suite.tests.emplace_back("quaternion part access", []()
	{
		fquat a{1.0f, 2.0f, 3.0f, 4.0f};
		ASSERT_EQ(a.w(), a.r);
		ASSERT_EQ(a.x(), a.i.x());
		ASSERT_EQ(a.y(), a.i.y());
		ASSERT_EQ(a.z(), a.i.z());

		a.w() = 5.0f;
		a.x() = 6.0f;
		a.y() = 7.0f;
		a.z() = 8.0f;
		ASSERT_EQ(a.r, 5.0f);
		ASSERT_EQ(a.i.x(), 6.0f);
		ASSERT_EQ(a.i.y(), 7.0f);
		ASSERT_EQ(a.i.z(), 8.0f);
	});

	suite.tests.emplace_back("quaternion conversion", []()
	{
		// Scalar type conversion
		fquat q = fquat(dquat{1.0, 2.0, 3.0, 4.0});
		ASSERT_NEAR(q.w(), 1.0f, 1e-6f);
		ASSERT_NEAR(q.x(), 2.0f, 1e-6f);
		ASSERT_NEAR(q.y(), 3.0f, 1e-6f);
		ASSERT_NEAR(q.z(), 4.0f, 1e-6f);

		// Matrix conversion (identity)
		q = {1.0f, 0.0f, 0.0f, 0.0f};
		fmat3 m = basis_from_quat(q);
		ASSERT_NEAR(m[0][0], 1.0f, 1e-6);
		ASSERT_NEAR(m[0][1], 0.0f, 1e-6);
		ASSERT_NEAR(m[0][2], 0.0f, 1e-6);
		ASSERT_NEAR(m[1][0], 0.0f, 1e-6);
		ASSERT_NEAR(m[1][1], 1.0f, 1e-6);
		ASSERT_NEAR(m[1][2], 0.0f, 1e-6);
		ASSERT_NEAR(m[2][0], 0.0f, 1e-6);
		ASSERT_NEAR(m[2][1], 0.0f, 1e-6);
		ASSERT_NEAR(m[2][2], 1.0f, 1e-6);

		// Matrix conversion (X-axis, 90 degrees)
		q = {cos(pi<float> / 4.0f), sin(pi<float> / 4.0f), 0.0f, 0.0f};
		m = basis_from_quat(q);
		ASSERT_NEAR(m[0][0], 1.0f, 1e-6);
		ASSERT_NEAR(m[0][1], 0.0f, 1e-6);
		ASSERT_NEAR(m[0][2], 0.0f, 1e-6);
		ASSERT_NEAR(m[1][0], 0.0f, 1e-6);
		ASSERT_NEAR(m[1][1], 0.0f, 1e-6);
		ASSERT_NEAR(m[1][2], 1.0f, 1e-6);
		ASSERT_NEAR(m[2][0], 0.0f, 1e-6);
		ASSERT_NEAR(m[2][1], -1.0f, 1e-6);
		ASSERT_NEAR(m[2][2], 0.0f, 1e-6);

		// Matrix conversion (Y-axis, 90 degrees)
		q = {cos(pi<float> / 4.0f), 0.0f, sin(pi<float> / 4.0f), 0.0f};
		m = basis_from_quat(q);
		ASSERT_NEAR(m[0][0], 0.0f, 1e-6);
		ASSERT_NEAR(m[0][1], 0.0f, 1e-6);
		ASSERT_NEAR(m[0][2], -1.0f, 1e-6);
		ASSERT_NEAR(m[1][0], 0.0f, 1e-6);
		ASSERT_NEAR(m[1][1], 1.0f, 1e-6);
		ASSERT_NEAR(m[1][2], 0.0f, 1e-6);
		ASSERT_NEAR(m[2][0], 1.0f, 1e-6);
		ASSERT_NEAR(m[2][1], 0.0f, 1e-6);
		ASSERT_NEAR(m[2][2], 0.0f, 1e-6);

		// Matrix conversion (Z-axis, 90 degrees)
		q = {cos(pi<float> / 4.0f), 0.0f, 0.0f, sin(pi<float> / 4.0f)};
		m = basis_from_quat(q);
		ASSERT_NEAR(m[0][0], 0.0f, 1e-6);
		ASSERT_NEAR(m[0][1], 1.0f, 1e-6);
		ASSERT_NEAR(m[0][2], 0.0f, 1e-6);
		ASSERT_NEAR(m[1][0], -1.0f, 1e-6);
		ASSERT_NEAR(m[1][1], 0.0f, 1e-6);
		ASSERT_NEAR(m[1][2], 0.0f, 1e-6);
		ASSERT_NEAR(m[2][0], 0.0f, 1e-6);
		ASSERT_NEAR(m[2][1], 0.0f, 1e-6);
		ASSERT_NEAR(m[2][2], 1.0f, 1e-6);
	});

	suite.tests.emplace_back("quaternion swap", []()
	{
		fquat a{1.0f, 2.0f, 3.0f, 4.0f};
		fquat b{5.0f, 6.0f, 7.0f, 8.0f};

		a.swap(b);

		ASSERT_EQ(a.w(), 5.0f);
		ASSERT_EQ(a.x(), 6.0f);
		ASSERT_EQ(a.y(), 7.0f);
		ASSERT_EQ(a.z(), 8.0f);
		ASSERT_EQ(b.w(), 1.0f);
		ASSERT_EQ(b.x(), 2.0f);
		ASSERT_EQ(b.y(), 3.0f);
		ASSERT_EQ(b.z(), 4.0f);
	});

	suite.tests.emplace_back("quaternion comparison", []()
	{
		fquat a{1.0f, 2.0f, 3.0f, 4.0f};
		fquat b{1.0f, 2.0f, 3.0f, 5.0f};
		fquat c{1.0f, 2.0f, 3.0f, 4.0f};

		ASSERT_EQ(a, c);
		ASSERT_NE(a, b);
		ASSERT_LT(a, b);
		ASSERT_LE(a, b);
		ASSERT_LE(a, c);
		ASSERT_GT(b, a);
		ASSERT_GE(b, a);
		ASSERT_GE(a, c);
	});

	suite.tests.emplace_back("quaternion tuple-like interface", []()
	{
		fquat q{1.0f, 2.0f, 3.0f, 4.0f};

		auto [r, i] = q;

		ASSERT_EQ(r, 1.0f);
		ASSERT_EQ(i.x(), 2.0f);
		ASSERT_EQ(i.y(), 3.0f);
		ASSERT_EQ(i.z(), 4.0f);

		ASSERT_EQ(get<0>(q), 1.0f);
		ASSERT_EQ(get<1>(q).x(), 2.0f);
		ASSERT_EQ(get<1>(q).y(), 3.0f);
		ASSERT_EQ(get<1>(q).z(), 4.0f);

		auto& [rr, ri] = q;
		rr = 5.0f;
		ri.x() = 6.0f;
		ri.y() = 7.0f;
		ri.z() = 8.0f;

		ASSERT_EQ(q.w(), 5.0f);
		ASSERT_EQ(q.x(), 6.0f);
		ASSERT_EQ(q.y(), 7.0f);
		ASSERT_EQ(q.z(), 8.0f);

		ASSERT_NE(r, rr);
		ASSERT_NE(i.x(), ri.x());
		ASSERT_NE(i.y(), ri.y());
		ASSERT_NE(i.z(), ri.z());
	});

	suite.tests.emplace_back("quaternion formatter", []()
	{
		fquat q{-9999.96f, 0.0f, 2.0f / 3.0f, std::numeric_limits<float>::infinity()};

		auto str = std::format("{:.4f}", q);
		ASSERT_EQ(str, "{-9999.9600, {0.0000, 0.6667, inf}}");
	});

	suite.tests.emplace_back("quaternion operators", []()
	{
		const float tolerance = 1e-6f;

		fquat a{1.0f, 2.0f, 3.0f, 4.0f};
		fquat b{-3.0f, 23.0f, 0.0f, 0.1f};
		fquat c;
		
		c = a + b;
		ASSERT_NEAR(c.w(), -2.0f, tolerance);
		ASSERT_NEAR(c.x(), 25.0f, tolerance);
		ASSERT_NEAR(c.y(), 3.0f, tolerance);
		ASSERT_NEAR(c.z(), 4.1f, tolerance);

		c = a;
		c += b;
		ASSERT_NEAR(c.w(), -2.0f, tolerance);
		ASSERT_NEAR(c.x(), 25.0f, tolerance);
		ASSERT_NEAR(c.y(), 3.0f, tolerance);
		ASSERT_NEAR(c.z(), 4.1f, tolerance);

		c = a + 5.0f;
		ASSERT_NEAR(c.w(), 6.0f, tolerance);
		ASSERT_NEAR(c.x(), 7.0f, tolerance);
		ASSERT_NEAR(c.y(), 8.0f, tolerance);
		ASSERT_NEAR(c.z(), 9.0f, tolerance);

		c = a;
		c += 5.0f;
		ASSERT_NEAR(c.w(), 6.0f, tolerance);
		ASSERT_NEAR(c.x(), 7.0f, tolerance);
		ASSERT_NEAR(c.y(), 8.0f, tolerance);
		ASSERT_NEAR(c.z(), 9.0f, tolerance);

		c = 3.0f + a;
		ASSERT_NEAR(c.w(), 4.0f, tolerance);
		ASSERT_NEAR(c.x(), 5.0f, tolerance);
		ASSERT_NEAR(c.y(), 6.0f, tolerance);
		ASSERT_NEAR(c.z(), 7.0f, tolerance);

		c = -a;
		ASSERT_NEAR(c.w(), -1.0f, tolerance);
		ASSERT_NEAR(c.x(), -2.0f, tolerance);
		ASSERT_NEAR(c.y(), -3.0f, tolerance);
		ASSERT_NEAR(c.z(), -4.0f, tolerance);

		c = a - b;
		ASSERT_NEAR(c.w(), 4.0f, tolerance);
		ASSERT_NEAR(c.x(), -21.0f, tolerance);
		ASSERT_NEAR(c.y(), 3.0f, tolerance);
		ASSERT_NEAR(c.z(), 3.9f, tolerance);

		c = a;
		c -= b;
		ASSERT_NEAR(c.w(), 4.0f, tolerance);
		ASSERT_NEAR(c.x(), -21.0f, tolerance);
		ASSERT_NEAR(c.y(), 3.0f, tolerance);
		ASSERT_NEAR(c.z(), 3.9f, tolerance);

		c = a - 2.0f;
		ASSERT_NEAR(c.w(), -1.0f, tolerance);
		ASSERT_NEAR(c.x(), 0.0f, tolerance);
		ASSERT_NEAR(c.y(), 1.0f, tolerance);
		ASSERT_NEAR(c.z(), 2.0f, tolerance);

		c = a;
		c -= 2.0f;
		ASSERT_NEAR(c.w(), -1.0f, tolerance);
		ASSERT_NEAR(c.x(), 0.0f, tolerance);
		ASSERT_NEAR(c.y(), 1.0f, tolerance);
		ASSERT_NEAR(c.z(), 2.0f, tolerance);

		c = 10.0f - a;
		ASSERT_NEAR(c.w(), 9.0f, tolerance);
		ASSERT_NEAR(c.x(), 8.0f, tolerance);
		ASSERT_NEAR(c.y(), 7.0f, tolerance);
		ASSERT_NEAR(c.z(), 6.0f, tolerance);

		c = b / a;
		ASSERT_NEAR(c.w(), -3.0f, tolerance);
		ASSERT_NEAR(c.x(), 23.0f / 2.0f, tolerance);
		ASSERT_NEAR(c.y(), 0.0f, tolerance);
		ASSERT_NEAR(c.z(), 0.1f / 4.0f, tolerance);

		c = b;
		c /= a;
		ASSERT_NEAR(c.w(), -3.0f, tolerance);
		ASSERT_NEAR(c.x(), 23.0f / 2.0f, tolerance);
		ASSERT_NEAR(c.y(), 0.0f, tolerance);
		ASSERT_NEAR(c.z(), 0.1f / 4.0f, tolerance);

		c = a / 2.0f;
		ASSERT_NEAR(c.w(), 0.5f, tolerance);
		ASSERT_NEAR(c.x(), 1.0f, tolerance);
		ASSERT_NEAR(c.y(), 1.5f, tolerance);
		ASSERT_NEAR(c.z(), 2.0f, tolerance);

		c = a;
		c /= 2.0f;
		ASSERT_NEAR(c.w(), 0.5f, tolerance);
		ASSERT_NEAR(c.x(), 1.0f, tolerance);
		ASSERT_NEAR(c.y(), 1.5f, tolerance);
		ASSERT_NEAR(c.z(), 2.0f, tolerance);

		c = 3.0f / a;
		ASSERT_NEAR(c.w(), 3.0f, tolerance);
		ASSERT_NEAR(c.x(), 3.0f / 2.0f, tolerance);
		ASSERT_NEAR(c.y(), 1.0f, tolerance);
		ASSERT_NEAR(c.z(), 0.75f, tolerance);

		c = a * 2.0f;
		ASSERT_NEAR(c.w(), 2.0f, tolerance);
		ASSERT_NEAR(c.x(), 4.0f, tolerance);
		ASSERT_NEAR(c.y(), 6.0f, tolerance);
		ASSERT_NEAR(c.z(), 8.0f, tolerance);

		c = a;
		c *= 2.0f;
		ASSERT_NEAR(c.w(), 2.0f, tolerance);
		ASSERT_NEAR(c.x(), 4.0f, tolerance);
		ASSERT_NEAR(c.y(), 6.0f, tolerance);
		ASSERT_NEAR(c.z(), 8.0f, tolerance);

		c = 0.5f * a;
		ASSERT_NEAR(c.w(), 0.5f, tolerance);
		ASSERT_NEAR(c.x(), 1.0f, tolerance);
		ASSERT_NEAR(c.y(), 1.5f, tolerance);
		ASSERT_NEAR(c.z(), 2.0f, tolerance);

		a = {0.0f, 1.0f, 0.0f, 0.0f};
		b = {0.0f, 0.0f, 1.0f, 0.0f};

		c = a * b;
		ASSERT_NEAR(c.w(), 0.0f, tolerance);
		ASSERT_NEAR(c.x(), 0.0f, tolerance);
		ASSERT_NEAR(c.y(), 0.0f, tolerance);
		ASSERT_NEAR(c.z(), 1.0f, tolerance);

		c = a;
		c *= b;
		ASSERT_NEAR(c.w(), 0.0f, tolerance);
		ASSERT_NEAR(c.x(), 0.0f, tolerance);
		ASSERT_NEAR(c.y(), 0.0f, tolerance);
		ASSERT_NEAR(c.z(), 1.0f, tolerance);

		c = b * a;
		ASSERT_NEAR(c.w(), 0.0f, tolerance);
		ASSERT_NEAR(c.x(), 0.0f, tolerance);
		ASSERT_NEAR(c.y(), 0.0f, tolerance);
		ASSERT_NEAR(c.z(), -1.0f, tolerance);

		c = b;
		c *= a;
		ASSERT_NEAR(c.w(), 0.0f, tolerance);
		ASSERT_NEAR(c.x(), 0.0f, tolerance);
		ASSERT_NEAR(c.y(), 0.0f, tolerance);
		ASSERT_NEAR(c.z(), -1.0f, tolerance);

		a = {sqrt_half<float>, sqrt_half<float>, 0.0f, 0.0f}; // 90 degree rotation about X-axis
		fvec3 v0{1.0f, 0.0f, 0.0f};

		fvec3 v1 = a * v0;
		ASSERT_NEAR(v1.x(), 1.0f, tolerance);
		ASSERT_NEAR(v1.y(), 0.0f, tolerance);
		ASSERT_NEAR(v1.z(), 0.0f, tolerance);
		ASSERT_NEAR(length(v1), 1.0f, tolerance);

		v0 = {0.0f, 1.0f, 0.0f};
		v1 = a * v0;
		ASSERT_NEAR(v1.x(), 0.0f, tolerance);
		ASSERT_NEAR(v1.y(), 0.0f, tolerance);
		ASSERT_NEAR(v1.z(), 1.0f, tolerance);
		ASSERT_NEAR(length(v1), 1.0f, tolerance);

		v0 = {0.0f, 0.0f, 2.0f};
		v1 = a * v0;
		ASSERT_NEAR(v1.x(), 0.0f, tolerance);
		ASSERT_NEAR(v1.y(), -2.0f, tolerance);
		ASSERT_NEAR(v1.z(), 0.0f, tolerance);
		ASSERT_NEAR(length(v1), 2.0f, tolerance);

		v1 = v0 * a;
		ASSERT_NEAR(v1.x(), 0.0f, tolerance);
		ASSERT_NEAR(v1.y(), 2.0f, tolerance);
		ASSERT_NEAR(v1.z(), 0.0f, tolerance);
		ASSERT_NEAR(length(v1), 2.0f, tolerance);
	});

	suite.tests.emplace_back("quaternion common functions", []()
	{
		const float tolerance = 1e-6f;

		fquat a{1.0f, 2.0f, 3.0f, 4.0f};
		fquat b{-7.0f, 39.0f, 0.0f, 0.1f};

		fquat c = conjugate(a);
		ASSERT_EQ(c.w(), 1.0f);
		ASSERT_EQ(c.x(), -2.0f);
		ASSERT_EQ(c.y(), -3.0f);
		ASSERT_EQ(c.z(), -4.0f);

		float s = dot(a, b);
		ASSERT_NEAR(s, 71.4f, tolerance);

		s = sqr_length(a);
		ASSERT_NEAR(s, 30.0f, tolerance);

		s = length(a);
		ASSERT_NEAR(s, sqrt(30.0f), tolerance);

		s = rcp_length(a);
		ASSERT_NEAR(s, 1.0f / sqrt(30.0f), tolerance);

		c = normalize(a);
		ASSERT_NEAR(c.w(), 1.0f / sqrt(30.0f), tolerance);
		ASSERT_NEAR(c.x(), 2.0f / sqrt(30.0f), tolerance);
		ASSERT_NEAR(c.y(), 3.0f / sqrt(30.0f), tolerance);
		ASSERT_NEAR(c.z(), 4.0f / sqrt(30.0f), tolerance);
		ASSERT_NEAR(length(c), 1.0f, tolerance);
	});

	suite.tests.emplace_back("quaternion rotation", []()
	{
		const float tolerance = 1e-6f;

		const fvec3 axes[][2] =
		{
			{{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Two perpendicular vectors
			{{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}, // Two identical vectors
			{normalize(fvec3{1, 2, 3}), normalize(fvec3{5, -3, 2})}, // Two arbitrary vectors
			{normalize(fvec3{10, 11, 1000}), normalize(fvec3{-50, -51, -52})}, // Two arbitrary vectors
			{normalize(fvec3{1, 1, 0}), normalize(fvec3{0, 1, 1})}, // Two arbitrary vectors
			{{1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}, // Two opposing vectors (X-axis)
			{{0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Two opposing vectors (Y-axis)
			{{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}}, // Two opposing vectors (Z-axis)
			{normalize(fvec3{-0.025f, 0.5f, -0.372f}), normalize(fvec3{0.025f, -0.5f, 0.372f})} // Two opposing vectors (arbitrary axis)
		};

		// Unlimited rotation (a to b)
		for (const auto& [a, b]: axes)
		{
			fvec3 c = rotation(a, b, tolerance) * a;
			ASSERT_NEAR(c.x(), b.x(), tolerance);
			ASSERT_NEAR(c.y(), b.y(), tolerance);
			ASSERT_NEAR(c.z(), b.z(), tolerance);

			c = rotate_towards(a, b, pi<float>, tolerance) * a;
			ASSERT_NEAR(c.x(), b.x(), tolerance);
			ASSERT_NEAR(c.y(), b.y(), tolerance);
			ASSERT_NEAR(c.z(), b.z(), tolerance);
		}

		// Angle-limited rotation (X-axis to Y-axis, 45 degree limit)
		fvec3 a = {1.0f, 0.0f, 0.0f};
		fvec3 b = {0.0f, 1.0f, 0.0f};
		fvec3 c = rotate_towards(a, b, pi<float> / 4.0f, tolerance) * a;
		ASSERT_NEAR(c.x(), sqrt_half<float>, tolerance);
		ASSERT_NEAR(c.y(), sqrt_half<float>, tolerance);
		ASSERT_NEAR(c.z(), 0.0f, tolerance);
	});

	suite.tests.emplace_back("quaternion interpolation", []()
	{
		const float tolerance = 1e-6f;

		struct test_data
		{
			fquat a;
			fquat b;
			float t;
			fquat c;
		};

		const test_data lerp_data[] =
		{
			{{0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, 0.0f, {0.0f, 0.0f, 0.0f, 0.0f}},
			{{0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, 0.0f, {0.0f, 0.0f, 0.0f, 0.0f}},
			{{1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, 0.0f, {1.0f, 0.0f, 0.0f, 0.0f}},
			{{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, 0.0f, {1.0f, 0.0f, 0.0f, 0.0f}},
			{{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, 0.0f, {1.0f, 0.0f, 0.0f, 0.0f}},
			{{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, 0.0f, {1.0f, 0.0f, 0.0f, 0.0f}},
			{{0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, 0.5f, {0.0f, 0.0f, 0.0f, 0.0f}},
			{{0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, 0.5f, {0.5f, 0.0f, 0.0f, 0.0f}},
			{{1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, 0.5f, {1.0f, 0.0f, 0.0f, 0.0f}},
			{{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, 0.5f, {0.5f, 0.5f, 0.0f, 0.0f}},
			{{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, 0.5f, {0.5f, 0.0f, 0.5f, 0.0f}},
			{{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, 0.5f, {0.5f, 0.0f, 0.0f, 0.5f}},
			{{0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, 1.0f, {0.0f, 0.0f, 0.0f, 0.0f}},
			{{0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, 1.0f, {1.0f, 0.0f, 0.0f, 0.0f}},
			{{1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, 1.0f, {1.0f, 0.0f, 0.0f, 0.0f}},
			{{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, 1.0f, {0.0f, 1.0f, 0.0f, 0.0f}},
			{{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, 1.0f, {0.0f, 0.0f, 1.0f, 0.0f}},
			{{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, 1.0f, {0.0f, 0.0f, 0.0f, 1.0f}}
		};

		const test_data nlerp_slerp_data[] =
		{
			{identity<fquat>, identity<fquat>, 0.0f, identity<fquat>},
			{identity<fquat>, identity<fquat>, 0.5f, identity<fquat>},
			{identity<fquat>, identity<fquat>, 1.0f, identity<fquat>},
			{identity<fquat>, -identity<fquat>, 0.0f, identity<fquat>},
			{identity<fquat>, -identity<fquat>, 0.5f, identity<fquat>},
			{identity<fquat>, -identity<fquat>, 1.0f, identity<fquat>},
			{identity<fquat>, {0.0f, 1.0f, 0.0f, 0.0f}, 0.0f, identity<fquat>},
			{identity<fquat>, {0.0f, 1.0f, 0.0f, 0.0f}, 0.5f, {sqrt_half<float>, sqrt_half<float>, 0.0f, 0.0f}},
			{identity<fquat>, {0.0f, 1.0f, 0.0f, 0.0f}, 1.0f, {0.0f, 1.0f, 0.0f, 0.0f}},
			{identity<fquat>, {sqrt_half<float>, sqrt_half<float>, 0.0f, 0.0f}, 0.0f, identity<fquat>},
			{identity<fquat>, {sqrt_half<float>, sqrt_half<float>, 0.0f, 0.0f}, 0.5f, {cos(radians(22.5f)), sin(radians(22.5f)), 0.0f, 0.0f}},
			{identity<fquat>, {sqrt_half<float>, sqrt_half<float>, 0.0f, 0.0f}, 1.0f, {sqrt_half<float>, sqrt_half<float>, 0.0f, 0.0f}},
		};

		for (const auto& [a, b, t, c_expected]: lerp_data)
		{
			const fquat c = lerp(a, b, t);
			ASSERT_NEAR(c.w(), c_expected.w(), tolerance);
			ASSERT_NEAR(c.x(), c_expected.x(), tolerance);
			ASSERT_NEAR(c.y(), c_expected.y(), tolerance);
			ASSERT_NEAR(c.z(), c_expected.z(), tolerance);
		}

		for (const auto& [a, b, t, c_expected] : nlerp_slerp_data)
		{
			fquat c = nlerp(a, b, t);
			ASSERT_NEAR(c.w(), c_expected.w(), tolerance);
			ASSERT_NEAR(c.x(), c_expected.x(), tolerance);
			ASSERT_NEAR(c.y(), c_expected.y(), tolerance);
			ASSERT_NEAR(c.z(), c_expected.z(), tolerance);
			ASSERT_NEAR(length(c), 1.0f, tolerance);
		}

		for (const auto& [a, b, t, c_expected] : nlerp_slerp_data)
		{
			const fquat c = slerp(a, b, t);
			ASSERT_NEAR(c.w(), c_expected.w(), tolerance);
			ASSERT_NEAR(c.x(), c_expected.x(), tolerance);
			ASSERT_NEAR(c.y(), c_expected.y(), tolerance);
			ASSERT_NEAR(c.z(), c_expected.z(), tolerance);
			ASSERT_NEAR(length(c), 1.0f, tolerance);
		}
	});

	suite.tests.emplace_back("quaternion swing twist decomposition", []()
	{
		const float tolerance = 1e-6f;

		// Identity quaternion: no rotation
		{
			const fquat q = identity<fquat>;
			const fvec3 axis{1.0f, 0.0f, 0.0f};
			const auto [swing, twist] = swing_twist(q, axis, tolerance);

			ASSERT_NEAR(swing.w(), 1.0f, tolerance);
			ASSERT_NEAR(swing.x(), 0.0f, tolerance);
			ASSERT_NEAR(swing.y(), 0.0f, tolerance);
			ASSERT_NEAR(swing.z(), 0.0f, tolerance);

			ASSERT_NEAR(twist.w(), 1.0f, tolerance);
			ASSERT_NEAR(twist.x(), 0.0f, tolerance);
			ASSERT_NEAR(twist.y(), 0.0f, tolerance);
			ASSERT_NEAR(twist.z(), 0.0f, tolerance);
		}

		// Pure twist: 90 degree rotation about X-axis
		{
			const fquat q{cos(pi<float> / 4.0f), sin(pi<float> / 4.0f), 0.0f, 0.0f};
			const fvec3 axis{1.0f, 0.0f, 0.0f};
			const auto [swing, twist] = swing_twist(q, axis, tolerance);

			ASSERT_NEAR(swing.w(), 1.0f, tolerance);
			ASSERT_NEAR(swing.x(), 0.0f, tolerance);
			ASSERT_NEAR(swing.y(), 0.0f, tolerance);
			ASSERT_NEAR(swing.z(), 0.0f, tolerance);

			ASSERT_NEAR(twist.w(), q.w(), tolerance);
			ASSERT_NEAR(twist.x(), q.x(), tolerance);
			ASSERT_NEAR(twist.y(), q.y(), tolerance);
			ASSERT_NEAR(twist.z(), q.z(), tolerance);
		}

		// Pure swing: 90 degree rotation about Y-axis, twist axis X
		{
			const fquat q{cos(pi<float> / 4.0f), 0.0f, sin(pi<float> / 4.0f), 0.0f};
			const fvec3 axis{1.0f, 0.0f, 0.0f};
			const auto [swing, twist] = swing_twist(q, axis, tolerance);

			ASSERT_NEAR(twist.w(), 1.0f, tolerance);
			ASSERT_NEAR(twist.x(), 0.0f, tolerance);
			ASSERT_NEAR(twist.y(), 0.0f, tolerance);
			ASSERT_NEAR(twist.z(), 0.0f, tolerance);

			ASSERT_NEAR(swing.w(), q.w(), tolerance);
			ASSERT_NEAR(swing.x(), q.x(), tolerance);
			ASSERT_NEAR(swing.y(), q.y(), tolerance);
			ASSERT_NEAR(swing.z(), q.z(), tolerance);
		}

		// Combined swing and twist: 45 deg X, 45 deg Y
		{
			const fquat q = normalize(fquat{cos(pi<float> / 4.0f), sin(pi<float> / 4.0f), sin(pi<float> / 4.0f), 0.0f});
			const fvec3 axis{1.0f, 0.0f, 0.0f};
			const auto [swing, twist] = swing_twist(q, axis, tolerance);

			// Twist should be a rotation about X only
			ASSERT_NEAR(twist.y(), 0.0f, tolerance);
			ASSERT_NEAR(twist.z(), 0.0f, tolerance);

			// Swing should have no X component in the vector part
			ASSERT_NEAR(swing.x(), 0.0f, tolerance);

			// Recompose: swing * twist = q
			const fquat recomposed = swing * twist;
			ASSERT_NEAR(recomposed.w(), q.w(), tolerance);
			ASSERT_NEAR(recomposed.x(), q.x(), tolerance);
			ASSERT_NEAR(recomposed.y(), q.y(), tolerance);
			ASSERT_NEAR(recomposed.z(), q.z(), tolerance);
		}

		// Arbitrary axis
		{
			const fquat q = {sqrt_2<float> / 2.0f, 0.0f, sqrt_2<float> / 2.0f, 0.0f};
			const fvec3 axis = {sqrt_2<float> / 2.0f, sqrt_2<float> / 2.0f, 0.0f};
			const auto [swing, twist] = swing_twist(q, axis, tolerance);

			// Twist axis should be parallel to axis
			ASSERT_NEAR(sqr_length(cross(twist.i, axis)), 0.0f, tolerance);

			// Recompose: swing * twist = q
			const fquat recomposed = swing * twist;
			ASSERT_NEAR(recomposed.w(), q.w(), tolerance);
			ASSERT_NEAR(recomposed.x(), q.x(), tolerance);
			ASSERT_NEAR(recomposed.y(), q.y(), tolerance);
			ASSERT_NEAR(recomposed.z(), q.z(), tolerance);
		}
	});

	return suite.run();
}
