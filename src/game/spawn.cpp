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

#include "game/spawn.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/scene-component.hpp"
#include <engine/scene/static-mesh.hpp>

entity::id spawn_ant_egg(::game& ctx, const ant_genome& genome, bool fertilized, const float3& position)
{
	// Create entity
	entity::id egg_eid = ctx.entity_registry->create();
	
	// Construct transform component
	transform_component transform_component;
	transform_component.local = math::transform<float>::identity;
	transform_component.local.translation = position;
	transform_component.world = transform_component.local;
	ctx.entity_registry->emplace<::transform_component>(egg_eid, transform_component);
	
	// Construct scene component
	ctx.entity_registry->emplace<::scene_component>(egg_eid, std::make_unique<scene::static_mesh>(genome.egg->phenes.front().model), std::uint8_t{1});
	
	return egg_eid;
}

entity::id spawn_ant_larva(::game& ctx, const ant_genome& genome, const float3& position)
{
	// Create entity
	entity::id larva_eid = ctx.entity_registry->create();
	
	// Construct transform component
	transform_component transform_component;
	transform_component.local = math::transform<float>::identity;
	transform_component.local.translation = position;
	transform_component.world = transform_component.local;
	ctx.entity_registry->emplace<::transform_component>(larva_eid, transform_component);
	
	// Construct scene component
	ctx.entity_registry->emplace<::scene_component>(larva_eid, std::make_unique<scene::static_mesh>(genome.larva->phenes.front().model), std::uint8_t{1});
	
	return larva_eid;
}

entity::id spawn_worker_ant(::game& ctx, const ant_genome& genome, const float3& position)
{
	return entt::null;
}

