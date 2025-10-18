// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "test.hpp"
#include <iostream>
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
			std::flush(std::cout);
			++passed;
		}
		catch (const std::exception& e)
		{
			std::println("[FAILED] {}: {}", test.name, e.what());
			std::flush(std::cout);
			++failed;
		}
		catch (...)
		{
			std::println("[FAILED] {}: Unknown exception.", test.name);
			std::flush(std::cout);
			++failed;
		}
	}

	std::println("Passed: {}/{}", passed, tests.size());

	return failed;
}

