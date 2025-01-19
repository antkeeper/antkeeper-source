// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/keyframe-interpolation.hpp>
#include <engine/animation/keyframe.hpp>

float interpolate_keyframes_linear(const keyframe& a, const keyframe& b, float time) noexcept
{
	// Calculate normalized interpolation factor
	const auto t = (time - a.time) / (b.time - a.time);

	// Lerp
	return (b.value - a.value) * t + a.value;
}

float interpolate_keyframes_constant(const keyframe& a, const keyframe&, float) noexcept
{
	return a.value;
}
