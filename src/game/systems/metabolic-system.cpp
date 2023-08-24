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

#include "game/systems/metabolic-system.hpp"
#include "game/components/isometric-growth-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include <execution>

metabolic_system::metabolic_system(entity::registry& registry):
	updatable_system(registry)
{}

void metabolic_system::update(float t, float dt)
{
	// Scale timestep
	const auto scaled_timestep = dt * m_time_scale;
	
	// Handle isometric growth
	auto isometric_growth_group = registry.group<isometric_growth_component>(entt::get<rigid_body_component>);
	std::for_each
	(
		std::execution::seq,
		isometric_growth_group.begin(),
		isometric_growth_group.end(),
		[&](auto entity_id)
		{
			auto& growth = isometric_growth_group.get<isometric_growth_component>(entity_id);
			auto& rigid_body = *isometric_growth_group.get<rigid_body_component>(entity_id).body;
			
			rigid_body.set_scale(rigid_body.get_scale() + growth.rate * scaled_timestep);
		}
	);
}
