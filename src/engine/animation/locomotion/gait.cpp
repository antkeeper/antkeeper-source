// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.animation.locomotion.gait;
import engine.math.functions;

namespace engine::animation
{
	float gait::phase(float t) const noexcept
	{
		return math::fract(t * frequency);
	}
}
