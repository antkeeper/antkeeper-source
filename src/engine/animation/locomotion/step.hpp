/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

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
