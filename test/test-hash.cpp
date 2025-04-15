// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "test.hpp"
import hash;

using namespace hash;

int main(int argc, char* argv[])
{
	test_suite suite;

	suite.tests.emplace_back("FNV hash functions", []()
	{
		ASSERT_EQ("Hello, World!"_fnv1a32, fnv32_t{0x5aecf734});
		ASSERT_EQ("Hello, World!"_fnv1a64, fnv64_t{0x6ef05bd7cc857c54});
	});

	return suite.run();
}
