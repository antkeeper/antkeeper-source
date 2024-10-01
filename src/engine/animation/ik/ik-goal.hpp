// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_IK_GOAL_HPP
#define ANTKEEPER_ANIMATION_IK_GOAL_HPP

#include <engine/animation/ik/ik-solver.hpp>
#include <engine/math/vector.hpp>
#include <memory>

/// IK goal.
class ik_goal
{
public:
	/// Sets the solver of the IK goal.
	/// @param solver Goal solver.
	inline void set_solver(std::shared_ptr<ik_solver> solver) noexcept
	{
		m_solver = solver;
	}
	
	/// Sets the center of the IK goal.
	/// @param center Goal center, in world-space.
	inline void set_center(const math::fvec3& center) noexcept
	{
		m_center = center;
	}
	
	/// Sets the radius of the IK goal.
	/// @param radius Goal radius.
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
