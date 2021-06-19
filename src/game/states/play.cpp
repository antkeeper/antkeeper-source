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

#include "game/states/play.hpp"
#include "entity/archetype.hpp"
#include "entity/commands.hpp"
#include "entity/components/observer.hpp"
#include "entity/components/camera-follow.hpp"
#include "entity/components/transform.hpp"
#include "entity/systems/astronomy.hpp"
#include "entity/systems/control.hpp"
#include "entity/systems/camera.hpp"
#include "animation/screen-transition.hpp"
#include "animation/ease.hpp"
#include "resources/resource-manager.hpp"

namespace game {
namespace state {
namespace play {

void enter(game::context* ctx)
{
	// Find planet EID by name
	entity::id planet_eid = entt::null;
	if (auto it = ctx->named_entities.find("planet"); it != ctx->named_entities.end())
	{
		planet_eid = it->second;
	}
	
	// Create observer
	auto observer_eid = ctx->entity_registry->create();
	{
		entity::component::observer observer;
		observer.reference_body_eid = planet_eid;
		observer.elevation = 0.0;
		observer.latitude = 0.0;
		observer.longitude = 0.0;
		observer.camera = ctx->overworld_camera;
		ctx->entity_registry->assign<entity::component::observer>(observer_eid, observer);
		
		// Set reference location of astronomy system
		ctx->astronomy_system->set_reference_body(planet_eid);
		ctx->astronomy_system->set_observer_location(double3{0.0, math::radians(0.0), math::radians(0.0)});
	}
	
	// Create camera focal point
	{
		entity::component::transform focal_point_transform;
		focal_point_transform.local = math::identity_transform<float>;
		focal_point_transform.warp = true;
		ctx->entity_registry->assign_or_replace<entity::component::transform>(ctx->focal_point_entity, focal_point_transform);
		
		entity::component::camera_follow focal_point_follow;
		ctx->entity_registry->assign_or_replace<entity::component::camera_follow>(ctx->focal_point_entity, focal_point_follow);
	}
	
	// Setup camera
	ctx->overworld_camera->look_at({0, 0, 1}, {0, 0, 0}, {0, 1, 0});
	ctx->overworld_camera->set_exposure(-14.5f);
	ctx->camera_system->set_camera(ctx->overworld_camera);
	
	entity::system::control* control_system = ctx->control_system;
	control_system->update(0.0, 0.0);
	control_system->set_nest(nullptr);
	
	// Create larva
	{
		entity::archetype* larva_archetype = ctx->resource_manager->load<entity::archetype>("ant-larva.ent");
		auto larva_eid = larva_archetype->create(*ctx->entity_registry);
		entity::command::warp_to(*ctx->entity_registry, larva_eid, {50, 0.1935f, 0});
	}
	
	ctx->overworld_scene->update_tweens();
	
	// Start fade in
	ctx->fade_transition->transition(1.0f, true, ease<float>::in_quad);
}

void exit(game::context* ctx)
{}

} // namespace play
} // namespace state
} // namespace game
