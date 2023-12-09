// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_EULER_IK_CONSTRAINT_HPP
#define ANTKEEPER_ANIMATION_EULER_IK_CONSTRAINT_HPP

#include <engine/animation/ik/ik-constraint.hpp>
#include <engine/math/euler-angles.hpp>
#include <engine/math/constants.hpp>

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
