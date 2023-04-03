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

#include "game/systems/ik-system.hpp"
#include "game/components/ik-component.hpp"
#include <engine/entity/id.hpp>
#include <algorithm>
#include <execution>

ik_system::ik_system(entity::registry& registry):
	updatable_system(registry)
{}

void ik_system::update(float t, float dt)
{
	auto view = registry.view<ik_component>();
	std::for_each
	(
		std::execution::par_unseq,
		view.begin(),
		view.end(),
		[&](auto entity_id)
		{
			const auto& component = view.get<ik_component>(entity_id);
			
			component.rig->solve();
		}
	);
}
