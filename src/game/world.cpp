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

#include "game/world.hpp"
#include <engine/color/color.hpp>
#include <engine/config.hpp>
#include <engine/debug/log.hpp>
#include <engine/entity/archetype.hpp>
#include "game/commands/commands.hpp"
#include "game/components/atmosphere-component.hpp"
#include "game/components/blackbody-component.hpp"
#include "game/components/celestial-body-component.hpp"
#include "game/components/observer-component.hpp"
#include "game/components/orbit-component.hpp"
#include "game/components/terrain-component.hpp"
#include "game/components/transform-component.hpp"
#include "game/systems/astronomy-system.hpp"
#include "game/systems/atmosphere-system.hpp"
#include "game/systems/orbit-system.hpp"
#include "game/systems/terrain-system.hpp"
#include <engine/geom/solid-angle.hpp>
#include <engine/gl/drawing-mode.hpp>
#include <engine/gl/texture-filter.hpp>
#include <engine/gl/texture-wrapping.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-attribute.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/i18n/string-table.hpp>
#include <engine/math/hash/hash.hpp>
#include <engine/math/noise/noise.hpp>
#include <engine/math/angles.hpp>
#include <engine/physics/light/photometry.hpp>
#include <engine/physics/light/vmag.hpp>
#include <engine/physics/orbit/ephemeris.hpp>
#include <engine/physics/orbit/orbit.hpp>
#include <engine/physics/time/constants.hpp>
#include <engine/physics/time/gregorian.hpp>
#include <engine/physics/time/utc.hpp>
#include <engine/render/material-flags.hpp>
#include <engine/render/material.hpp>
#include <engine/render/model.hpp>
#include <engine/render/passes/sky-pass.hpp>
#include <engine/render/vertex-attribute.hpp>
#include <engine/utility/image.hpp>
#include <engine/utility/json.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/scene/directional-light.hpp>
#include <engine/scene/text.hpp>
#include <algorithm>
#include <execution>
#include <fstream>
#include <stb/stb_image_write.h>
#include <engine/animation/screen-transition.hpp>
#include <engine/animation/ease.hpp>

