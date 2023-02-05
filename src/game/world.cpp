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

#include "application.hpp"
#include "color/color.hpp"
#include "config.hpp"
#include "debug/log.hpp"
#include "entity/archetype.hpp"
#include "entity/commands.hpp"
#include "game/component/atmosphere.hpp"
#include "game/component/blackbody.hpp"
#include "game/component/celestial-body.hpp"
#include "game/component/observer.hpp"
#include "game/component/orbit.hpp"
#include "game/component/terrain.hpp"
#include "game/component/transform.hpp"
#include "game/system/astronomy.hpp"
#include "game/system/atmosphere.hpp"
#include "game/system/orbit.hpp"
#include "game/system/terrain.hpp"
#include "game/world.hpp"
#include "geom/solid-angle.hpp"
#include "geom/spherical.hpp"
#include "gl/drawing-mode.hpp"
#include "gl/texture-filter.hpp"
#include "gl/texture-wrapping.hpp"
#include "gl/vertex-array.hpp"
#include "gl/vertex-attribute.hpp"
#include "gl/vertex-buffer.hpp"
#include "math/hash/hash.hpp"
#include "math/noise/noise.hpp"
#include "physics/light/photometry.hpp"
#include "physics/light/vmag.hpp"
#include "physics/orbit/ephemeris.hpp"
#include "physics/orbit/orbit.hpp"
#include "physics/time/constants.hpp"
#include "physics/time/gregorian.hpp"
#include "physics/time/utc.hpp"
#include "render/material-flags.hpp"
#include "render/material.hpp"
#include "render/model.hpp"
#include "render/passes/ground-pass.hpp"
#include "render/passes/shadow-map-pass.hpp"
#include "render/passes/sky-pass.hpp"
#include "render/vertex-attribute.hpp"
#include "resources/image.hpp"
#include "resources/json.hpp"
#include "resources/resource-manager.hpp"
#include "scene/ambient-light.hpp"
#include "scene/directional-light.hpp"
#include "scene/text.hpp"
#include "i18n/string-table.hpp"
#include <algorithm>
#include <execution>
#include <fstream>
#include <stb/stb_image_write.h>

