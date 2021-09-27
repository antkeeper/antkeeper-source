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
#include "application.hpp"
#include "astro/illuminance.hpp"
#include "color/color.hpp"
#include "entity/components/atmosphere.hpp"
#include "entity/components/blackbody.hpp"
#include "entity/components/celestial-body.hpp"
#include "entity/components/orbit.hpp"
#include "entity/components/terrain.hpp"
#include "entity/components/transform.hpp"
#include "entity/systems/astronomy.hpp"
#include "entity/systems/orbit.hpp"
#include "entity/commands.hpp"
#include "game/states/nuptial-flight.hpp"
#include "game/states/splash.hpp"
#include "game/states/forage.hpp"
#include "geom/spherical.hpp"
#include "gl/drawing-mode.hpp"
#include "gl/vertex-array.hpp"
#include "gl/vertex-attribute-type.hpp"
#include "gl/vertex-buffer.hpp"
#include "physics/light/photometry.hpp"
#include "physics/orbit/orbit.hpp"
#include "renderer/material.hpp"
#include "renderer/model.hpp"
#include "renderer/passes/shadow-map-pass.hpp"
#include "renderer/vertex-attributes.hpp"
#include "resources/resource-manager.hpp"
#include "scene/ambient-light.hpp"
#include "scene/directional-light.hpp"
#include "resources/config-file.hpp"
#include "utility/timestamp.hpp"

