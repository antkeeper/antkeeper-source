// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_KEYFRAME_TIME_COMPARATOR_HPP
#define ANTKEEPER_ANIMATION_KEYFRAME_TIME_COMPARATOR_HPP

#include <engine/animation/keyframe.hpp>

/// Keyframe time comparator.
struct keyframe_time_comparator
{
	/// Compares the times of two keyframes.
	/// @param lhs First keyframe.
	/// @param rhs Second keyframe.
	/// @return `true` if @p lhs precedes @p rhs, `false` otherwise.
	[[nodiscard]] inline constexpr bool operator()(const keyframe& lhs, const keyframe& rhs) const noexcept
	{
		return lhs.time < rhs.time;
	}
};

#endif // ANTKEEPER_ANIMATION_KEYFRAME_TIME_COMPARATOR_HPP
