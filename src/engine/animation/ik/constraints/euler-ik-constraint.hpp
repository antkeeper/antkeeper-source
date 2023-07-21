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

#ifndef ANTKEEPER_ANIMATION_EULER_IK_CONSTRAINT_HPP
#define ANTKEEPER_ANIMATION_EULER_IK_CONSTRAINT_HPP

#include <engine/animation/ik/ik-constraint.hpp>
#include <engine/math/numbers.hpp>

/**
 * Euler angle IK constraint.
 */
class euler_ik_constraint: public ik_constraint
{
public:
	void solve(math::fquat& q) override;
	
	/**
	 * Sets the angular limits.
	 *
	 * @param min Minimum angles of the x-, y-, and z-axes, in radians.
	 * @param max Maximum angles of the x-, y-, and z-axes, in radians.
	 */
	inline void set_limits(const math::fvec3& min, const math::fvec3& max) noexcept
	{
		m_min = min;
		m_max = max;
	}
	
private:
	math::fvec3 m_min{-math::pi<float>, -math::pi<float>, -math::pi<float>};
	math::fvec3 m_max{ math::pi<float>,  math::pi<float>,  math::pi<float>};
};

#endif // ANTKEEPER_ANIMATION_EULER_IK_CONSTRAINT_HPP
