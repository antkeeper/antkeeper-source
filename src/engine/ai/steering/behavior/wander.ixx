// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.ai.steering.behavior.wander;
export import engine.math.vector;
import engine.ai.steering.agent;

export namespace engine::ai::steering::behavior
{
	/// Steers an agent in a continuously shifting random direction on the yaw plane.
	/// @param agent Autonomous agent to steer.
	/// @param distance Distance to the center of the wander circle.
	/// @param noise Maximum wander angle shift, in radians.
	/// @param radius Radius of the wander circle.
	/// @param[in,out] angle Angular coordinate on the wander circle, in radians.
	/// @return Wander force.
	math::fvec3 wander_2d(const agent& agent, float noise, float distance, float radius, float& angle);

	/// Steers an agent in a continuously shifting random direction.
	/// @param agent Autonomous agent to steer.
	/// @param noise Maximum wander angle shift, in radians.
	/// @param distance Distance to the wander sphere.
	/// @param radius Radius of the wander sphere.
	/// @param[in,out] theta Polar wander angle, in radians.
	/// @param[in,out] phi Azimuthal wander angle, in radians.
	/// @return Wander force.
	math::fvec3 wander_3d(const agent& agent, float noise, float distance, float radius, float& theta, float& phi);
}
