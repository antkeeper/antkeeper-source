// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/keyframe-extrapolation.hpp>
#include <stdexcept>

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

