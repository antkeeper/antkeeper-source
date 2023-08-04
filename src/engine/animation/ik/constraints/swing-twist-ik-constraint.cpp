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
