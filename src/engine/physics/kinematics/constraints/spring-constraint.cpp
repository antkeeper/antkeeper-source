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

#include <engine/physics/kinematics/constraints/spring-constraint.hpp>

namespace physics {

void spring_constraint::solve(float dt)
{
	if (!m_body_a || !m_body_b)
	{
		return;
	}
	
	// Get radius vectors from centers of mass to spring attachment points
	const math::vector<float, 3> radius_a = m_body_a->get_orientation() * m_point_a;
	const math::vector<float, 3> radius_b = m_body_b->get_orientation() * m_point_b;
	
	// Get world-space spring attachment points
	const math::vector<float, 3> point_a = m_body_a->get_center_of_mass() + radius_a;
	const math::vector<float, 3> point_b = m_body_b->get_center_of_mass() + radius_b;
	
	// Calculate relative velocity between the attachment points
	const math::vector<float, 3> velocity = m_body_b->get_point_velocity(radius_b) - m_body_a->get_point_velocity(radius_a);
	
	// Calculate spring force
	// F = -k * (|x| - d) * (x / |x|) - bv
	const math::vector<float, 3> difference = point_b - point_a;
	const float distance = std::sqrt(math::dot(difference, difference));
	const math::vector<float, 3> force = -m_stiffness * (distance - m_resting_length) * (difference / distance) - velocity * m_damping;
	
	// Apply spring force to bodies at attachment points
	m_body_a->apply_force(-force, radius_a);
	m_body_b->apply_force(force, radius_b);
}

} // namespace physics
