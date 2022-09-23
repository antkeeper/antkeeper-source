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

#include "entity/systems/steering.hpp"
#include "entity/components/steering.hpp"
#include "entity/components/transform.hpp"
#include "entity/id.hpp"
#include "ai/steering/behavior/wander.hpp"
#include "ai/steering/behavior/seek.hpp"
#include "config.hpp"

namespace entity {
namespace system {

steering::steering(entity::registry& registry):
	updatable(registry)
{}

void steering::update(double t, double dt)
{
	registry.view<component::steering, component::transform>().each
	(
		[&](entity::id entity_id, auto& steering, auto& transform)
		{
			auto& agent = steering.agent;
			
			// Update agent orientation
			agent.orientation = transform.local.rotation;
			
			// Accumulate forces
			float3 force = {0, 0, 0};
			if (steering.wander_weight)
			{
				//force += ai::steering::behavior::wander_2d(agent, steering.wander_noise * dt, steering.wander_distance, steering.wander_radius, steering.wander_angle) * steering.wander_weight;
				force += ai::steering::behavior::wander_3d(agent, steering.wander_noise * dt, steering.wander_distance, steering.wander_radius, steering.wander_angle, steering.wander_angle2) * steering.wander_weight;
			}
			if (steering.seek_weight)
			{
				force += ai::steering::behavior::seek(agent, steering.seek_target) * steering.seek_weight;
			}
			
			// Normalize force
			if (steering.sum_weights)
				force /= steering.sum_weights;
			
			// Accelerate
			agent.acceleration = force / agent.mass;
			agent.velocity += agent.acceleration * static_cast<float>(dt);
			
			// Limit speed
			const float speed_squared = math::length_squared(agent.velocity);
			if (speed_squared > agent.max_speed_squared)
			{
				const float speed = std::sqrt(speed_squared);
				agent.velocity = (agent.velocity / speed) * agent.max_speed;
			}
			
			// Move agent
			agent.position += agent.velocity * static_cast<float>(dt);
			
			// Rotate agent
			if (speed_squared)
			{
				agent.orientation = math::look_rotation(agent.velocity / std::sqrt(speed_squared), agent.up);
				agent.forward = agent.orientation * config::global_forward;
				agent.up = agent.orientation * config::global_up;
			}
			
			// Update transform
			transform.local.translation = agent.position;
			transform.local.rotation = agent.orientation;
		}
	);
}

} // namespace system
} // namespace entity
