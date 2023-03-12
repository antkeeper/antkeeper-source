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

#include "game/systems/locomotion-system.hpp"
#include "game/components/legged-locomotion-component.hpp"
#include "game/components/physics-component.hpp"
#include <engine/entity/id.hpp>
#include <algorithm>
#include <execution>

locomotion_system::locomotion_system(entity::registry& registry):
	updatable_system(registry)
{}

void locomotion_system::update(float t, float dt)
{
	auto group = registry.group<legged_locomotion_component>(entt::get<physics_component>);
	std::for_each
	(
		std::execution::par_unseq,
		group.begin(),
		group.end(),
		[&](auto entity_id)
		{
			const auto& locomotion = group.get<legged_locomotion_component>(entity_id);
			auto& body = group.get<physics_component>(entity_id);
			
			// Apply locomotion force
			body.force += locomotion.force;
			
			// Apply friction
			const float friction_coef = 2.0f;
			body.force -= body.velocity * friction_coef * body.mass;
		}
	);
}
