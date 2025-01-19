// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/ik/constraints/euler-ik-constraint.hpp>
#include <algorithm>
#include <cmath>

void euler_ik_constraint::solve(math::fquat& q)
{
	// Store w-component of quaternion
	float old_w = q.w();
	
	// Derive Euler angles from quaternion
	auto angles = math::euler_from_quat(m_rotation_sequence, q);
	
	// Constrain Euler angles
	angles = math::clamp(angles, m_min_angles, m_max_angles);
	
	// Rebuild quaternion from constrained Euler angles
	q = math::euler_to_quat(m_rotation_sequence, angles);
	
	// Restore quaternion sign
	q.w() = std::copysign(q.w(), old_w);
}
