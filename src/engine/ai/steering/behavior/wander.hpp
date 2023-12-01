// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_AI_STEERING_BEHAVIOR_WANDER_HPP
#define ANTKEEPER_AI_STEERING_BEHAVIOR_WANDER_HPP

#include <engine/ai/steering/agent.hpp>
#include <engine/math/vector.hpp>

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
math::fvec3 wander_2d(const agent& agent, float noise, float distance, float radius, float& angle);

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
math::fvec3 wander_3d(const agent& agent, float noise, float distance, float radius, float& theta, float& phi);

} // namespace behavior
} // namespace steering
} // namespace ai

#endif // ANTKEEPER_AI_STEERING_BEHAVIOR_WANDER_HPP
