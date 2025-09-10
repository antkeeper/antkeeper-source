// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/physics/kinematics/restitution.hpp>
#include <engine/math/functions.hpp>

namespace engine::physics
{
	float combine_restitution(float a, float b, restitution_combine_mode mode) noexcept
	{
		switch (mode)
		{
			case restitution_combine_mode::average:
				return (a + b) * 0.5f;

			case restitution_combine_mode::minimum:
				return math::min(a, b);

			case restitution_combine_mode::multiply:
				return a * b;

			case restitution_combine_mode::maximum:
				return math::max(a, b);

			default:
				break;
		}

		return 0.0f;
	}
}
