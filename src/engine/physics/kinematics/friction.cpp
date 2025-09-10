// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/physics/kinematics/friction.hpp>
#include <engine/math/functions.hpp>

namespace engine::physics
{
	float combine_friction(float a, float b, friction_combine_mode mode) noexcept
	{
		switch (mode)
		{
			case friction_combine_mode::average:
				return (a + b) * 0.5f;

			case friction_combine_mode::minimum:
				return math::min(a, b);

			case friction_combine_mode::multiply:
				return a * b;

			case friction_combine_mode::maximum:
				return math::max(a, b);

			default:
				break;
		}

		return 0.0f;
	}
}
