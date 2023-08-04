/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <engine/animation/locomotion/step.hpp>
#include <cmath>

float step::phase(float t) const noexcept
{
	// Make phase relative to step stance
	float i;
	t = std::modf(1.0f + t + delay - duty_factor, &i);
	
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
