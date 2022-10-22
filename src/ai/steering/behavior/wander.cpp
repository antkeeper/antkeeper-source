/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "ai/steering/behavior/wander.hpp"
#include "ai/steering/behavior/seek.hpp"
#include "math/random.hpp"
#include "math/quaternion.hpp"

namespace ai {
namespace steering {
namespace behavior {

float3 wander_2d(const agent& agent, float noise, float distance, float radius, float& angle)
{
	// Shift wander angle
	angle += math::random(-noise, noise);
	
	// Calculate center of wander circle
	const float3 center = agent.position + agent.forward * distance;
	
	// Decompose orientation into swing and twist rotations
	math::quaternion<float> swing, twist;
	math::swing_twist(agent.orientation, agent.up, swing, twist);
	
	// Calculate offset to point on wander circle
	const float3 offset = math::conjugate(twist) * (math::angle_axis(angle, agent.up) * agent.forward * radius);
	
	// Seek toward point on wander circle
	return seek(agent, center + offset);
}

float3 wander_3d(const agent& agent, float noise, float distance, float radius, float& theta, float& phi)
{
	// Shift wander angles
	theta += math::random(-noise, noise);
	phi += math::random(-noise, noise);
	
	// Calculate center of wander sphere
	const float3 center = agent.position + agent.forward * distance;
	
	// Convert spherical coordinates to Cartesian point on wander sphere
	const float r_cos_theta = radius * std::cos(theta);
	const float3 offset =
	{
		r_cos_theta * std::cos(phi),
		r_cos_theta * std::sin(phi),
		radius * std::sin(theta)
	};
	
	// Seek toward point on wander sphere
	return seek(agent, center + offset);
}

} // namespace behavior
} // namespace steering
} // namespace ai
