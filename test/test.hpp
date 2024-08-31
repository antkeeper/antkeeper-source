// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TEST_HPP
#define ANTKEEPER_TEST_HPP

#include <cmath>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

/** Individual test case. */
struct test_case
{
	/** Name of the test case. */
	std::string name;

	/** Test function. */
	std::function<void()> function;
};

/** Set of related tests. */
struct test_suite
{
	/**
	 * Runs all tests in the suite.
	 *
	 * @return Number of failed tests.
	 */
	int run();

	std::vector<test_case> tests;
};

#define ASSERT(condition) \
	if (!(condition)) throw std::runtime_error("Assertion failed: " #condition)

#define ASSERT_EQ(a, b) \
	if ((a) != (b)) throw std::runtime_error("Assertion failed: " #a " == " #b)

#define ASSERT_NE(a, b) \
	if ((a) == (b)) throw std::runtime_error("Assertion failed: " #a " != " #b)

#define ASSERT_LT(a, b) \
	if (!((a) < (b))) throw std::runtime_error("Assertion failed: " #a " < " #b)

#define ASSERT_LE(a, b) \
	if (!((a) <= (b))) throw std::runtime_error("Assertion failed: " #a " <= " #b)

#define ASSERT_GT(a, b) \
	if (!((a) > (b))) throw std::runtime_error("Assertion failed: " #a " > " #b)

#define ASSERT_GE(a, b) \
	if (!((a) >= (b))) throw std::runtime_error("Assertion failed: " #a " >= " #b)

#define ASSERT_NEAR(a, b, tolerance) \
	if (std::fabs((a) - (b)) > (tolerance)) throw std::runtime_error("Assertion failed: |" #a " - " #b "| <= " #tolerance)

#endif // ANTKEEPER_TEST_HPP
