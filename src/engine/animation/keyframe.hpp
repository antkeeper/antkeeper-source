// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_KEYFRAME_HPP
#define ANTKEEPER_ANIMATION_KEYFRAME_HPP

/// Keyframe in an animation curve.
struct keyframe
{
	/// Time at the keyframe.
	const float time;

	/// Value at the keyframe.
	float value;
};

#endif // ANTKEEPER_ANIMATION_KEYFRAME_HPP
