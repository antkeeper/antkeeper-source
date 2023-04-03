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

#include <engine/animation/ik/constraints/euler-ik-constraint.hpp>
#include <algorithm>
#include <cmath>

void euler_ik_constraint::solve(math::quaternion<float>& q)
{
	// Derive XYZ angles from quaternion
	const auto x = std::atan2(2.0f * (q.w() * q.x() + q.y() * q.z()), 1.0f - 2.0f * (q.x() * q.x() + q.y() * q.y()));
	const auto y = std::asin(2.0f * (q.w() * q.y() - q.z() * q.x()));
	const auto z = std::atan2(2.0f * (q.w() * q.z() + q.x() * q.y()), 1.0f - 2.0f * (q.y() * q.y() + q.z() * q.z()));
	
	// Constrain and halve angles
	const auto half_constrained_x = std::min<float>(std::max<float>(x, m_min.x()), m_max.x()) * 0.5f;
	const auto half_constrained_y = std::min<float>(std::max<float>(y, m_min.y()), m_max.y()) * 0.5f;
	const auto half_constrained_z = std::min<float>(std::max<float>(z, m_min.z()), m_max.z()) * 0.5f;
	
	// Reconstruct quaternion from constrained, halved angles
	const auto cx = std::cos(half_constrained_x);
	const auto sx = std::sin(half_constrained_x);
	const auto cy = std::cos(half_constrained_y);
	const auto sy = std::sin(half_constrained_y);
	const auto cz = std::cos(half_constrained_z);
	const auto sz = std::sin(half_constrained_z);
	q = math::normalize
	(
		math::quaternion<float>
		{
			cx * cy * cz + sx * sy * sz,
			sx * cy * cz - cx * sy * sz,
			cx * sy * cz + sx * cy * sz,
			cx * cy * sz - sx * sy * cz
		}
	);
}
