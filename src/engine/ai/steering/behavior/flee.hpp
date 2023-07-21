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

#ifndef ANTKEEPER_AI_STEERING_BEHAVIOR_FLEE_HPP
#define ANTKEEPER_AI_STEERING_BEHAVIOR_FLEE_HPP

#include <engine/ai/steering/agent.hpp>
#include <engine/math/vector.hpp>

namespace ai {
namespace steering {
namespace behavior {

/**
 * Attempts to steer an agent so that it moves away from a target.
 *
 * @param agent Autonomous agent to steer.
 * @param target Target position.
 * @return Flee force.
 */
math::fvec3 flee(const agent& agent, const math::fvec3& target);

} // namespace behavior
} // namespace steering
} // namespace ai

#endif // ANTKEEPER_AI_STEERING_BEHAVIOR_FLEE_HPP
