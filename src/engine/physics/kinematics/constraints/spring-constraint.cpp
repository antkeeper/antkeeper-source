// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.physics.kinematics.spring_constraint;
import engine.math.functions;

namespace engine::physics
{
	void spring_constraint::solve(float)
	{
		if (!m_body_a || !m_body_b)
		{
			return;
		}

		// Get radius vectors from centers of mass to spring attachment points
		const auto radius_a = m_body_a->get_orientation() * m_point_a;
		const auto radius_b = m_body_b->get_orientation() * m_point_b;

		// Get world-space spring attachment points
		const auto point_a = m_body_a->get_position() + radius_a;
		const auto point_b = m_body_b->get_position() + radius_b;

		// Calculate relative velocity between the attachment points
		const auto velocity = m_body_b->get_point_velocity(radius_b) - m_body_a->get_point_velocity(radius_a);

		// Calculate spring force
		// F = -k * (|x| - d) * (x / |x|) - bv
		const auto difference = point_b - point_a;
		const float distance = math::sqrt(math::dot(difference, difference));
		const auto force = -m_stiffness * (distance - m_resting_length) * (difference / distance) - velocity * m_damping;

		// Apply spring force to bodies at attachment points
		m_body_a->apply_force(-force, radius_a);
		m_body_b->apply_force(force, radius_b);
	}
}
