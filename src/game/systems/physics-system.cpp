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

#include "game/systems/physics-system.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/physics-component.hpp"
#include <engine/entity/id.hpp>

physics_system::physics_system(entity::registry& registry):
	updatable_system(registry)
{}

void physics_system::update(float t, float dt)
{
	integrate(dt);
}

void physics_system::integrate(float dt)
{
	auto group = registry.group<physics_component>(entt::get<transform_component>);
	for (auto entity_id: group)
	{
		auto& body = group.get<physics_component>(entity_id);
		
		// Air resistance
		const float air_drag_coef = 0.58f;
		body.force -= body.velocity * air_drag_coef * body.mass;
		
		// Gravity
		const math::vector<float, 3> weight_force = gravity * body.mass;
		//body.force += weight_force;
		
		body.acceleration = body.force / body.mass;
		body.velocity += body.acceleration * dt;
		
		registry.patch<::transform_component>
		(
			entity_id,
			[&body, dt](auto& transform)
			{
				transform.local.translation += body.velocity * dt;
			}
		);
		
		body.force = math::vector<float, 3>::zero();
	}
}
