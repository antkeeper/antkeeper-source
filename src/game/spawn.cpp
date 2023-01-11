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
#include "game/component/transform.hpp"
#include "game/component/model.hpp"

namespace game {

entity::id spawn_ant_egg(game::context& ctx, const ant::genome& genome, bool fertilized, const float3& position)
{
	// Create entity
	entity::id egg_eid = ctx.entity_registry->create();
	
	// Construct transform component
	component::transform transform_component;
	transform_component.local = math::transform<float>::identity;
	transform_component.local.translation = position;
	transform_component.world = transform_component.local;
	transform_component.warp = true;
	ctx.entity_registry->emplace<component::transform>(egg_eid, transform_component);
	
	// Construct model component
	component::model model_component;
	model_component.render_model = genome.egg->phene.model;
	model_component.instance_count = 0;
	model_component.layers = ~0;
	ctx.entity_registry->emplace<component::model>(egg_eid, model_component);
	
	return egg_eid;
}

entity::id spawn_ant_larva(game::context& ctx, const ant::genome& genome, const float3& position)
{
	// Create entity
	entity::id larva_eid = ctx.entity_registry->create();
	
	// Construct transform component
	component::transform transform_component;
	transform_component.local = math::transform<float>::identity;
	transform_component.local.translation = position;
	transform_component.world = transform_component.local;
	transform_component.warp = true;
	ctx.entity_registry->emplace<component::transform>(larva_eid, transform_component);
	
	// Construct model component
	component::model model_component;
	model_component.render_model = genome.larva->phene.model;
	model_component.instance_count = 0;
	model_component.layers = ~0;
	ctx.entity_registry->emplace<component::model>(larva_eid, model_component);
	
	return larva_eid;
}

entity::id spawn_worker_ant(game::context& ctx, const ant::genome& genome, const float3& position)
{
	return entt::null;
}

} // namespace game
