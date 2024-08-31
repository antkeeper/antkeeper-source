// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "test.hpp"
#include <engine/math/math.hpp>

using namespace math;

int main(int argc, char* argv[])
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

		ASSERT_EQ(v2.size(), 2);
		ASSERT_EQ(v3.size(), 3);
		ASSERT_EQ(v4.size(), 4);

		ASSERT_EQ(v2.max_size(), 2);
		ASSERT_EQ(v3.max_size(), 3);
		ASSERT_EQ(v4.max_size(), 4);
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
		ivec3 b{4, 5, 6};
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

	return suite.run();
}

