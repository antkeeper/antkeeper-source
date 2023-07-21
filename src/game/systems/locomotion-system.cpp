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
#include "game/components/winged-locomotion-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include <engine/entity/id.hpp>
#include <algorithm>
#include <execution>

locomotion_system::locomotion_system(entity::registry& registry):
	updatable_system(registry)
{}

void locomotion_system::update(float t, float dt)
{
	// Legged locomotion
	auto legged_group = registry.group<legged_locomotion_component>(entt::get<rigid_body_component>);
	std::for_each
	(
		std::execution::par_unseq,
		legged_group.begin(),
		legged_group.end(),
		[&](auto entity_id)
		{
			const auto& locomotion = legged_group.get<legged_locomotion_component>(entity_id);
			auto& body = *(legged_group.get<rigid_body_component>(entity_id).body);
			
			// Apply locomotive force
			body.apply_central_force(locomotion.force);
		}
	);
	
	// Winged locomotion
	auto winged_group = registry.group<winged_locomotion_component>(entt::get<rigid_body_component>);
	std::for_each
	(
		std::execution::par_unseq,
		winged_group.begin(),
		winged_group.end(),
		[&](auto entity_id)
		{
			const auto& locomotion = winged_group.get<winged_locomotion_component>(entity_id);
			auto& body = *(winged_group.get<rigid_body_component>(entity_id).body);
			
			const math::fvec3 gravity{0.0f, 9.80665f * 10.0f, 0.0f};
			//const math::fvec3 gravity{0.0f, 0.0f, 0.0f};
			
			// Apply locomotive force
			body.apply_central_force(locomotion.force + gravity * body.get_mass());
		}
	);
}
