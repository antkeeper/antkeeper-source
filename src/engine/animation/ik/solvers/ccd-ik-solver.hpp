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

#ifndef ANTKEEPER_ANIMATION_CCD_IK_SOLVER_HPP
#define ANTKEEPER_ANIMATION_CCD_IK_SOLVER_HPP

#include <engine/animation/ik/ik-solver.hpp>
#include <engine/animation/bone.hpp>
#include <vector>

class ik_rig;

/**
 * Cyclic Coordinate Descent (CCD) IK solver.
 */
class ccd_ik_solver: public ik_solver
{
public:
	/**
	 * Constructs a CCD IK solver.
	 *
	 * @param ik_rig IK rig with which to associate this IK solver.
	 * @param root_bone_index Index of the first bone in the bone chain.
	 * @param effector_bone_index Index of the last bone in the bone chain.
	 * @param chain_length Number of bones in the IK chain.
	 */
	ccd_ik_solver(ik_rig& ik_rig, bone_index_type root_bone_index, bone_index_type effector_bone_index);
	
	/// @name Solving
	/// @{
	
	void solve() override;
	
	/**
	 * Sets the maximum number of solving iterations.
	 *
	 * @param iterations Maximum number of solving iterations.
	 */
	inline void set_max_iterations(std::size_t iterations) noexcept
	{
		m_max_iterations = iterations;
	}
	
	/// Returns the maximum number of solving iterations.
	[[nodiscard]] inline std::size_t get_max_iterations() const noexcept
	{
		return m_max_iterations;
	}
	
	/// @}
	
	/// @name Effector
	/// @{
	
	/**
	 * Sets the position of the end effector.
	 *
	 * @param position Position of the end effector, relative to the tip bone.
	 */
	inline void set_effector_position(const math::fvec3& position) noexcept
	{
		m_effector_position = position;
	}
	
	/// Returns the position of the end effector, relative to the tip bone.
	[[nodiscard]] inline const math::fvec3& get_effector_position() const
	{
		return m_effector_position;
	}
	
	/// @}
	
	/// @name Goal
	/// @{
	
	/**
	 * Sets the center of the IK goal.
	 *
	 * @param center IK goal center, in world-space.
	 */
	inline void set_goal_center(const math::fvec3& center) noexcept
	{
		m_goal_center = center;
	}
	
	/**
	 * Sets the radius of the IK goal.
	 *
	 * @param radius IK goal radius.
	 */
	inline void set_goal_radius(float radius) noexcept
	{
		m_sqr_goal_radius = radius * radius;
	}
	
	/// Returns the center of the IK goal, in world-space.
	[[nodiscard]] inline const math::fvec3& get_goal_center() const
	{
		return m_goal_center;
	}
	
	/// @}
	
private:
	ik_rig* m_ik_rig{nullptr};
	std::size_t m_max_iterations{10};
	std::vector<bone_index_type> m_bone_indices;
	math::fvec3 m_effector_position{0.0f, 0.0f, 0.0f};
	math::fvec3 m_goal_center{0.0f, 0.0f, 0.0f};
	float m_sqr_goal_radius{1e-5f};
};

#endif // ANTKEEPER_ANIMATION_CCD_IK_SOLVER_HPP
