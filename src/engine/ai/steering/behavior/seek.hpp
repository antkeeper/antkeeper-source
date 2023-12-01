// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_AI_STEERING_BEHAVIOR_SEEK_HPP
#define ANTKEEPER_AI_STEERING_BEHAVIOR_SEEK_HPP

#include <engine/ai/steering/agent.hpp>
#include <engine/math/vector.hpp>

namespace ai {
namespace steering {
namespace behavior {

/**
 * Attempts to steer an agent so that it moves toward a target.
 *
 * @param agent Autonomous agent to steer.
 * @param target Target position.
 * @return Seek force.
 */
[[nodiscard]] math::fvec3 seek(const agent& agent, const math::fvec3& target);

} // namespace behavior
} // namespace steering
} // namespace ai

#endif // ANTKEEPER_AI_STEERING_BEHAVIOR_SEEK_HPP
