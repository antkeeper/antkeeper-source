// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_AI_STEERING_HPP
#define ANTKEEPER_AI_STEERING_HPP

namespace ai {

/// Autonomous agent steering.
/// @see Reynolds, Craig. (2002). Steering Behaviors For Autonomous Characters.
namespace steering {}

#include <engine/ai/steering/agent.hpp>
#include <engine/ai/steering/behavior/flee.hpp>
#include <engine/ai/steering/behavior/seek.hpp>
#include <engine/ai/steering/behavior/wander.hpp>

} // namespace ai

#endif // ANTKEEPER_AI_STEERING_HPP
