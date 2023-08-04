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

#ifndef ANTKEEPER_ANIMATION_SWING_TWIST_IK_CONSTRAINT_HPP
#define ANTKEEPER_ANIMATION_SWING_TWIST_IK_CONSTRAINT_HPP

#include <engine/animation/ik/ik-constraint.hpp>
#include <engine/math/numbers.hpp>

/**
 * IK constraint with cone-limited swing and angle-limited twist.
 */
class swing_twist_ik_constraint: public ik_constraint
{
public:
	void solve(math::fquat& q) override;
	
	/**
	 * Sets the twist rotation limit.
	 *
	 * @param min_angle Minimum twist angle, in radians.
	 * @param max_angle Maximum twist angle, in radians.
	 */
	void set_twist_limit(float min_angle, float max_angle);
	
private:
	float m_cos_half_twist_min{ 0};
	float m_sin_half_twist_min{-1};
	float m_cos_half_twist_max{ 0};
	float m_sin_half_twist_max{ 1};
};

#endif // ANTKEEPER_ANIMATION_SWING_TWIST_IK_CONSTRAINT_HPP
