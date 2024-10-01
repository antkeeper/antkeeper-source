// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_AI_STEERING_AGENT_HPP
#define ANTKEEPER_AI_STEERING_AGENT_HPP

#include <engine/math/vector.hpp>
#include <engine/math/quaternion.hpp>

namespace ai {
namespace steering {

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

} // namespace steering
} // namespace ai

#endif // ANTKEEPER_AI_STEERING_AGENT_HPP
