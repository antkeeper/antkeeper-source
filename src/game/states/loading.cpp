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

#include "game/states/loading.hpp"
#include "game/states/play.hpp"
#include "game/states/splash.hpp"
#include "entity/components/celestial-body.hpp"
#include "entity/components/orbit.hpp"
#include "entity/components/blackbody.hpp"
#include "entity/components/terrain.hpp"
#include "entity/components/atmosphere.hpp"
#include "entity/components/transform.hpp"
#include "entity/systems/astronomy.hpp"
#include "entity/systems/orbit.hpp"
#include "scene/directional-light.hpp"
#include "scene/ambient-light.hpp"
#include "resources/resource-manager.hpp"
#include "application.hpp"
#include "renderer/passes/shadow-map-pass.hpp"

namespace game {
namespace state {
namespace loading {

/// Creates the universe and solar system.
static void cosmogenesis(game::context* ctx);

/// Creates a sun.
static void heliogenesis(game::context* ctx);

/// Creates a planet.
static void planetogenesis(game::context* ctx);

/// Creates a moon.
static void selenogenesis(game::context* ctx);

void enter(game::context* ctx)
{
	// Create universe
	ctx->logger->push_task("Creating the universe");
	try
	{
		cosmogenesis(ctx);
	}
	catch (...)
	{
		ctx->logger->pop_task(EXIT_FAILURE);
		throw;
	}
	ctx->logger->pop_task(EXIT_SUCCESS);
	
	// Determine next game state
	application::state next_state;
	if (ctx->option_quick_start.has_value())
	{
		next_state.name = "play";
		next_state.enter = std::bind(game::state::play::enter, ctx);
		next_state.exit = std::bind(game::state::play::exit, ctx);
	}
	else
	{
		next_state.name = "splash";
		next_state.enter = std::bind(game::state::splash::enter, ctx);
		next_state.exit = std::bind(game::state::splash::exit, ctx);
	}
	
	// Queue next game state
	ctx->app->queue_state(next_state);
}

void exit(game::context* ctx)
{}

void cosmogenesis(game::context* ctx)
{
	// Init time
	const double time = 0.0;
	ctx->astronomy_system->set_universal_time(time);
	ctx->orbit_system->set_universal_time(time);
	
	// Create sun
	ctx->logger->push_task("Creating the sun");
	try
	{
		heliogenesis(ctx);
	}
	catch (...)
	{
		ctx->logger->pop_task(EXIT_FAILURE);
		throw;
	}
	ctx->logger->pop_task(EXIT_SUCCESS);
	
	// Create planet
	ctx->logger->push_task("Creating the planet");
	try
	{
		planetogenesis(ctx);
	}
	catch (...)
	{
		ctx->logger->pop_task(EXIT_FAILURE);
		throw;
	}
	ctx->logger->pop_task(EXIT_SUCCESS);
	
	// Create moon
	ctx->logger->push_task("Creating the moon");
	try
	{
		selenogenesis(ctx);
	}
	catch (...)
	{
		ctx->logger->pop_task(EXIT_FAILURE);
		throw;
	}
	ctx->logger->pop_task(EXIT_SUCCESS);
}

void heliogenesis(game::context* ctx)
{
	// Create solar entity
	auto sun_eid = ctx->entity_registry->create();
	
	// Name solar entity
	ctx->named_entities["sun"] = sun_eid;
	
	// Assign solar celestial body component
	entity::component::celestial_body body;
	body.radius = 6.957e+8;
	body.axial_tilt = math::radians(0.0);
	body.axial_rotation = math::radians(0.0);
	body.angular_frequency = math::radians(0.0);
	ctx->entity_registry->assign<entity::component::celestial_body>(sun_eid, body);
	
	// Assign solar orbit component
	entity::component::orbit orbit;
	orbit.elements.a = 0.0;
	orbit.elements.e = 0.0;
	orbit.elements.i = math::radians(0.0);
	orbit.elements.raan = math::radians(0.0);
	orbit.elements.w = math::radians(0.0);
	orbit.elements.ta = math::radians(0.0);
	ctx->entity_registry->assign<entity::component::orbit>(sun_eid, orbit);
	
	// Assign solar blackbody component
	entity::component::blackbody blackbody;
	blackbody.temperature = 5778.0;
	ctx->entity_registry->assign<entity::component::blackbody>(sun_eid, blackbody);
	
	// Assign solar transform component
	entity::component::transform transform;
	transform.local = math::identity_transform<float>;
	transform.warp = true;
	ctx->entity_registry->assign<entity::component::transform>(sun_eid, transform);
	
	// Create direct sun light scene object
	scene::directional_light* sun_direct = new scene::directional_light();
	
	// Create ambient sun light scene object
	scene::ambient_light* sun_ambient = new scene::ambient_light();
	sun_ambient->set_color({1, 1, 1});
	sun_ambient->set_intensity(0.0f);
	sun_ambient->update_tweens();
	
	// Add sun light scene objects to overworld scene
	ctx->overworld_scene->add_object(sun_direct);
	ctx->overworld_scene->add_object(sun_ambient);
	
	// Pass direct sun light scene object to shadow map pass and astronomy system
	ctx->overworld_shadow_map_pass->set_light(sun_direct);
	ctx->astronomy_system->set_sun_light(sun_direct);
}

void planetogenesis(game::context* ctx)
{
	// Create planetary entity
	auto planet_eid = ctx->entity_registry->create();
	
	// Name planetary entity
	ctx->named_entities["planet"] = planet_eid;
	
	// Assign planetary celestial body component
	entity::component::celestial_body body;
	body.radius = 6.3781e6;
	body.axial_tilt = math::radians(23.4393);
	body.axial_rotation = math::radians(280.46061837504);
	body.angular_frequency = math::radians(360.9856122880876128);
	ctx->entity_registry->assign<entity::component::celestial_body>(planet_eid, body);
	
	// Assign planetary orbit component
	entity::component::orbit orbit;
	orbit.elements.a = 1.496e+11;
	orbit.elements.e = 0.01671123;
	orbit.elements.i = math::radians(-0.00001531);
	orbit.elements.raan = math::radians(0.0);
	const double longitude_periapsis = math::radians(102.93768193);
	orbit.elements.w = longitude_periapsis - orbit.elements.raan;
	orbit.elements.ta = math::radians(100.46457166) - longitude_periapsis;
	ctx->entity_registry->assign<entity::component::orbit>(planet_eid, orbit);
	
	// Assign planetary terrain component
	entity::component::terrain terrain;
	terrain.elevation = [](double, double) -> double
	{
		//return math::random<double>(0.0, 1.0);
		return 0.0;
	};
	terrain.max_lod = 18;
	terrain.patch_material = ctx->resource_manager->load<material>("desert-terrain.mtl");
	ctx->entity_registry->assign<entity::component::terrain>(planet_eid, terrain);
	
	// Assign planetary atmosphere component
	entity::component::atmosphere atmosphere;
	atmosphere.exosphere_altitude = 65e3;
	atmosphere.index_of_refraction = 1.000293;
	atmosphere.rayleigh_density = 2.545e25;
	atmosphere.rayleigh_scale_height = 8000.0;
	atmosphere.mie_density = 14.8875;
	atmosphere.mie_scale_height = 1200.0;
	atmosphere.mie_anisotropy = 0.8;
	ctx->entity_registry->assign<entity::component::atmosphere>(planet_eid, atmosphere);
	
	// Assign planetary transform component
	entity::component::transform transform;
	transform.local = math::identity_transform<float>;
	transform.warp = true;
	ctx->entity_registry->assign<entity::component::transform>(planet_eid, transform);
	
	// Pass planet to astronomy system as reference body
	ctx->astronomy_system->set_reference_body(planet_eid);
	
	// Load sky model
	ctx->overworld_sky_pass->set_sky_model(ctx->resource_manager->load<model>("sky-dome.mdl"));
}

void selenogenesis(game::context* ctx)
{
	// Create lunar entity
	auto moon_eid = ctx->entity_registry->create();
	
	// Name lunar entity
	ctx->named_entities["moon"] = moon_eid;
	
	// Pass moon model to sky pass
	ctx->overworld_sky_pass->set_moon_model(ctx->resource_manager->load<model>("moon.mdl"));
}

} // namespace loading
} // namespace state
} // namespace game
