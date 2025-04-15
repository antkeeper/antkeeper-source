// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.ai.steering.agent;
export import engine.math.vector;
export import engine.math.quaternion;

export namespace engine::ai::steering
{
	/// Autonomous agent governed by steering behaviors.
	struct agent
	{
		/// Mass of the agent.
		float mass;
	
		/// Cartesian position vector.
		math::fvec3 position;
	
		/// Velocity vector.
		math::fvec3 velocity;
	
		/// Acceleration vector.
		math::fvec3 acceleration;
	
		/// Maximum force.
		float max_force;
	
		/// Maximum speed.
		float max_speed;
	
		/// Maximum speed squared.
		float max_speed_squared;
	
		/// Orientation quaternion.
		math::fquat orientation;
	
		/// Orthonormal basis forward direction vector.
		math::fvec3 forward;
	
		/// Orthonormal basis up direction vector.
		math::fvec3 up;
	};
}
