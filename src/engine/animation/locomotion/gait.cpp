// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/locomotion/gait.hpp>
#include <engine/math/functions.hpp>
#include <cmath>

float gait::phase(float t) const noexcept
{
	return math::fract(t * frequency);
}
