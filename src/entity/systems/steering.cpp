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

namespace entity {
namespace system {

steering::steering(entity::registry& registry):
	updatable(registry)
{}

void steering::update(double t, double dt)
{
	registry.view<component::transform, component::steering>().each(
		[&](entity::id entity_id, auto& transform, auto& boid)
		{
			// Accelerate
			boid.velocity += boid.acceleration;
			if (math::dot(boid.velocity, boid.velocity) > boid.max_speed * boid.max_speed)
			{
				boid.velocity = math::normalize(boid.velocity) * boid.max_speed;
			}
			
			// Clear acceleration
			boid.acceleration = {0, 0, 0};
			
			// Move
			transform.local.translation += boid.velocity;
		});
}

void steering::wander()
{
	
}

} // namespace system
} // namespace entity
