// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/animation-curve.hpp>

float animation_curve::evaluate(float time) const
{
	// Check if time is outside keyframe range
	if (m_keyframes.empty() || time < m_keyframes.begin()->time || time > m_keyframes.rbegin()->time)
	{
		// Extrapolate outside keyframe range
		return m_extrapolator(m_keyframes, time);
	}

	// Find first keyframe at or after given time
	const auto next = m_keyframes.lower_bound({time, 0.0f});

	// Check if next keyframe is the first keyframe
	if (next == m_keyframes.begin())
	{
		return next->value;
	}

	// Find preceding keyframe
	const auto previous = std::prev(next);

	// Interpolate between keyframes
	return m_interpolator(*previous, *next, time);
}

