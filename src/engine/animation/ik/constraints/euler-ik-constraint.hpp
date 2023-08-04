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
#include <engine/math/euler-angles.hpp>
#include <engine/math/numbers.hpp>

/**
 * Euler angle IK constraint.
 */
class euler_ik_constraint: public ik_constraint
{
public:
	void solve(math::fquat& q) override;
	
	/**
	 * Sets the rotation sequence of the Euler angles of the constraint.
	 *
	 * @param sequence Euler angle rotation sequence.
	 */
	inline void set_rotation_sequence(math::rotation_sequence sequence) noexcept
	{
		m_rotation_sequence = sequence;
	}
	
	/**
	 * Sets the minimum Euler angles of the constraint.
	 *
	 * @param angles Minimum angles of the first, second, and third Euler angles, in radians.
	 */
	inline void set_min_angles(const math::fvec3& angles) noexcept
	{
		m_min_angles = angles;
	}
	
	/**
	 * Sets the maximum Euler angles of the constraint.
	 *
	 * @param angles Maximum angles of the first, second, and third Euler angles, in radians.
	 */
	inline void set_max_angles(const math::fvec3& angles) noexcept
	{
		m_max_angles = angles;
	}
	
	/// Returns the rotation sequence of the Euler angles of the constraint.
	[[nodiscard]] inline constexpr math::rotation_sequence get_rotation_sequence() const noexcept
	{
		return m_rotation_sequence;
	}
	
	/// Returns the minimum angles of the first, second, and third Euler angles, in radians.
	[[nodiscard]] inline constexpr const math::fvec3& get_min_angles() const noexcept
	{
		return m_min_angles;
	}
	
	/// Returns the maximum angles of the first, second, and third Euler angles, in radians.
	[[nodiscard]] inline constexpr const math::fvec3& get_max_angles() const noexcept
	{
		return m_max_angles;
	}
	
private:
	math::rotation_sequence m_rotation_sequence{math::rotation_sequence::xyz};
	math::fvec3 m_min_angles{-math::pi<float>, -math::pi<float>, -math::pi<float>};
	math::fvec3 m_max_angles{ math::pi<float>,  math::pi<float>,  math::pi<float>};
};

#endif // ANTKEEPER_ANIMATION_EULER_IK_CONSTRAINT_HPP
