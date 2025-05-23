// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.ai.steering.behavior.wander;
import engine.ai.steering.behavior.seek;
import engine.ai.steering.agent;
import engine.math.quaternion;
import engine.math.functions;

namespace engine::ai::steering::behavior
{
	math::fvec3 wander_2d(const agent& agent, float, float distance, float radius, float& angle)
	{
		// Shift wander angle
		// angle += math::random(-noise, noise);

		// Calculate center of wander circle
		const math::fvec3 center = agent.position + agent.forward * distance;

		// Decompose orientation into swing and twist rotations
		auto [swing, twist] = math::swing_twist(agent.orientation, agent.up);

		// Calculate offset to point on wander circle
		const math::fvec3 offset = (math::angle_axis(angle, agent.up) * agent.forward * radius) * twist;

		// Seek toward point on wander circle
		return seek(agent, center + offset);
	}

	math::fvec3 wander_3d(const agent& agent, float, float distance, float radius, float& theta, float& phi)
	{
		// Shift wander angles
		// theta += math::random(-noise, noise);
		// phi += math::random(-noise, noise);

		// Calculate center of wander sphere
		const math::fvec3 center = agent.position + agent.forward * distance;

		// Convert spherical coordinates to Cartesian point on wander sphere
		const float r_cos_theta = radius * math::cos(theta);
		const math::fvec3 offset =
		{
			r_cos_theta * math::cos(phi),
			r_cos_theta * math::sin(phi),
			radius * math::sin(theta)
		};

		// Seek toward point on wander sphere
		return seek(agent, center + offset);
	}
}
