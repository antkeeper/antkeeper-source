// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_ANIMATION_CURVE_HPP
#define ANTKEEPER_ANIMATION_ANIMATION_CURVE_HPP

#include <engine/animation/keyframe.hpp>
#include <set>

/**
 * Keyframe animation curve.
 */
class animation_curve
{
public:
	/**
	 * Evaluates the curve at a given time.
	 *
	 * @param time Time at which to evaluate the curve.
	 *
	 * @return Value of the curve at @p time.
	 */
	[[nodiscard]] float evaluate(float time) const;

	/** Returns the keyframes of the track. */
	[[nodiscard]] inline constexpr auto& keyframes() noexcept
	{
		return m_keyframes;
	}

	/** @copydoc keyframes() */
	[[nodiscard]] inline constexpr const auto& keyframes() const noexcept
	{
		return m_keyframes;
	}

private:
	/**
	 * Interpolates between two keyframes.
	 *
	 * @param a First keyframe.
	 * @param b Second keyframe.
	 * @param t Interpolation factor.
	 *
	 * @return Interpolated value between @p a and @p b.
	 */
	[[nodiscard]] float interpolate(const keyframe& a, const keyframe& b, float t) const;

	/**
	 * Extrapolates outside of the keyframe range.
	 *
	 * @param a First keyframe.
	 * @param b Last keyframe.
	 * @param time Time at which to evaluate the curve.
	 */
	[[nodiscard]] float extrapolate(const keyframe& a, const keyframe& b, float time) const;

	std::set<keyframe, keyframe_time_comparator> m_keyframes;
};

#endif // ANTKEEPER_ANIMATION_ANIMATION_CURVE_HPP
