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

#include "ai/steering/behavior/seek.hpp"

namespace ai {
namespace steering {
namespace behavior {

float3 seek(const agent& agent, const float3& target)
{
	float3 force = {0, 0, 0};
	const float3 difference = target - agent.position;
	const float sqr_distance = math::dot(difference, difference);
	
	if (sqr_distance)
	{
		const float inverse_distance = 1.0f / std::sqrt(sqr_distance);
		force = difference * inverse_distance * agent.max_force;
		force -= agent.velocity;
	}
	
	return force;
}

} // namespace behavior
} // namespace steering
} // namespace ai
