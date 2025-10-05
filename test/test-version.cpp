// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "test.hpp"
#include <engine/utility/version.hpp>

int main(int, char*[])
{
	test_suite suite;

	suite.tests.emplace_back("Version comparison", []()
	{
		using v = engine::version;

		ASSERT_EQ((v{1, 2, 3}), (v{1, 2, 3})); // 1.2.3 == 1.2.3
		ASSERT_NE((v{1, 2, 3}), (v{3, 2, 1})); // 1.2.3 != 3.2.1

		ASSERT_EQ((v{1, 2, 3, "alpha"}), (v{1, 2, 3, "alpha"})); // 1.2.3-alpha == 1.2.3-alpha
		ASSERT_NE((v{1, 2, 3, "alpha"}), (v{1, 2, 3, "beta"})); // 1.2.3-alpha != 1.2.3-beta

		ASSERT_EQ((v{1, 2, 3, "alpha", "build.123"}), (v{1, 2, 3, "alpha", "build.456"})); // 1.2.3-alpha+build.123 == 1.2.3-alpha+build.456
		ASSERT_NE((v{1, 2, 3, "alpha", "build.123"}), (v{1, 2, 3, "beta", "build.123"})); // 1.2.3-alpha+build.123 != 1.2.3-beta+build.123

		ASSERT_LT((v{1, 0, 0}), (v{1, 0, 1})); // 1.0.0 < 1.0.1
		ASSERT_GT((v{1, 0, 1}), (v{1, 0, 0})); // 1.0.1 > 1.0.0

		ASSERT_LT((v{1, 0, 0}), (v{1, 1, 0})); // 1.0.0 < 1.1.0
		ASSERT_GT((v{1, 1, 0}), (v{1, 0, 0})); // 1.1.0 > 1.0.0

		ASSERT_LT((v{1, 0, 0}), (v{2, 0, 0})); // 1.0.0 < 2.0.0
		ASSERT_GT((v{2, 0, 0}), (v{1, 0, 0})); // 2.0.0 > 1.0.0
		
		ASSERT_LT((v{1, 0, 0, "alpha"}), (v{1, 0, 0})); // 1.0.0-alpha < 1.0.0
		ASSERT_GT((v{1, 0, 0}), (v{1, 0, 0, "alpha"})); // 1.0.0 > 1.0.0-alpha

		ASSERT_LT((v{1, 0, 0, "alpha"}), (v{1, 0, 0, "beta"})); // 1.0.0-alpha < 1.0.0-beta
		ASSERT_GT((v{1, 0, 0, "beta"}), (v{1, 0, 0, "alpha"})); // 1.0.0-beta > 1.0.0-alpha

		ASSERT_LT((v{1, 0, 0, "alpha.1"}), (v{1, 0, 0, "alpha.2"})); // 1.0.0-alpha.1 < 1.0.0-alpha.2
		ASSERT_GT((v{1, 0, 0, "alpha.2"}), (v{1, 0, 0, "alpha.1"})); // 1.0.0-alpha.2 > 1.0.0-alpha.1

		ASSERT_LT((v{1, 0, 0, "alpha"}), (v{1, 0, 0, "alpha.1"})); // 1.0.0-alpha < 1.0.0-alpha.1
		ASSERT_GT((v{1, 0, 0, "alpha.1"}), (v{1, 0, 0, "alpha"})); // 1.0.0-alpha.1 > 1.0.0-alpha

		ASSERT_LT((v{1, 0, 0, "alpha.1"}), (v{1, 0, 0, "alpha.beta"})); // 1.0.0-alpha.1 < 1.0.0-alpha.beta
		ASSERT_GT((v{1, 0, 0, "alpha.beta"}), (v{1, 0, 0, "alpha.1"})); // 1.0.0-alpha.beta > 1.0.0-alpha.1

		ASSERT_LT((v{1, 0, 0, "alpha.beta"}), (v{1, 0, 0, "beta"})); // 1.0.0-alpha.beta < 1.0.0-beta
		ASSERT_GT((v{1, 0, 0, "beta"}), (v{1, 0, 0, "alpha.beta"})); // 1.0.0-beta > 1.0.0-alpha.beta

		ASSERT_LT((v{1, 0, 0, "beta"}), (v{1, 0, 0, "beta.2"})); // 1.0.0-beta < 1.0.0-beta.2
		ASSERT_GT((v{1, 0, 0, "beta.2"}), (v{1, 0, 0, "beta"})); // 1.0.0-beta.2 > 1.0.0-beta

		ASSERT_LT((v{1, 0, 0, "beta.2"}), (v{1, 0, 0, "beta.11"})); // 1.0.0-beta.2 < 1.0.0-beta.11
		ASSERT_GT((v{1, 0, 0, "beta.11"}), (v{1, 0, 0, "beta.2"})); // 1.0.0-beta.11 > 1.0.0-beta.2

		ASSERT_LT((v{1, 0, 0, "beta.11"}), (v{1, 0, 0, "rc.1"})); // 1.0.0-beta.11 < 1.0.0-rc.1
		ASSERT_GT((v{1, 0, 0, "rc.1"}), (v{1, 0, 0, "beta.11"})); // 1.0.0-rc.1 > 1.0.0-beta.11

		ASSERT_LT((v{1, 0, 0, "rc.1"}), (v{1, 0, 0})); // 1.0.0-rc.1 < 1.0.0
		ASSERT_GT((v{1, 0, 0}), (v{1, 0, 0, "rc.1"})); // 1.0.0 > 1.0.0-rc.1

		ASSERT_EQ((std::format("{}", v{1, 2, 3})), "1.2.3");
		ASSERT_EQ((std::format("{}", v{1, 2, 3, "alpha.1"})), "1.2.3-alpha.1");
		ASSERT_EQ((std::format("{}", v{1, 2, 3, "alpha.1", "build.123"})), "1.2.3-alpha.1+build.123");
	});

	return suite.run();
}
