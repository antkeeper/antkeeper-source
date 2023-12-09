// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/locomotion/step.hpp>
#include <engine/math/common.hpp>
#include <cmath>

float step::phase(float t) const noexcept
{
	// Make phase relative to step stance
	t = math::fract(1.0f + t + delay - duty_factor);
	
	if (t < duty_factor)
	{
		// Stance phase, on `[-1, 0)`
		return (t - duty_factor) / duty_factor;
	}
	else
	{
		// Swing phase, on `[0, 1]`
		return (t - duty_factor) / (1.0f - duty_factor);
	}
}
