// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/ik/constraints/swing-twist-ik-constraint.hpp>
#include <cmath>

void swing_twist_ik_constraint::set_twist_limit(float min_angle, float max_angle)
{
	m_cos_half_twist_min = std::cos(min_angle * 0.5f);
	m_sin_half_twist_min = std::sin(min_angle * 0.5f);
	m_cos_half_twist_max = std::cos(max_angle * 0.5f);
	m_sin_half_twist_max = std::sin(max_angle * 0.5f);
}

void swing_twist_ik_constraint::solve(math::fquat& q)
{
	constexpr math::fvec3 twist_axis{0.0f, 0.0f, 1.0f};
	
	// Decompose rotation into swing and twist components
	auto [swing, twist] = math::swing_twist(q, twist_axis);
	
	// Limit swing
	// ...
	
	// Limit twist
	if (twist.w() < m_cos_half_twist_min)
	{
		twist.w() = m_cos_half_twist_min;
		twist.z() = m_sin_half_twist_min;
	}
	else if (twist.w() > m_cos_half_twist_max)
	{
		twist.w() = m_cos_half_twist_max;
		twist.z() = m_sin_half_twist_max;
	}
	
	// Re-compose rotation from swing and twist components
	q = math::normalize(swing * twist);
}
