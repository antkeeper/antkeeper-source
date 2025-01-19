// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/spawn.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/scene-component.hpp"
#include <engine/scene/static-mesh.hpp>

entity::id spawn_ant_egg(::game& ctx, const ant_genome& genome, const math::fvec3& position)
{
	// Create entity
	entity::id egg_eid = ctx.entity_registry->create();
	
	// Construct transform component
	transform_component transform_component;
	transform_component.local = math::identity<math::transform<float>>;
	transform_component.local.translation = position;
	transform_component.world = transform_component.local;
	ctx.entity_registry->emplace<::transform_component>(egg_eid, transform_component);
	
	// Construct scene component
	ctx.entity_registry->emplace<::scene_component>(egg_eid, std::make_unique<scene::static_mesh>(genome.egg->phenes.front().model), std::uint8_t{1});
	
	return egg_eid;
}

entity::id spawn_ant_larva(::game& ctx, const ant_genome& genome, const math::fvec3& position)
{
	// Create entity
	entity::id larva_eid = ctx.entity_registry->create();
	
	// Construct transform component
	transform_component transform_component;
	transform_component.local = math::identity<math::transform<float>>;
	transform_component.local.translation = position;
	transform_component.world = transform_component.local;
	ctx.entity_registry->emplace<::transform_component>(larva_eid, transform_component);
	
	// Construct scene component
	ctx.entity_registry->emplace<::scene_component>(larva_eid, std::make_unique<scene::static_mesh>(genome.larva->phenes.front().model), std::uint8_t{1});
	
	return larva_eid;
}

// entity::id spawn_worker_ant(::game& ctx, const ant_genome& genome, const math::fvec3& position)
// {
	// return entt::null;
// }

