// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_ANIMATION_CURVE_HPP
#define ANTKEEPER_ANIMATION_ANIMATION_CURVE_HPP

#include <engine/animation/keyframe.hpp>
#include <engine/animation/keyframe-container.hpp>
#include <engine/animation/keyframe-extrapolation.hpp>
#include <engine/animation/keyframe-interpolation.hpp>
#include <functional>

/// Keyframe animation curve.
class animation_curve
{
public:
	/// Keyframe interpolator function type.
	/// @details An interpolator function takes three parameters: the first keyframe, the second keyframe, and a time on the interval formed by the two keyframe times. An interpolator function should return a value interpolated between the two keyframe values.
	using keyframe_interpolator_type = std::function<float(const keyframe&, const keyframe&, float)>;

	/// Keyframe extrapolator function type.
	/// @details An extrapolator function takes two parameters: a reference to the keyframe container, and a time outside the interval formed by the first and last keyframe times. An extrapolator function should return an extrapolated value according to the given time.
	/// @warning Keyframe container may be empty.
	using keyframe_extrapolator_type = std::function<float(const keyframe_container&, float)>;

	/// Evaluates the curve at a given time.
	/// @param time Time at which to evaluate the curve.
	/// @return Value of the curve at @p time.
	[[nodiscard]] float evaluate(float time) const;

	/// Returns a reference to the keyframes of the curve.
	[[nodiscard]] inline constexpr auto& keyframes() noexcept
	{
		return m_keyframes;
	}

	/// @copydoc keyframes()
	[[nodiscard]] inline constexpr const auto& keyframes() const noexcept
	{
		return m_keyframes;
	}

	/// Returns a reference to the keyframe interpolator function object.
	[[nodiscard]] inline constexpr auto& interpolator() noexcept
	{
		return m_interpolator;
	}

	/// @copydoc interpolator()
	[[nodiscard]] inline constexpr const auto& interpolator() const noexcept
	{
		return m_interpolator;
	}

	/// Returns a reference to the keyframe extrapolator function object.
	/// @note The extrapolator may be utilized as a source of procedural animation: if a curve with no keyframes is evaluated, the extrapolator will be called with the empty keyframe container and evaluation time as parameters.
	[[nodiscard]] inline constexpr auto& extrapolator() noexcept
	{
		return m_extrapolator;
	}

	/// @copydoc extrapolator()
	[[nodiscard]] inline constexpr const auto& extrapolator() const noexcept
	{
		return m_extrapolator;
	}

	/// Returns the non-negative duration of the curve, in seconds.
	[[nodiscard]] float duration() const;

private:
	keyframe_container m_keyframes;
	keyframe_interpolator_type m_interpolator{interpolate_keyframes_linear};
	keyframe_extrapolator_type m_extrapolator{extrapolate_keyframes_clamp};
};

#endif // ANTKEEPER_ANIMATION_ANIMATION_CURVE_HPP
