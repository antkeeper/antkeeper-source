// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_STEP_HPP
#define ANTKEEPER_ANIMATION_STEP_HPP

/**
 * Describes the timing of a single step in a gait.
 */
struct step
{
public:
	/// Fraction of the gait cycle, on `[0, 1]`, in which the limb is in the stance phase.
	float duty_factor{};
	
	/// Fraction of the gait cycle, on `[0, 1]`, at which the limb enters the swing phase.
	float delay{};
	
	/**
	 * Returns the phase of the step at the given gait phase.
	 *
	 * @param t Gait phase, on `[0, 1]`.
	 *
	 * @return Step phase, on `[-1, 1]`. Values on `[-1, 0)` indicate a stance phase, while values on `[0, 1]` indicate a swing phase.
	 */
	[[nodiscard]] float phase(float t) const noexcept;
};

#endif // ANTKEEPER_ANIMATION_STEP_HPP