namespace game {
namespace state {
namespace loading {

/// Creates or loads control configuration
static void load_controls(game::context* ctx);

/// Creates the universe and solar system.
static void cosmogenesis(game::context* ctx);

/// Creates a sun.
static void heliogenesis(game::context* ctx);

/// Creates a planet.
static void planetogenesis(game::context* ctx);

/// Creates a moon.
static void selenogenesis(game::context* ctx);

/// Creates fixed stars.
static void extrasolar_heliogenesis(game::context* ctx);

/// Creates an ant colony
static void colonigenesis(game::context* ctx);

void enter(game::context* ctx)
{
	// Load controls
	ctx->logger->push_task("Loading controls");
	try
	{
		load_controls(ctx);
	}
	catch (...)
	{
		ctx->logger->pop_task(EXIT_FAILURE);
	}
	ctx->logger->pop_task(EXIT_SUCCESS);
	
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
		next_state.name = "forage";
		next_state.enter = std::bind(game::state::forage::enter, ctx);
		next_state.exit = std::bind(game::state::forage::exit, ctx);
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

void load_controls(game::context* ctx)
{
	// Toggle fullscreen
	if (!ctx->controls.count("toggle_fullscreen"))
	{
		input::control* control = new input::control();
		ctx->input_event_router->add_mapping(input::key_mapping(control, nullptr, input::scancode::f11));
		ctx->controls["toggle_fullscreen"] = control;
	}
	ctx->controls["toggle_fullscreen"]->set_activated_callback
	(
		[ctx]()
		{
			bool fullscreen = !ctx->app->is_fullscreen();
			
			ctx->app->set_fullscreen(fullscreen);
			
			if (!fullscreen)
			{
				int2 resolution = ctx->config->get<int2>("windowed_resolution");
				ctx->app->resize_window(resolution.x, resolution.y);
			}
			
			ctx->config->set<int>("fullscreen", (fullscreen) ? 1 : 0);
		}
	);
	
	// Screenshot
	if (!ctx->controls.count("screenshot"))
	{
		input::control* control = new input::control();
		ctx->input_event_router->add_mapping(input::key_mapping(control, nullptr, input::scancode::f12));
		ctx->controls["screenshot"] = control;
	}
	ctx->controls["screenshot"]->set_activated_callback
	(
		[ctx]()
		{
			std::string path = ctx->screenshots_path + "antkeeper-" + timestamp() + ".png";
			ctx->app->save_frame(path);
		}
	);
	
	// Menu back
	if (!ctx->controls.count("menu_back"))
	{
		input::control* control = new input::control();
		ctx->input_event_router->add_mapping(input::key_mapping(control, nullptr, input::scancode::escape));
		ctx->input_event_router->add_mapping(input::key_mapping(control, nullptr, input::scancode::backspace));
		ctx->controls["menu_back"] = control;
	}
	ctx->controls["menu_back"]->set_activated_callback
	(
		std::bind(&application::close, ctx->app, 0)
	);
	
	// Dolly forward
	if (!ctx->controls.count("dolly_forward"))
	{
		input::control* control = new input::control();
		ctx->input_event_router->add_mapping(input::key_mapping(control, nullptr, input::scancode::w));
		ctx->input_event_router->add_mapping(input::game_controller_axis_mapping(control, nullptr, input::game_controller_axis::left_y, true));
		ctx->controls["dolly_forward"] = control;
	}
	
	// Dolly backward
	if (!ctx->controls.count("dolly_backward"))
	{
		input::control* control = new input::control();
		ctx->input_event_router->add_mapping(input::key_mapping(control, nullptr, input::scancode::s));
		ctx->input_event_router->add_mapping(input::game_controller_axis_mapping(control, nullptr, input::game_controller_axis::left_y, false));
		ctx->controls["dolly_backward"] = control;
	}
	
	// Truck left
	if (!ctx->controls.count("truck_left"))
	{
		input::control* control = new input::control();
		ctx->input_event_router->add_mapping(input::key_mapping(control, nullptr, input::scancode::a));
		ctx->input_event_router->add_mapping(input::game_controller_axis_mapping(control, nullptr, input::game_controller_axis::left_x, true));
		ctx->controls["truck_left"] = control;
	}
	
	// Truck right
	if (!ctx->controls.count("truck_right"))
	{
		input::control* control = new input::control();
		ctx->input_event_router->add_mapping(input::key_mapping(control, nullptr, input::scancode::d));
		ctx->input_event_router->add_mapping(input::game_controller_axis_mapping(control, nullptr, input::game_controller_axis::left_x, false));
		ctx->controls["truck_right"] = control;
	}
	
	// Pedestal up
	if (!ctx->controls.count("pedestal_up"))
	{
		input::control* control = new input::control();
		ctx->input_event_router->add_mapping(input::mouse_wheel_mapping(control, nullptr, input::mouse_wheel_axis::positive_y));
		ctx->controls["pedestal_up"] = control;
	}
	
	// Pedestal down
	if (!ctx->controls.count("pedestal_down"))
	{
		input::control* control = new input::control();
		ctx->input_event_router->add_mapping(input::mouse_wheel_mapping(control, nullptr, input::mouse_wheel_axis::negative_y));
		ctx->controls["pedestal_down"] = control;
	}
	
	// Move slow
	if (!ctx->controls.count("move_slow"))
	{
		input::control* control = new input::control();
		ctx->input_event_router->add_mapping(input::key_mapping(control, nullptr, input::scancode::left_ctrl));
		ctx->controls["move_slow"] = control;
	}
	
	// Move fast
	if (!ctx->controls.count("move_fast"))
	{
		input::control* control = new input::control();
		ctx->input_event_router->add_mapping(input::key_mapping(control, nullptr, input::scancode::left_shift));
		ctx->controls["move_fast"] = control;
	}
	
	// Mouse rotate
	if (!ctx->controls.count("mouse_rotate"))
	{
		input::control* control = new input::control();
		ctx->input_event_router->add_mapping(input::mouse_button_mapping(control, nullptr, 3));
		ctx->input_event_router->add_mapping(input::key_mapping(control, nullptr, input::scancode::left_alt));
		ctx->controls["mouse_rotate"] = control;
	}
	
	// Mouse left
	if (!ctx->controls.count("mouse_left"))
	{
		input::control* control = new input::control();
		ctx->input_event_router->add_mapping(input::mouse_motion_mapping(control, nullptr, input::mouse_motion_axis::negative_x));
		ctx->controls["mouse_left"] = control;
	}
	
	// Mouse right
	if (!ctx->controls.count("mouse_right"))
	{
		input::control* control = new input::control();
		ctx->input_event_router->add_mapping(input::mouse_motion_mapping(control, nullptr, input::mouse_motion_axis::positive_x));
		ctx->controls["mouse_right"] = control;
	}
	
	// Mouse up
	if (!ctx->controls.count("mouse_up"))
	{
		input::control* control = new input::control();
		ctx->input_event_router->add_mapping(input::mouse_motion_mapping(control, nullptr, input::mouse_motion_axis::negative_y));
		ctx->controls["mouse_up"] = control;
	}
	
	// Mouse down
	if (!ctx->controls.count("mouse_down"))
	{
		input::control* control = new input::control();
		ctx->input_event_router->add_mapping(input::mouse_motion_mapping(control, nullptr, input::mouse_motion_axis::positive_y));
		ctx->controls["mouse_down"] = control;
	}
}

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
	
	// Create fixed stars
	ctx->logger->push_task("Creating fixed stars");
	try
	{
		extrasolar_heliogenesis(ctx);
	}
	catch (...)
	{
		ctx->logger->pop_task(EXIT_FAILURE);
		throw;
	}
	ctx->logger->pop_task(EXIT_SUCCESS);
	
	// Create ant colony
	ctx->logger->push_task("Creating ant colony");
	try
	{
		colonigenesis(ctx);
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
	entity::id sun_eid = ctx->entity_registry->create();
	ctx->entities["sun"] = sun_eid;
	
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
	
	// Add sun light scene objects to surface scene
	ctx->surface_scene->add_object(sun_direct);
	ctx->surface_scene->add_object(sun_ambient);
	
	// Pass direct sun light scene object to shadow map pass and astronomy system
	ctx->surface_shadow_map_pass->set_light(sun_direct);
	ctx->astronomy_system->set_sun_light(sun_direct);
}

void planetogenesis(game::context* ctx)
{
	// Create planetary entity
	entity::id planet_eid = ctx->entity_registry->create();
	ctx->entities["planet"] = planet_eid;
	
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
	terrain.max_lod = 0;
	terrain.patch_material = nullptr;
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
	ctx->surface_sky_pass->set_sky_model(ctx->resource_manager->load<model>("sky-dome.mdl"));
}

void selenogenesis(game::context* ctx)
{
	// Create lunar entity
	entity::id moon_eid = ctx->entity_registry->create();
	ctx->entities["moon"] = moon_eid;
	
	// Pass moon model to sky pass
	ctx->surface_sky_pass->set_moon_model(ctx->resource_manager->load<model>("moon.mdl"));
}

void extrasolar_heliogenesis(game::context* ctx)
{
	// Load star catalog
	string_table* star_catalog = ctx->resource_manager->load<string_table>("stars.csv");
	
	// Allocate star catalog vertex data
	std::size_t star_count = 0;
	if (star_catalog->size() > 0)
		star_count = star_catalog->size() - 1;
	std::size_t star_vertex_size = 6;
	std::size_t star_vertex_stride = star_vertex_size * sizeof(float);
	float* star_vertex_data = new float[star_count * star_vertex_size];
	float* star_vertex = star_vertex_data;
	
	// Build star catalog vertex data
	for (std::size_t i = 1; i < star_catalog->size(); ++i)
	{
		const string_table_row& catalog_row = (*star_catalog)[i];
		
		double ra = 0.0;
		double dec = 0.0;
		double vmag = 0.0;
		double bv_color = 0.0;
		
		// Parse star catalog entry
		try
		{
			ra = std::stod(catalog_row[1]);
			dec = std::stod(catalog_row[2]);
			vmag = std::stod(catalog_row[3]);
			bv_color = std::stod(catalog_row[4]);
		}
		catch (const std::exception& e)
		{
			continue;
		}
		
		// Convert right ascension and declination from degrees to radians
		ra = math::wrap_radians(math::radians(ra));
		dec = math::wrap_radians(math::radians(dec));
		
		// Transform spherical equatorial coordinates to rectangular equatorial coordinates
		double3 position_bci = geom::spherical::to_cartesian(double3{1.0, dec, ra});
		
		// Transform coordinates from equatorial space to inertial space
		physics::frame<double> bci_to_inertial = physics::orbit::inertial::to_bci({0, 0, 0}, 0.0, math::radians(23.4393)).inverse();
		double3 position_inertial = bci_to_inertial * position_bci;
		
		// Convert color index to color temperature
		double cct = color::index::bv_to_cct(bv_color);
		
		// Calculate XYZ color from color temperature
		double3 color_xyz = color::cct::to_xyz(cct);
		
		// Transform XYZ color to ACEScg colorspace
		double3 color_acescg = color::xyz::to_acescg(color_xyz);
		
		// Convert apparent magnitude to irradiance (W/m^2)
		double vmag_irradiance = std::pow(10.0, 0.4 * (-vmag - 19.0 + 0.4));
		
		// Convert irradiance to illuminance
		double vmag_illuminance = vmag_irradiance * (683.0 * 0.14);
		
		// Scale color by illuminance
		double3 scaled_color = color_acescg * vmag_illuminance;
		
		// Build vertex
		*(star_vertex++) = static_cast<float>(position_inertial.x);
		*(star_vertex++) = static_cast<float>(position_inertial.y);
		*(star_vertex++) = static_cast<float>(position_inertial.z);
		*(star_vertex++) = static_cast<float>(scaled_color.x);
		*(star_vertex++) = static_cast<float>(scaled_color.y);
		*(star_vertex++) = static_cast<float>(scaled_color.z);
	}
	
	// Unload star catalog
	ctx->resource_manager->unload("stars.csv");
	
	// Allocate stars model
	model* stars_model = new model();
	
	// Resize model VBO and upload vertex data
	gl::vertex_buffer* vbo = stars_model->get_vertex_buffer();
	vbo->resize(star_count * star_vertex_stride, star_vertex_data);
	
	// Free star catalog vertex data
	delete[] star_vertex_data;
	
	// Bind vertex attributes to model VAO
	gl::vertex_array* vao = stars_model->get_vertex_array();
	std::size_t vao_offset = 0;
	vao->bind_attribute(VERTEX_POSITION_LOCATION, *vbo, 3, gl::vertex_attribute_type::float_32, star_vertex_stride, 0);
	vao_offset += 3;
	vao->bind_attribute(VERTEX_COLOR_LOCATION, *vbo, 3, gl::vertex_attribute_type::float_32, star_vertex_stride, sizeof(float) * vao_offset);
	
	// Load star material
	material* star_material = ctx->resource_manager->load<material>("fixed-star.mtl");
	
	// Create model group
	model_group* stars_model_group = stars_model->add_group("stars");
	stars_model_group->set_material(star_material);
	stars_model_group->set_drawing_mode(gl::drawing_mode::points);
	stars_model_group->set_start_index(0);
	stars_model_group->set_index_count(star_count);
	
	// Pass stars model to sky pass
	ctx->surface_sky_pass->set_stars_model(stars_model);
}

void colonigenesis(game::context* ctx)
{}

} // namespace loading
} // namespace state
} // namespace game
