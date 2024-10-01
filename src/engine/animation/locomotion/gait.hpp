// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_GAIT_HPP
#define ANTKEEPER_ANIMATION_GAIT_HPP

#include <engine/animation/locomotion/step.hpp>
#include <vector>

/// Describes the synchronized timing of limbs in a locomotion pattern.
struct gait
{
public:
	/// Frequency of the gait cycle, in Hz.
	float frequency{};
	
	/// Array of steps for each limb.
	std::vector<step> steps;
	
	/// Returns the phase of the gait at the elapsed time.
	/// @param t Elapsed time, in seconds.
	/// @return Gait phase, on `[0, 1]`.
	[[nodiscard]] float phase(float t) const noexcept;
};

#endif // ANTKEEPER_ANIMATION_GAIT_HPP
