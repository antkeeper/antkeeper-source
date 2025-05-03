// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.animation.keyframe;
import engine.math.functions;
import <stdexcept>;

namespace engine::animation
{
	float interpolate_keyframes_linear(const keyframe& a, const keyframe& b, float time) noexcept
	{
		const auto t = (time - a.time) / (b.time - a.time);
		return math::lerp(a.value, b.value, t);
	}

	float interpolate_keyframes_constant(const keyframe& a, const keyframe&, float) noexcept
	{
		return a.value;
	}

	float extrapolate_keyframes_clamp(const keyframe_container& keyframes, float time)
	{
		if (keyframes.empty())
		{
			throw std::runtime_error("Failed clamp extrapolation of keyframes: no keyframes provided.");
		}

		if (time < keyframes.begin()->time)
		{
			return keyframes.begin()->value;
		}
		else
		{
			return keyframes.rbegin()->value;
		}
	}
}
