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

#ifndef ANTKEEPER_AI_STEERING_AGENT_HPP
#define ANTKEEPER_AI_STEERING_AGENT_HPP

#include <engine/utility/fundamental-types.hpp>
#include <engine/math/quaternion.hpp>

namespace ai {
namespace steering {

/**
 * Autonomous agent governed by steering behaviors.
 */
struct agent
{
	/// Mass of the agent.
	float mass;
	
	/// Cartesian position vector.
	float3 position;
	
	/// Velocity vector.
	float3 velocity;
	
	/// Acceleration vector.
	float3 acceleration;
	
	/// Maximum force.
	float max_force;
	
	/// Maximum speed.
	float max_speed;
	
	/// Maximum speed squared.
	float max_speed_squared;
	
	/// Orientation quaternion.
	math::quaternion<float> orientation;
	
	/// Orthonormal basis forward direction vector.
	float3 forward;
	
	/// Orthonormal basis up direction vector.
	float3 up;
};

} // namespace steering
} // namespace ai

#endif // ANTKEEPER_AI_STEERING_AGENT_HPP
