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

void swing_twist_ik_constraint::set_twist_limit(float angle_min, float angle_max)
{
	m_cos_half_twist_min = std::cos(angle_min * 0.5f);
	m_sin_half_twist_min = std::sin(angle_min * 0.5f);
	m_cos_half_twist_max = std::cos(angle_max * 0.5f);
	m_sin_half_twist_max = std::sin(angle_max * 0.5f);
}

void swing_twist_ik_constraint::solve(math::fquat& q)
{
	constexpr math::fvec3 twist_axis{0.0f, 0.0f, 1.0f};
	
	// Decompose rotation into swing and twist components
	math::fquat swing;
	math::fquat twist;
	math::swing_twist(q, twist_axis, swing, twist);
	
	// Limit twist
	if (twist.z() < m_sin_half_twist_min)
	{
		twist.z() = m_sin_half_twist_min;
		twist.w() = m_cos_half_twist_min;
	}
	else if (twist.z() > m_sin_half_twist_max)
	{
		twist.z() = m_sin_half_twist_max;
		twist.w() = m_cos_half_twist_max;
	}
	
	// Re-compose rotation from swing and twist components
	q = math::normalize(swing * twist);
}
