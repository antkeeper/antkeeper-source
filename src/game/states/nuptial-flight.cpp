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
#include "entity/archetype.hpp"
#include "entity/systems/astronomy.hpp"
#include "entity/systems/orbit.hpp"
#include "entity/systems/camera.hpp"
#include "entity/components/observer.hpp"
#include "entity/components/transform.hpp"
#include "entity/components/terrain.hpp"
#include "entity/commands.hpp"
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
	// Switch to surface camera
	ctx->underground_camera->set_active(false);
	ctx->surface_camera->set_active(true);
	
	// Find planet EID by name
	auto planet_eid = entity::command::find(*ctx->entity_registry, "planet");
	
	// Remove terrain component from planet (if any)
	if (ctx->entity_registry->has<entity::component::terrain>(planet_eid))
		ctx->entity_registry->remove<entity::component::terrain>(planet_eid);
	
	// Enable clouds in sky pass
	ctx->surface_sky_pass->set_clouds_model(ctx->resource_manager->load<model>("cloud-plane.mdl"));
	
	// Create observer
	auto observer_eid = ctx->entity_registry->create();
	{
		entity::component::observer observer;
		observer.reference_body_eid = planet_eid;
		observer.elevation = 2000.0;
		observer.latitude = 0.0;
		observer.longitude = 0.0;
		observer.camera = ctx->surface_camera;
		ctx->entity_registry->assign<entity::component::observer>(observer_eid, observer);
		
		// Set reference location of astronomy system
		ctx->astronomy_system->set_reference_body(planet_eid);
		ctx->astronomy_system->set_observer_location(double3{observer.elevation, observer.latitude, observer.longitude});
	}
	
	// Create wing
	entity::archetype* ant_forewing_archetype = ctx->resource_manager->load<entity::archetype>("ant-forewing.ent");
	auto forewing_eid = ctx->entity_registry->create();
	//ant_forewing_archetype->assign(*ctx->entity_registry, forewing_eid);
	
	// Create eye
	entity::archetype* ant_round_eye_archetype = ctx->resource_manager->load<entity::archetype>("ant-round-eye.ent");
	auto ant_round_eye_eid = ctx->entity_registry->create();
	ant_round_eye_archetype->assign(*ctx->entity_registry, ant_round_eye_eid);
	entity::command::assign_render_layers(*ctx->entity_registry, ant_round_eye_eid, 0b10);
	
	// Create green orb ring
	entity::archetype* orb_ring_archetype = ctx->resource_manager->load<entity::archetype>("orb-ring.ent");
	auto orb_ring_eid = ctx->entity_registry->create();
	//orb_ring_archetype->assign(*ctx->entity_registry, orb_ring_eid);
	
	// Setup camera
	ctx->surface_camera->look_at({0, 0, 1}, {0, 0, 0}, {0, 1, 0});
	ctx->surface_camera->set_exposure(-14.5f);
	
	ctx->surface_scene->update_tweens();
	
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