namespace world {

/// Loads an ephemeris.
static void load_ephemeris(::game& ctx);

/// Creates the fixed stars.
static void create_stars(::game& ctx);

/// Creates the Sun.
static void create_sun(::game& ctx);

/// Creates the Earth-Moon system.
static void create_earth_moon_system(::game& ctx);

/// Creates the Earth.
static void create_earth(::game& ctx);

/// Creates the Moon.
static void create_moon(::game& ctx);

void cosmogenesis(::game& ctx)
{
	debug::log::trace("Generating cosmos...");
	
	load_ephemeris(ctx);
	create_stars(ctx);
	create_sun(ctx);
	create_earth_moon_system(ctx);
	
	debug::log::trace("Generated cosmos");
}

void create_observer(::game& ctx)
{
	debug::log::trace("Creating observer...");
	
	{
		// Create observer entity
		entity::id observer_eid = ctx.entity_registry->create();
		ctx.entities["observer"] = observer_eid;
		
		// Construct observer component
		::observer_component observer;
		
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
		ctx.entity_registry->emplace<::observer_component>(observer_eid, observer);
		
		// Set atmosphere system active atmosphere
		ctx.atmosphere_system->set_active_atmosphere(observer.reference_body_eid);
		
		// Set astronomy system observer
		ctx.astronomy_system->set_observer(observer_eid);
	}
	
	debug::log::trace("Created observer");
}

void set_location(::game& ctx, double elevation, double latitude, double longitude)
{
	if (auto it = ctx.entities.find("observer"); it != ctx.entities.end())
	{
		entity::id observer_eid = it->second;
		
		if (ctx.entity_registry->valid(observer_eid) && ctx.entity_registry->all_of<::observer_component>(observer_eid))
		{
			// Update observer location 
			ctx.entity_registry->patch<::observer_component>
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

void set_time(::game& ctx, double t)
{
	try
	{
		ctx.astronomy_system->set_time(t);
		ctx.orbit_system->set_time(t);
		
		// debug::log::info("Set time to UT1 {}", t);
	}
	catch (const std::exception& e)
	{
		debug::log::error("Failed to set time to UT1 {}: {}", t, e.what());
	}
}

void set_time(::game& ctx, int year, int month, int day, int hour, int minute, double second)
{
	double longitude = 0.0;
	
	// Get longitude of observer
	if (auto it = ctx.entities.find("observer"); it != ctx.entities.end())
	{
		entity::id observer_eid = it->second;
		if (ctx.entity_registry->valid(observer_eid))
		{
			const auto observer = ctx.entity_registry->try_get<::observer_component>(observer_eid);
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

void set_time_scale(::game& ctx, double scale)
{
	// Convert time scale from seconds to days
	const double astronomical_scale = scale / physics::time::seconds_per_day<double>;
	
	ctx.orbit_system->set_time_scale(astronomical_scale);
	ctx.astronomy_system->set_time_scale(astronomical_scale);
}

void load_ephemeris(::game& ctx)
{
	ctx.orbit_system->set_ephemeris(ctx.resource_manager->load<physics::orbit::ephemeris<double>>("de421.eph"));
}

void create_stars(::game& ctx)
{
	debug::log::trace("Generating fixed stars...");
	
	// Load star catalog
	auto star_catalog = ctx.resource_manager->load<i18n::string_table>("hipparcos-7.tsv");
	
	// Allocate star catalog vertex data
	std::size_t star_count = 0;
	if (star_catalog->rows.size() > 0)
		star_count = star_catalog->rows.size() - 1;
	std::size_t star_vertex_size = 7;
	std::size_t star_vertex_stride = star_vertex_size * sizeof(float);
	std::vector<float> star_vertex_data(star_count * star_vertex_size);
	float* star_vertex = star_vertex_data.data();
	
	// Init starlight illuminance
	math::dvec3 starlight_illuminance = {0, 0, 0};
	
	// Build star catalog vertex data
	for (std::size_t i = 1; i < star_catalog->rows.size(); ++i)
	{
		const auto& row = star_catalog->rows[i];
		
		// Parse star catalog item
		float ra = 0.0;
		float dec = 0.0;
		float vmag = 0.0;
		float bv = 0.0;
		try
		{
			ra = std::stof(row[1]);
			dec = std::stof(row[2]);
			vmag = std::stof(row[3]);
			bv = std::stof(row[4]);
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
		math::fvec3 position = physics::orbit::frame::bci::cartesian(math::fvec3{1.0f, dec, ra});
		
		// Convert color index to color temperature
		float cct = color::bv_to_cct(bv);
		
		// Calculate XYZ color from color temperature
		math::fvec3 color_xyz = color::cct_to_xyz(cct);
		
		// Transform XYZ color to RGB
		math::fvec3 color_rgb = config::scene_linear_color_space<float>.from_xyz * color_xyz;
		
		// Convert apparent magnitude to brightness factor relative to a 0th magnitude star
		float brightness = physics::light::vmag::to_brightness(vmag);
		
		// Build vertex
		*(star_vertex++) = position.x();
		*(star_vertex++) = position.y();
		*(star_vertex++) = position.z();
		*(star_vertex++) = color_rgb.x();
		*(star_vertex++) = color_rgb.y();
		*(star_vertex++) = color_rgb.z();
		*(star_vertex++) = brightness;
		
		// Calculate spectral illuminance
		math::dvec3 illuminance = math::dvec3(color_rgb * physics::light::vmag::to_illuminance(vmag));
		
		// Add spectral illuminance to total starlight illuminance
		starlight_illuminance += illuminance;
	}
	
	// Allocate stars model
	std::shared_ptr<render::model> stars_model = std::make_shared<render::model>();
	
	// Get model VBO and VAO
	auto& vbo = stars_model->get_vertex_buffer();
	auto& vao = stars_model->get_vertex_array();
	
	// Resize model VBO and upload vertex data
	vbo->resize(star_vertex_data.size(), std::as_bytes(std::span{star_vertex_data}));
	
	std::size_t attribute_offset = 0;
	
	// Define position vertex attribute
	gl::vertex_attribute position_attribute;
	position_attribute.buffer = vbo.get();
	position_attribute.offset = attribute_offset;
	position_attribute.stride = star_vertex_stride;
	position_attribute.type = gl::vertex_attribute_type::float_32;
	position_attribute.components = 3;
	attribute_offset += position_attribute.components * sizeof(float);
	
	// Define color vertex attribute
	gl::vertex_attribute color_attribute;
	color_attribute.buffer = vbo.get();
	color_attribute.offset = attribute_offset;
	color_attribute.stride = star_vertex_stride;
	color_attribute.type = gl::vertex_attribute_type::float_32;
	color_attribute.components = 4;
	//attribute_offset += color_attribute.components * sizeof(float);
	
	// Bind vertex attributes to VAO
	vao->bind(render::vertex_attribute::position, position_attribute);
	vao->bind(render::vertex_attribute::color, color_attribute);
	
	// Load star material
	std::shared_ptr<render::material> star_material = ctx.resource_manager->load<render::material>("fixed-star.mtl");
	
	// Create model group
	stars_model->get_groups().resize(1);
	render::model_group& stars_model_group = stars_model->get_groups().back();
	
	stars_model_group.id = "stars";
	stars_model_group.material = star_material;
	stars_model_group.drawing_mode = gl::drawing_mode::points;
	stars_model_group.start_index = 0;
	stars_model_group.index_count = static_cast<std::uint32_t>(star_count);
	
	// Pass stars model to sky pass
	ctx.sky_pass->set_stars_model(stars_model);
	
	// Pass starlight illuminance to astronomy system
	ctx.astronomy_system->set_starlight_illuminance(starlight_illuminance);
	
	debug::log::trace("Generated fixed stars");
}

void create_sun(::game& ctx)
{
	debug::log::trace("Generating Sun...");
	
	{
		// Create sun entity
		auto sun_archetype = ctx.resource_manager->load<entity::archetype>("sun.ent");
		entity::id sun_eid = sun_archetype->create(*ctx.entity_registry);
		ctx.entities["sun"] = sun_eid;
		
		// Create sun directional light scene object
		ctx.sun_light = std::make_unique<scene::directional_light>();
		ctx.sun_light->set_shadow_caster(true);
		ctx.sun_light->set_shadow_framebuffer(ctx.shadow_map_framebuffer);
		ctx.sun_light->set_shadow_bias(0.005f);
		ctx.sun_light->set_shadow_distance(50.0f);
		ctx.sun_light->set_shadow_cascade_count(4);
		ctx.sun_light->set_shadow_cascade_distribution(0.8f);
		
		// Add sun light scene objects to surface scene
		ctx.surface_scene->add_object(*ctx.sun_light);
		
		// Pass direct sun light scene object to shadow map pass and astronomy system
		ctx.astronomy_system->set_sun_light(ctx.sun_light.get());
	}
	
	debug::log::trace("Generated Sun");
}

void create_earth_moon_system(::game& ctx)
{
	debug::log::trace("Generating Earth-Moon system...");
	
	{
		// Create Earth-Moon barycenter entity
		auto em_bary_archetype = ctx.resource_manager->load<entity::archetype>("em-bary.ent");
		entity::id em_bary_eid = em_bary_archetype->create(*ctx.entity_registry);
		ctx.entities["em_bary"] = em_bary_eid;
		
		// Create Earth
		create_earth(ctx);
		
		// Create Moon
		create_moon(ctx);
	}
	
	debug::log::trace("Generated Earth-Moon system");
}

void create_earth(::game& ctx)
{
	debug::log::trace("Generating Earth...");
	
	{
		// Create earth entity
		auto earth_archetype = ctx.resource_manager->load<entity::archetype>("earth.ent");
		entity::id earth_eid = earth_archetype->create(*ctx.entity_registry);
		ctx.entities["earth"] = earth_eid;
		
		// Assign orbital parent
		ctx.entity_registry->get<::orbit_component>(earth_eid).parent = ctx.entities["em_bary"];
	}
	
	debug::log::trace("Generated Earth");
}

void create_moon(::game& ctx)
{
	debug::log::trace("Generating Moon...");
	
	{
		// Create lunar entity
		auto moon_archetype = ctx.resource_manager->load<entity::archetype>("moon.ent");
		entity::id moon_eid = moon_archetype->create(*ctx.entity_registry);
		ctx.entities["moon"] = moon_eid;
		
		// Assign orbital parent
		ctx.entity_registry->get<::orbit_component>(moon_eid).parent = ctx.entities["em_bary"];
		
		// Pass moon model to sky pass
		ctx.sky_pass->set_moon_model(ctx.resource_manager->load<render::model>("moon.mdl"));
		
		// Create moon directional light scene object
		ctx.moon_light = std::make_unique<scene::directional_light>();
		
		// Add moon light scene objects to surface scene
		ctx.surface_scene->add_object(*ctx.moon_light);
		
		// Pass moon light scene object to astronomy system
		ctx.astronomy_system->set_moon_light(ctx.moon_light.get());
	}
	
	debug::log::trace("Generated Moon");
}

void enter_ecoregion(::game& ctx, const ecoregion& ecoregion)
{	
	debug::log::trace("Entering ecoregion {}...", ecoregion.name);
	{
		// Set active ecoregion
		//ctx.active_ecoregion = &ecoregion;
		
		// Set location
		::world::set_location(ctx, ecoregion.elevation, ecoregion.latitude, ecoregion.longitude);
		
		// Setup sky
		ctx.sky_pass->set_sky_model(ctx.resource_manager->load<render::model>("celestial-hemisphere.mdl"));
		ctx.sky_pass->set_ground_albedo(ecoregion.terrain_albedo);
		
		// Setup terrain
		// ctx.terrain_system->set_patch_material(ecoregion.terrain_material);
		// ctx.terrain_system->set_elevation_function
		// (
			// [](float x, float z) -> float
			// {
				// const math::fvec2 position = math::fvec2{x, z};

				// const std::size_t octaves = 3;
				// const float lacunarity = 1.5f;
				// const float gain = 0.5f;
				
				// const float fbm = math::noise::fbm
				// (
					// position * 0.005f,
					// octaves,
					// lacunarity,
					// gain
				// );
				
				// float y = fbm * 4.0f;
				
				// return y;
			// }
		// );
	}
	
	debug::log::trace("Entered ecoregion {}", ecoregion.name);
}

void switch_scene(::game& ctx)
{
	ctx.fade_transition_color->set({0, 0, 0});
	ctx.fade_transition->transition(1.0f, false, ease<float>::out_cubic, false, [](){});
}

} // namespace world
