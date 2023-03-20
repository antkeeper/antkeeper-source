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

#include <engine/physics/kinematics/rigid-body.hpp>
#include <algorithm>

namespace physics {

void rigid_body::integrate_forces(float dt) noexcept
{
	// Apply forces
	m_linear_momentum += m_applied_force * dt;
	m_angular_momentum += m_applied_torque * dt;
	
	// Apply damping
	m_linear_momentum *= std::max<float>(0.0f, 1.0f - m_linear_damping * dt);
	m_angular_momentum *= std::max<float>(0.0f, 1.0f - m_angular_damping * dt);
	
	// Update velocities
	m_linear_velocity = m_inverse_mass * m_linear_momentum;
	m_angular_velocity = m_inverse_inertia * m_angular_momentum;
	
	// Clear forces
	m_applied_force = math::vector<float, 3>::zero();
	m_applied_torque = math::vector<float, 3>::zero();
}

void rigid_body::integrate_velocities(float dt) noexcept
{
	// Update center of mass
	m_center_of_mass += m_linear_velocity * dt;
	
	// Update orientation
	const math::quaternion<float> spin = math::quaternion<float>{0.0f, m_angular_velocity * 0.5f} * m_orientation;
	m_orientation = math::normalize(m_orientation + spin * dt);
}

} // namespace physics