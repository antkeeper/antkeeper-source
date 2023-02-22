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

#ifndef ANTKEEPER_AI_STEERING_BEHAVIOR_WANDER_HPP
#define ANTKEEPER_AI_STEERING_BEHAVIOR_WANDER_HPP

#include <engine/ai/steering/agent.hpp>
#include <engine/utility/fundamental-types.hpp>

namespace ai {
namespace steering {
namespace behavior {

/**
 * Steers an agent in a continuously shifting random direction on the yaw plane.
 *
 * @param agent Autonomous agent to steer.
 * @param distance Distance to the center of the wander circle.
 * @param noise Maximum wander angle shift, in radians.
 * @param radius Radius of the wander circle.
 * @param[in,out] angle Angular coordinate on the wander circle, in radians.
 *
 * @return Wander force.
 */
float3 wander_2d(const agent& agent, float noise, float distance, float radius, float& angle);

/**
 * Steers an agent in a continuously shifting random direction.
 *
 * @param agent Autonomous agent to steer.
 * @param distance Distance to the wander sphere.
 * @param radius Radius of the wander sphere.
 * @param delta Maximum angle offset.
 * @param[in,out] theta Polar wander angle, in radians.
 * @param[in,out] phi Azimuthal wander angle, in radians.
 *
 * @return Wander force.
 */
float3 wander_3d(const agent& agent, float noise, float distance, float radius, float& theta, float& phi);

} // namespace behavior
} // namespace steering
} // namespace ai

#endif // ANTKEEPER_AI_STEERING_BEHAVIOR_WANDER_HPP
