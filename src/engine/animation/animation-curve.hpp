// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_ANIMATION_CURVE_HPP
#define ANTKEEPER_ANIMATION_ANIMATION_CURVE_HPP

#include <engine/animation/keyframe.hpp>
#include <engine/animation/keyframe-time-comparator.hpp>
#include <engine/animation/keyframe-interpolation.hpp>
#include <set>
#include <functional>

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

	/** Returns a reference the keyframes of the curve. */
	[[nodiscard]] inline constexpr auto& keyframes() noexcept
	{
		return m_keyframes;
	}

	/** @copydoc keyframes() */
	[[nodiscard]] inline constexpr const auto& keyframes() const noexcept
	{
		return m_keyframes;
	}

	/** Returns a reference to the keyframe interpolator function object. */
	[[nodiscard]] inline constexpr auto& interpolator() noexcept
	{
		return m_interpolator;
	}

	/** @copydoc interpolator() */
	[[nodiscard]] inline constexpr const auto& interpolator() const noexcept
	{
		return m_interpolator;
	}

private:
	/**
	 * Extrapolates outside of the keyframe range.
	 *
	 * @param a First keyframe.
	 * @param b Last keyframe.
	 * @param time Time at which to evaluate the curve.
	 */
	[[nodiscard]] float extrapolate(const keyframe& a, const keyframe& b, float time) const;

	std::set<keyframe, keyframe_time_comparator> m_keyframes;
	std::function<float(const keyframe&, const keyframe&, float)> m_interpolator{interpolate_keyframes_linear};
};

#endif // ANTKEEPER_ANIMATION_ANIMATION_CURVE_HPP
