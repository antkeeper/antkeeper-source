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

#include "game/states/nuptial-flight.hpp"
#include "entity/systems/astronomy.hpp"
#include "entity/systems/orbit.hpp"
#include "entity/systems/control.hpp"
#include "entity/systems/camera.hpp"
#include "entity/components/observer.hpp"
#include "entity/components/camera-follow.hpp"
#include "entity/components/transform.hpp"
#include "entity/components/terrain.hpp"
#include "renderer/material-property.hpp"
#include "animation/screen-transition.hpp"
#include "animation/ease.hpp"
#include "resources/config-file.hpp"
#include "resources/resource-manager.hpp"

namespace game {
namespace state {
namespace nuptial_flight {

void enter(game::context* ctx)
{
	// Find planet EID by name
	entity::id planet_eid = entt::null;
	if (auto it = ctx->named_entities.find("planet"); it != ctx->named_entities.end())
	{
		planet_eid = it->second;
	}
	
	// Replace planet terrain material with cloud material
	entity::component::terrain planet_terrain = ctx->entity_registry->get<entity::component::terrain>(planet_eid);
	planet_terrain.patch_material = ctx->resource_manager->load<material>("cloud.mtl");
	ctx->entity_registry->replace<entity::component::terrain>(planet_eid, planet_terrain);
	
	// Create observer
	auto observer_eid = ctx->entity_registry->create();
	{
		entity::component::observer observer;
		observer.reference_body_eid = planet_eid;
		observer.elevation = 2000.0;
		observer.latitude = 0.0;
		observer.longitude = 0.0;
		observer.camera = ctx->overworld_camera;
		ctx->entity_registry->assign<entity::component::observer>(observer_eid, observer);
		
		// Set reference location of astronomy system
		ctx->astronomy_system->set_reference_body(planet_eid);
		ctx->astronomy_system->set_observer_location(double3{observer.elevation, observer.latitude, observer.longitude});
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
	
	ctx->overworld_scene->update_tweens();
	
	// Pause motion of celestial objects
	ctx->astronomy_system->set_time_scale(0.0);
	ctx->orbit_system->set_time_scale(0.0);
	
	// Start fade in from white
	ctx->fade_transition_color->set_value({1, 1, 1});
	ctx->fade_transition->transition(2.0f, true, ease<float>::in_quad);
}

void exit(game::context* ctx)
{
	// Resume motion of celestial objects
	const double time_scale = ctx->config->get<double>("time_scale");
	ctx->astronomy_system->set_time_scale(time_scale);
	ctx->orbit_system->set_time_scale(time_scale);
}

} // namespace nuptial_flight
} // namespace state
} // namespace game
