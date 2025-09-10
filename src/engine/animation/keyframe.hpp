// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <set>

namespace engine::animation
{
	/// Keyframe in an animation curve.
	struct keyframe
	{
		/// Time at the keyframe.
		const float time;

		/// Value at the keyframe.
		float value;
	};

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

	/// Set of keyframes, sorted by time.
	using keyframe_container = std::set<keyframe, keyframe_time_comparator>;

	/// Interpolates between two keyframes using linear interpolation.
	/// @param a First keyframe.
	/// @param b Second keyframe.
	/// @param time Interpolation time.
	/// @return Interpolated value.
	[[nodiscard]] float interpolate_keyframes_linear(const keyframe& a, const keyframe& b, float time) noexcept;

	/// Interpolates between two keyframes using constant interpolation.
	/// @param a First keyframe.
	/// @param b Second keyframe.
	/// @param time Interpolation time.
	/// @return Value of @p a.
	[[nodiscard]] float interpolate_keyframes_constant(const keyframe& a, const keyframe& b, float time) noexcept;

	/// Extrapolates keyframes by clamping to the values of the first or last keyframes.
	/// @param keyframes Keyframes.
	/// @param time Extrapolation time.
	/// @return Extrapolated value.
	/// @exception std::runtime_error Failed clamp extrapolation of keyframes: no keyframes provided.
	[[nodiscard]] float extrapolate_keyframes_clamp(const keyframe_container& keyframes, float time);
}
