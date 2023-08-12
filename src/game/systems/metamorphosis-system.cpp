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

#include "game/systems/metamorphosis-system.hpp"
#include "game/components/egg-component.hpp"
#include "game/components/scene-component.hpp"
#include "game/components/ant-genome-component.hpp"
#include <engine/scene/skeletal-mesh.hpp>
#include <engine/debug/log.hpp>
#include <execution>

metamorphosis_system::metamorphosis_system(entity::registry& registry):
	updatable_system(registry)
{}

void metamorphosis_system::update(float t, float dt)
{
	auto egg_group = registry.group<egg_component>(entt::get<ant_genome_component>);
	std::for_each
	(
		std::execution::seq,
		egg_group.begin(),
		egg_group.end(),
		[&](auto entity_id)
		{
			auto& egg = egg_group.get<egg_component>(entity_id);
			if (egg.elapsed_incubation_time >= egg.incubation_period)
			{
				return;
			}
			
			egg.elapsed_incubation_time += dt * m_time_scale;
			if (egg.elapsed_incubation_time >= egg.incubation_period)
			{
				const auto& genome = *egg_group.get<ant_genome_component>(entity_id).genome;
				const auto layer_mask = registry.get<scene_component>(entity_id).layer_mask;
				
				registry.erase<scene_component>(entity_id);
				registry.emplace<scene_component>(entity_id, std::make_shared<scene::skeletal_mesh>(genome.larva->phenes.front().model), layer_mask);
			}
		}
	);
}
