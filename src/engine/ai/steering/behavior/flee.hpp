// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_AI_STEERING_BEHAVIOR_FLEE_HPP
#define ANTKEEPER_AI_STEERING_BEHAVIOR_FLEE_HPP

#include <engine/ai/steering/agent.hpp>
#include <engine/math/vector.hpp>

namespace ai {
namespace steering {
namespace behavior {

/// Attempts to steer an agent so that it moves away from a target.
/// @param agent Autonomous agent to steer.
/// @param target Target position.
/// @return Flee force.
math::fvec3 flee(const agent& agent, const math::fvec3& target);

} // namespace behavior
} // namespace steering
} // namespace ai

#endif // ANTKEEPER_AI_STEERING_BEHAVIOR_FLEE_HPP
