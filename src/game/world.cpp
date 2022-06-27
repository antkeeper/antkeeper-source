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

#include "game/world.hpp"
#include "scene/text.hpp"
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
#include "entity/archetype.hpp"
#include "geom/spherical.hpp"
#include "gl/drawing-mode.hpp"
#include "gl/vertex-array.hpp"
#include "gl/vertex-attribute.hpp"
#include "gl/vertex-buffer.hpp"
#include "physics/light/photometry.hpp"
#include "physics/orbit/orbit.hpp"
#include "render/material.hpp"
#include "render/model.hpp"
#include "render/passes/shadow-map-pass.hpp"
#include "render/vertex-attribute.hpp"
#include "resources/resource-manager.hpp"
#include "scene/ambient-light.hpp"
#include "scene/directional-light.hpp"
#include "gl/texture-wrapping.hpp"
#include "gl/texture-filter.hpp"
#include "render/material-flags.hpp"
#include "config.hpp"

namespace game {
namespace world {

void create_stars(game::context& ctx)
{
	// Load star catalog
	string_table* star_catalog = ctx.resource_manager->load<string_table>("stars.csv");
	
	// Allocate star catalog vertex data
	std::size_t star_count = 0;
	if (star_catalog->size() > 0)
		star_count = star_catalog->size() - 1;
	std::size_t star_vertex_size = 7;
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
		*(star_vertex++) = static_cast<float>(vmag);
	}
	
	// Unload star catalog
	ctx.resource_manager->unload("stars.csv");
	
	// Allocate stars model
	render::model* stars_model = new render::model();
	
	// Get model VBO and VAO
	gl::vertex_buffer* vbo = stars_model->get_vertex_buffer();
	gl::vertex_array* vao = stars_model->get_vertex_array();
	
	// Resize model VBO and upload vertex data
	vbo->resize(star_count * star_vertex_stride, star_vertex_data);
	
	// Free star catalog vertex data
	delete[] star_vertex_data;
	
	std::size_t attribute_offset = 0;
	
	// Define position vertex attribute
	gl::vertex_attribute position_attribute;
	position_attribute.buffer = vbo;
	position_attribute.offset = attribute_offset;
	position_attribute.stride = star_vertex_stride;
	position_attribute.type = gl::vertex_attribute_type::float_32;
	position_attribute.components = 3;
	attribute_offset += position_attribute.components * sizeof(float);
	
	// Define color vertex attribute
	gl::vertex_attribute color_attribute;
	color_attribute.buffer = vbo;
	color_attribute.offset = attribute_offset;
	color_attribute.stride = star_vertex_stride;
	color_attribute.type = gl::vertex_attribute_type::float_32;
	color_attribute.components = 4;
	attribute_offset += color_attribute.components * sizeof(float);
	
	// Bind vertex attributes to VAO
	vao->bind(render::vertex_attribute::position, position_attribute);
	vao->bind(render::vertex_attribute::color, color_attribute);
	
	// Load star material
	render::material* star_material = ctx.resource_manager->load<render::material>("fixed-star.mtl");
	
	// Create model group
	render::model_group* stars_model_group = stars_model->add_group("stars");
	stars_model_group->set_material(star_material);
	stars_model_group->set_drawing_mode(gl::drawing_mode::points);
	stars_model_group->set_start_index(0);
	stars_model_group->set_index_count(star_count);
	
	// Pass stars model to sky pass
	ctx.surface_sky_pass->set_stars_model(stars_model);
}

void create_sun(game::context& ctx)
{
	// Create sun entity
	entity::archetype* sun_archetype = ctx.resource_manager->load<entity::archetype>("sun.ent");
	entity::id sun_eid = sun_archetype->create(*ctx.entity_registry);
	ctx.entities["sun"] = sun_eid;
	
	// Create direct sun light scene object
	scene::directional_light* sun_direct = new scene::directional_light();
	
	// Create ambient sun light scene object
	scene::ambient_light* sun_ambient = new scene::ambient_light();
	sun_ambient->set_color({1, 1, 1});
	sun_ambient->set_intensity(0.0f);
	sun_ambient->update_tweens();
	
	// Add sun light scene objects to surface scene
	ctx.surface_scene->add_object(sun_direct);
	ctx.surface_scene->add_object(sun_ambient);
	
	// Pass direct sun light scene object to shadow map pass and astronomy system
	ctx.surface_shadow_map_pass->set_light(sun_direct);
	ctx.astronomy_system->set_sun_light(sun_direct);
}

void create_planet(game::context& ctx)
{
	// Create planet entity
	entity::archetype* planet_archetype = ctx.resource_manager->load<entity::archetype>("planet.ent");
	entity::id planet_eid = planet_archetype->create(*ctx.entity_registry);
	ctx.entities["planet"] = planet_eid;
	
	// Assign planetary terrain component
	entity::component::terrain terrain;
	terrain.elevation = [](double, double) -> double
	{
		//return math::random<double>(0.0, 1.0);
		return 0.0;
	};
	terrain.max_lod = 0;
	terrain.patch_material = nullptr;
	ctx.entity_registry->assign<entity::component::terrain>(planet_eid, terrain);
	
	// Pass planet to astronomy system as reference body
	ctx.astronomy_system->set_reference_body(planet_eid);
	
	// Load sky model
	ctx.surface_sky_pass->set_sky_model(ctx.resource_manager->load<render::model>("sky-dome.mdl"));
}

void create_moon(game::context& ctx)
{
	// Create lunar entity
	entity::id moon_eid = ctx.entity_registry->create();
	ctx.entities["moon"] = moon_eid;
	
	// Pass moon model to sky pass
	ctx.surface_sky_pass->set_moon_model(ctx.resource_manager->load<render::model>("moon.mdl"));
}

void set_time(game::context& ctx, double t)
{
	ctx.astronomy_system->set_universal_time(t);
	ctx.orbit_system->set_universal_time(t);
}

void set_time_scale(game::context& ctx, double scale)
{
	static constexpr double seconds_per_day = 24.0 * 60.0 * 60.0;
	scale /= seconds_per_day;
	
	ctx.orbit_system->set_time_scale(scale);
	ctx.astronomy_system->set_time_scale(scale);
}

} // namespace world
} // namespace game
