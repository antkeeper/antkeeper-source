// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/animation-curve.hpp>
#include <stdexcept>

float animation_curve::evaluate(float time) const
{
	if (m_keyframes.empty())
	{
		throw std::runtime_error("Animation curve evaluation failed: no keyframes.");
	}

	if (m_keyframes.size() == 1)
	{
		// One keyframe, curve is constant
		return m_keyframes.begin()->value;
	}

	// Get first and last keyframes
	const auto& first = *m_keyframes.begin();
	const auto& last = *m_keyframes.rbegin();

	// Check if time is within keyframe range
	if (time <= first.time || time >= last.time)
	{
		// Extrapolate outside of keyframe range
		return extrapolate(first, last, time);
	}

	// Find boundary keyframes
	const auto next = m_keyframes.lower_bound({time, 0.0f});
	const auto previous = std::prev(next);

	// Calculate interpolation factor
	const auto t = (time - previous->time) / (next->time - previous->time);

	// Interpolate between keyframes
	return interpolate(*previous, *next, t);
}

float animation_curve::interpolate(const keyframe& a, const keyframe& b, float t) const
{
	// Lerp
	return (b.value - a.value) * t + a.value;
}

float animation_curve::extrapolate(const keyframe& a, const keyframe& b, float time) const
{
	// Clamp
	if (time <= a.time)
	{
		return a.value;
	}
	else
	{
		return b.value;
	}
}

