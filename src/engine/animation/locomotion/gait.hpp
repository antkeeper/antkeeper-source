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

#ifndef ANTKEEPER_ANIMATION_GAIT_HPP
#define ANTKEEPER_ANIMATION_GAIT_HPP

#include <engine/animation/locomotion/step.hpp>
#include <vector>

/**
 * Describes the synchronized timing of limbs in a locomotion pattern.
 */
struct gait
{
public:
	/// Frequency of the gait cycle, in Hz.
	float frequency{};
	
	/// Array of steps for each limb.
	std::vector<step> steps;
	
	/**
	 * Returns the phase of the gait at the elapsed time.
	 *
	 * @param t Elapsed time, in seconds.
	 *
	 * @return Gait phase, on `[0, 1]`.
	 */
	[[nodiscard]] float phase(float t) const noexcept;
};

#endif // ANTKEEPER_ANIMATION_GAIT_HPP
