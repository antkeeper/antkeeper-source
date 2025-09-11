// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "test.hpp"
#include <engine/geom/primitives/hypersphere.hpp>
#include <engine/math/constants.hpp>

using namespace engine::geom;
using namespace engine::geom::primitives;
using namespace engine::math;

int main(int, char*[])
{
	test_suite suite;

	suite.tests.emplace_back("Hypersphere", []()
	{
		const float r = 2.0f / 3.0f;

		hypersphere<float, 1> h1{{}, r};
		hypersphere<float, 2> h2{{}, r};
		hypersphere<float, 3> h3{{}, r};
		hypersphere<float, 4> h4{{}, r};
		hypersphere<float, 5> h5{{}, r};

		ASSERT_NEAR(h1.volume(), 2.0f * r, 1e-6);
		ASSERT_NEAR(h2.volume(), pi<float> * r * r, 1e-6);
		ASSERT_NEAR(h3.volume(), 4.0f * pi<float> / 3.0f * r * r * r, 1e-6);
		ASSERT_NEAR(h4.volume(), pi<float> * pi<float> / 2.0f * r * r * r * r, 1e-6);
		ASSERT_NEAR(h5.volume(), 8.0f * pi<float> * pi<float> / 15.0f * r * r * r * r * r, 1e-6);
	});

	return suite.run();
}
