// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "test.hpp"
#include <print>

int test_suite::run()
{
	int passed = 0;
	int failed = 0;

	for (const auto& test: tests)
	{
		try
		{
			test.function();
			std::println("[PASSED] {}", test.name);
			++passed;
		}
		catch (const std::exception& e)
		{
			std::println("[FAILED] {}: {}", test.name, e.what());
			++failed;
		}
		catch (...)
		{
			std::println("[FAILED] {}: Unknown exception.", test.name);
			++failed;
		}
	}

	std::println("passed: {}/{}", passed, tests.size());

	return failed;
}

