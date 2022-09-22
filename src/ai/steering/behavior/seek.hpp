/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_AI_STEERING_BEHAVIOR_SEEK_HPP
#define ANTKEEPER_AI_STEERING_BEHAVIOR_SEEK_HPP

#include "ai/steering/agent.hpp"
#include "utility/fundamental-types.hpp"

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
float3 seek(const agent& agent, const float3& target);

} // namespace behavior
} // namespace steering
} // namespace ai

#endif // ANTKEEPER_AI_STEERING_BEHAVIOR_SEEK_HPP