namespace game {
namespace world {

/// Loads an ephemeris.
static void load_ephemeris(game::context& ctx);

/// Creates the fixed stars.
static void create_stars(game::context& ctx);

/// Creates the Sun.
static void create_sun(game::context& ctx);

/// Creates the Earth-Moon system.
static void create_earth_moon_system(game::context& ctx);

/// Creates the Earth.
static void create_earth(game::context& ctx);

/// Creates the Moon.
static void create_moon(game::context& ctx);

void cosmogenesis(game::context& ctx)
{
	debug::log::trace("Generating cosmos...");
	
	load_ephemeris(ctx);
	create_stars(ctx);
	create_sun(ctx);
	create_earth_moon_system(ctx);
	
	debug::log::trace("Generated cosmos");
}

void create_observer(game::context& ctx)
{
	debug::log::trace("Creating observer...");
	
	{
		// Create observer entity
		entity::id observer_eid = ctx.entity_registry->create();
		ctx.entities["observer"] = observer_eid;
		
		// Construct observer component
		game::component::observer observer;
		
		// Set observer reference body
		if (auto it = ctx.entities.find("earth"); it != ctx.entities.end())
			observer.reference_body_eid = it->second;
		else
			observer.reference_body_eid = entt::null;
		
		// Set observer location
		observer.elevation = 0.0;
		observer.latitude = 0.0;
		observer.longitude = 0.0;
		
		// Assign observer component to observer entity
		ctx.entity_registry->emplace<game::component::observer>(observer_eid, observer);
		
		// Set atmosphere system active atmosphere
		ctx.atmosphere_system->set_active_atmosphere(observer.reference_body_eid);
		
		// Set astronomy system observer
		ctx.astronomy_system->set_observer(observer_eid);
	}
	
	debug::log::trace("Created observer");
}

void set_location(game::context& ctx, double elevation, double latitude, double longitude)
{
	if (auto it = ctx.entities.find("observer"); it != ctx.entities.end())
	{
		entity::id observer_eid = it->second;
		
		if (ctx.entity_registry->valid(observer_eid) && ctx.entity_registry->all_of<game::component::observer>(observer_eid))
		{
			// Update observer location 
			ctx.entity_registry->patch<game::component::observer>
			(
				observer_eid,
				[&](auto& component)
				{
					component.elevation = elevation;
					component.latitude = latitude;
					component.longitude = longitude;
				}
			);
		}
	}
}

void set_time(game::context& ctx, double t)
{
	try
	{
		ctx.astronomy_system->set_time(t);
		ctx.orbit_system->set_time(t);
		
		debug::log::info("Set time to UT1 {}", t);
	}
	catch (const std::exception& e)
	{
		debug::log::error("Failed to set time to UT1 {}: {}", t, e.what());
	}
}

void set_time(game::context& ctx, int year, int month, int day, int hour, int minute, double second)
{
	double longitude = 0.0;
	
	// Get longitude of observer
	if (auto it = ctx.entities.find("observer"); it != ctx.entities.end())
	{
		entity::id observer_eid = it->second;
		if (ctx.entity_registry->valid(observer_eid))
		{
			const auto observer = ctx.entity_registry->try_get<game::component::observer>(observer_eid);
			if (observer)
				longitude = observer->longitude;
		}
	}
	
	// Calculate UTC offset at longitude
	const double utc_offset = physics::time::utc::offset<double>(longitude);
	
	// Convert time from Gregorian to UT1
	const double t = physics::time::gregorian::to_ut1<double>(year, month, day, hour, minute, second, utc_offset);
	
	set_time(ctx, t);
}

void set_time_scale(game::context& ctx, double scale)
{
	// Convert time scale from seconds to days
	const double astronomical_scale = scale / physics::time::seconds_per_day<double>;
	
	ctx.orbit_system->set_time_scale(astronomical_scale);
	ctx.astronomy_system->set_time_scale(astronomical_scale);
}

void load_ephemeris(game::context& ctx)
{
	ctx.orbit_system->set_ephemeris(ctx.resource_manager->load<physics::orbit::ephemeris<double>>("de421.eph"));
}

void create_stars(game::context& ctx)
{
	debug::log::trace("Generating fixed stars...");
	
	// Load star catalog
	i18n::string_table* star_catalog = nullptr;
	star_catalog = ctx.resource_manager->load<i18n::string_table>("hipparcos-7.tsv");
	
	// Allocate star catalog vertex data
	std::size_t star_count = 0;
	if (star_catalog->size() > 0)
		star_count = star_catalog->size() - 1;
	std::size_t star_vertex_size = 7;
	std::size_t star_vertex_stride = star_vertex_size * sizeof(float);
	float* star_vertex_data = new float[star_count * star_vertex_size];
	float* star_vertex = star_vertex_data;
	
	// Init starlight illuminance
	double3 starlight_illuminance = {0, 0, 0};
	
	// Build star catalog vertex data
	for (std::size_t i = 1; i < star_catalog->size(); ++i)
	{
		const i18n::string_table_row& catalog_row = (*star_catalog)[i];
		
		// Parse star catalog item
		float ra = 0.0;
		float dec = 0.0;
		float vmag = 0.0;
		float bv = 0.0;
		try
		{
			ra = std::stof(catalog_row[1]);
			dec = std::stof(catalog_row[2]);
			vmag = std::stof(catalog_row[3]);
			bv = std::stof(catalog_row[4]);
		}
		catch (const std::exception&)
		{
			debug::log::warning("Invalid star catalog item on row {}", i);
			continue;
		}
		
		// Convert right ascension and declination from degrees to radians
		ra = math::wrap_radians(math::radians(ra));
		dec = math::wrap_radians(math::radians(dec));
		
		// Convert ICRF coordinates from spherical to Cartesian
		float3 position = physics::orbit::frame::bci::cartesian(float3{1.0f, dec, ra});
		
		// Convert color index to color temperature
		float cct = color::index::bv_to_cct(bv);
		
		// Calculate XYZ color from color temperature
		float3 color_xyz = color::cct::to_xyz(cct);
		
		// Transform XYZ color to ACEScg colorspace
		float3 color_acescg = color::aces::ap1<float>.from_xyz * color_xyz;
		
		// Convert apparent magnitude to brightness factor relative to a 0th magnitude star
		float brightness = physics::light::vmag::to_brightness(vmag);
		
		// Build vertex
		*(star_vertex++) = position.x();
		*(star_vertex++) = position.y();
		*(star_vertex++) = position.z();
		*(star_vertex++) = color_acescg.x();
		*(star_vertex++) = color_acescg.y();
		*(star_vertex++) = color_acescg.z();
		*(star_vertex++) = brightness;
		
		// Calculate spectral illuminance
		double3 illuminance = double3(color_acescg * physics::light::vmag::to_illuminance(vmag));
		
		// Add spectral illuminance to total starlight illuminance
		starlight_illuminance += illuminance;
	}
	
	// Unload star catalog
	ctx.resource_manager->unload("hipparcos-7.tsv");
	
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
	ctx.sky_pass->set_stars_model(stars_model);
	
	// Pass starlight illuminance to astronomy system
	ctx.astronomy_system->set_starlight_illuminance(starlight_illuminance);
	
	debug::log::trace("Generated fixed stars");
}

void create_sun(game::context& ctx)
{
	debug::log::trace("Generating Sun...");
	
	{
		// Create sun entity
		entity::archetype* sun_archetype = ctx.resource_manager->load<entity::archetype>("sun.ent");
		entity::id sun_eid = sun_archetype->create(*ctx.entity_registry);
		ctx.entities["sun"] = sun_eid;
		
		// Create sun directional light scene object
		scene::directional_light* sun_light = new scene::directional_light();
		sun_light->set_color({0, 0, 0});
		sun_light->set_shadow_caster(true);
		sun_light->set_shadow_framebuffer(ctx.shadow_map_framebuffer);
		sun_light->set_shadow_bias(0.005f);
		sun_light->set_shadow_cascade_count(4);
		sun_light->set_shadow_cascade_coverage(0.15f);
		sun_light->set_shadow_cascade_distribution(0.8f);
		sun_light->update_tweens();
		
		// Create sky ambient light scene object
		scene::ambient_light* sky_light = new scene::ambient_light();
		sky_light->set_color({0, 0, 0});
		sky_light->update_tweens();
		
		// Create bounce directional light scene object
		scene::directional_light* bounce_light = new scene::directional_light();
		bounce_light->set_color({0, 0, 0});
		bounce_light->look_at({0, 0, 0}, {0, 1, 0}, {1, 0, 0});
		bounce_light->update_tweens();
		
		// Add sun light scene objects to surface scene
		ctx.surface_scene->add_object(sun_light);
		ctx.surface_scene->add_object(sky_light);
		//ctx.surface_scene->add_object(bounce_light);
		
		// Pass direct sun light scene object to shadow map pass and astronomy system
		ctx.astronomy_system->set_sun_light(sun_light);
		ctx.astronomy_system->set_sky_light(sky_light);
		ctx.astronomy_system->set_bounce_light(bounce_light);
	}
	
	debug::log::trace("Generated Sun");
}

void create_earth_moon_system(game::context& ctx)
{
	debug::log::trace("Generating Earth-Moon system...");
	
	{
		// Create Earth-Moon barycenter entity
		entity::archetype* em_bary_archetype = ctx.resource_manager->load<entity::archetype>("em-bary.ent");
		entity::id em_bary_eid = em_bary_archetype->create(*ctx.entity_registry);
		ctx.entities["em_bary"] = em_bary_eid;
		
		// Create Earth
		create_earth(ctx);
		
		// Create Moon
		create_moon(ctx);
	}
	
	debug::log::trace("Generated Earth-Moon system");
}

void create_earth(game::context& ctx)
{
	debug::log::trace("Generating Earth...");
	
	{
		// Create earth entity
		entity::archetype* earth_archetype = ctx.resource_manager->load<entity::archetype>("earth.ent");
		entity::id earth_eid = earth_archetype->create(*ctx.entity_registry);
		ctx.entities["earth"] = earth_eid;
		
		// Assign orbital parent
		ctx.entity_registry->get<game::component::orbit>(earth_eid).parent = ctx.entities["em_bary"];
	}
	
	debug::log::trace("Generated Earth");
}

void create_moon(game::context& ctx)
{
	debug::log::trace("Generating Moon...");
	
	{
		// Create lunar entity
		entity::archetype* moon_archetype = ctx.resource_manager->load<entity::archetype>("moon.ent");
		entity::id moon_eid = moon_archetype->create(*ctx.entity_registry);
		ctx.entities["moon"] = moon_eid;
		
		// Assign orbital parent
		ctx.entity_registry->get<game::component::orbit>(moon_eid).parent = ctx.entities["em_bary"];
		
		// Pass moon model to sky pass
		ctx.sky_pass->set_moon_model(ctx.resource_manager->load<render::model>("moon.mdl"));
		
		// Create moon directional light scene object
		scene::directional_light* moon_light = new scene::directional_light();
		moon_light->set_color({0, 0, 0});
		moon_light->update_tweens();
		
		// Add moon light scene objects to surface scene
		ctx.surface_scene->add_object(moon_light);
		
		// Pass moon light scene object to astronomy system
		ctx.astronomy_system->set_moon_light(moon_light);
	}
	
	debug::log::trace("Generated Moon");
}

void enter_ecoregion(game::context& ctx, const ecoregion& ecoregion)
{
	/*
	image img;
	img.format(1, 4);
	img.resize(2048, 2048);
	
	auto width = img.get_width();
	auto height = img.get_height();
	unsigned char* pixels = (unsigned char*)img.data();
	
	const float frequency = 400.0f;
	float scale_x = 1.0f / static_cast<float>(width - 1) * frequency;
	float scale_y = 1.0f / static_cast<float>(height - 1) * frequency;
	
	std::for_each
	(
		std::execution::par_unseq,
		img.begin<math::vector<unsigned char, 4>>(),
		img.end<math::vector<unsigned char, 4>>(),
		[pixels, width, height, scale_x, scale_y, frequency](auto& pixel)
		{
			const std::size_t i = &pixel - (math::vector<unsigned char, 4>*)pixels;
			const std::size_t y = i / width;
			const std::size_t x = i % width;
			
			const float2 position =
			{
				static_cast<float>(x) * scale_x,
				static_cast<float>(y) * scale_y
			};
			
			const auto
			[
				f1_sqr_distance,
				f1_displacement,
				f1_id
			] = math::noise::voronoi::f1<float, 2>(position, 1.0f, {frequency, frequency});
			
			const float f1_distance = std::sqrt(f1_sqr_distance);
			
			const float2 uv = (position + f1_displacement) / frequency;
			
			pixel = 
			{
				static_cast<unsigned char>(std::min(255.0f, f1_distance * 255.0f)),
				static_cast<unsigned char>(std::min(255.0f, uv[0] * 255.0f)),
				static_cast<unsigned char>(std::min(255.0f, uv[1] * 255.0f)),
				static_cast<unsigned char>(f1_id % 256)
			};
		}
	);
	
	stbi_flip_vertically_on_write(1);
	stbi_write_tga((ctx.screenshots_path / "voronoi-f1-400-nc8-2k.tga").string().c_str(), img.get_width(), img.get_height(), img.get_channel_count(), img.data());
	*/

	
	debug::log::trace("Entering ecoregion {}...", ecoregion.name);
	{
		// Set active ecoregion
		ctx.active_ecoregion = &ecoregion;
		
		// Set location
		game::world::set_location(ctx, ecoregion.elevation, ecoregion.latitude, ecoregion.longitude);
		
		// Setup sky
		ctx.sky_pass->set_sky_model(ctx.resource_manager->load<render::model>("celestial-hemisphere.mdl"));
		render::model* terrestrial_hemisphere_model = ctx.resource_manager->load<render::model>("terrestrial-hemisphere.mdl");
		(*terrestrial_hemisphere_model->get_groups())[0]->set_material(ecoregion.horizon_material);
		ctx.ground_pass->set_ground_model(terrestrial_hemisphere_model);
		
		// Setup terrain
		ctx.terrain_system->set_patch_material(ecoregion.terrain_material);
		ctx.terrain_system->set_elevation_function
		(
			[](float x, float z) -> float
			{
				const float2 position = float2{x, z};

				const std::size_t octaves = 3;
				const float lacunarity = 1.5f;
				const float gain = 0.5f;
				
				const float fbm = math::noise::fbm
				(
					position * 0.005f,
					octaves,
					lacunarity,
					gain
				);
				
				float y = fbm * 4.0f;
				
				return y;
			}
		);
		ctx.astronomy_system->set_bounce_albedo(double3(ecoregion.terrain_albedo));
	}
	
	debug::log::trace("Entered ecoregion {}", ecoregion.name);
}

} // namespace world
} // namespace game
