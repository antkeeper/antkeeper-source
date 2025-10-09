// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cmath>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

/// Individual test case.
struct test_case
{
	/// Name of the test case.
	std::string name;

	/// Test function.
	std::function<void()> function;
};

/// Set of related tests.
struct test_suite
{
	/// Runs all tests in the suite.
	/// @return Number of failed tests.
	int run();

	std::vector<test_case> tests;
};

#define ASSERT(condition) \
	do { if (!(condition)) throw std::runtime_error("Assertion failed: " #condition); } while (false)

#define ASSERT_EQ(a, b) \
	do { if ((a) != (b)) throw std::runtime_error("Assertion failed: " #a " == " #b); } while (false)

#define ASSERT_NE(a, b) \
	do { if ((a) == (b)) throw std::runtime_error("Assertion failed: " #a " != " #b); } while (false)

#define ASSERT_LT(a, b) \
	do { if (!((a) < (b))) throw std::runtime_error("Assertion failed: " #a " < " #b); } while (false)

#define ASSERT_LE(a, b) \
	do { if (!((a) <= (b))) throw std::runtime_error("Assertion failed: " #a " <= " #b); } while (false)

#define ASSERT_GT(a, b) \
	do { if (!((a) > (b))) throw std::runtime_error("Assertion failed: " #a " > " #b); } while (false)

#define ASSERT_GE(a, b) \
	do { if (!((a) >= (b))) throw std::runtime_error("Assertion failed: " #a " >= " #b); } while (false)

#define ASSERT_NEAR(a, b, tolerance) \
	do \
	{ \
		const auto difference = std::fabs((a) - (b)); \
		if (std::isnan(difference) || difference > (tolerance)) \
			throw std::runtime_error("Assertion failed: |" #a " - " #b "| <= " #tolerance); \
	} while (false)

#define STATIC_ASSERT(condition) static_assert(condition, "Static assertion failed: " #condition)

#define STATIC_ASSERT_EQ(a, b) static_assert((a) == (b), "Static assertion failed: " #a " == " #b)

#define STATIC_ASSERT_NE(a, b) static_assert((a) != (b), "Static assertion failed: " #a " != " #b)

#define STATIC_ASSERT_LT(a, b) static_assert((a) < (b), "Static assertion failed: " #a " < " #b)

#define STATIC_ASSERT_LE(a, b) static_assert((a) <= (b), "Static assertion failed: " #a " <= " #b)

#define STATIC_ASSERT_GT(a, b) static_assert((a) > (b), "Static assertion failed: " #a " > " #b)

#define STATIC_ASSERT_GE(a, b) static_assert((a) >= (b), "Static assertion failed: " #a " >= " #b)
