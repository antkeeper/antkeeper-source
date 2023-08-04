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

#ifndef ANTKEEPER_ANIMATION_IK_GOAL_HPP
#define ANTKEEPER_ANIMATION_IK_GOAL_HPP

#include <engine/animation/ik/ik-solver.hpp>
#include <engine/math/vector.hpp>
#include <memory>

/**
 * IK goal.
 */
class ik_goal
{
public:
	/**
	 * Sets the solver of the IK goal.
	 *
	 * @param solver Goal solver.
	 */
	inline void set_solver(std::shared_ptr<ik_solver> solver) noexcept
	{
		m_solver = solver;
	}
	
	/**
	 * Sets the center of the IK goal.
	 *
	 * @param center Goal center, in world-space.
	 */
	inline void set_center(const math::fvec3& center) noexcept
	{
		m_center = center;
	}
	
	/**
	 * Sets the radius of the IK goal.
	 *
	 * @param radius Goal radius.
	 */
	inline void set_radius(float radius) noexcept
	{
		m_radius = radius;
		m_sqr_radius = m_sqr_radius * m_sqr_radius;
	}
	
	/// Returns the solver of the IK goal.
	[[nodiscard]] inline const std::shared_ptr<ik_solver>& get_solver() const noexcept
	{
		return m_solver;
	}
	
	/// Returns the center of the IK goal.
	[[nodiscard]] inline const math::fvec3& get_center() const noexcept
	{
		return m_center;
	}
	
	/// Returns the radius of the IK goal.
	[[nodiscard]] inline float get_radius() const noexcept
	{
		return m_radius;
	}

private:
	std::shared_ptr<ik_solver> m_solver;
	math::fvec3 m_center{};
	float m_radius{1e-3f};
	float m_sqr_radius{1e-6f};
};

#endif // ANTKEEPER_ANIMATION_IK_GOAL_HPP
