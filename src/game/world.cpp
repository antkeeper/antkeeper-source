// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/world.hpp"
#include <engine/color/color.hpp>
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
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/i18n/string-table.hpp>
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
#include <engine/render/vertex-attribute-location.hpp>
#include <engine/utility/json.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/scene/directional-light.hpp>
#include <engine/scene/text.hpp>
#include <algorithm>
#include <execution>
#include <fstream>
#include <engine/animation/ease.hpp>
#include <engine/math/functions.hpp>
#include <engine/hash/fnv.hpp>

using namespace hash::literals;

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
	debug::log_trace("Generating cosmos...");
	
	load_ephemeris(ctx);
	create_stars(ctx);
	create_sun(ctx);
	create_earth_moon_system(ctx);
	
	debug::log_trace("Generating cosmos... OK");
}

void create_observer(::game& ctx)
{
	debug::log_trace("Creating observer...");
	
	{
		// Create observer entity
		entity::id observer_eid = ctx.entity_registry->create();
		ctx.entities["observer"_fnv1a32] = observer_eid;
		
		// Construct observer component
		::observer_component observer;
		
		// Set observer reference body
		if (auto it = ctx.entities.find("earth"_fnv1a32); it != ctx.entities.end())
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
		ctx.m_atmosphere_system->set_active_atmosphere(observer.reference_body_eid);
		
		// Set astronomy system observer
		ctx.m_astronomy_system->set_observer(observer_eid);
	}
	
	debug::log_trace("Creating observer... OK");
}

void set_location(::game& ctx, double elevation, double latitude, double longitude)
{
	if (auto it = ctx.entities.find("observer"_fnv1a32); it != ctx.entities.end())
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
		ctx.m_astronomy_system->set_time(t);
		ctx.m_orbit_system->set_time(t);
		
		// debug::log_info("Set time to UT1 {}", t);
	}
	catch (const std::exception& e)
	{
		debug::log_error("Failed to set time to UT1 {}: {}", t, e.what());
	}
}

void set_time(::game& ctx, int year, int month, int day, int hour, int minute, double second)
{
	double longitude = 0.0;
	
	// Get longitude of observer
	if (auto it = ctx.entities.find("observer"_fnv1a32); it != ctx.entities.end())
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

void load_ephemeris(::game& ctx)
{
	ctx.m_orbit_system->set_ephemeris(ctx.resource_manager->load<physics::orbit::ephemeris<double>>("de421.eph"));
}

void create_stars(::game& ctx)
{
	debug::log_trace("Generating fixed stars...");
	
	// Load star catalog
	auto star_catalog = ctx.resource_manager->load<i18n::string_table>("hipparcos-7.tsv");
	
	// Allocate star catalog vertex data
	std::size_t star_count = 0;
	if (star_catalog->rows.size() > 0)
		star_count = star_catalog->rows.size() - 1;
	constexpr std::size_t star_vertex_stride = 7 * sizeof(float);
	std::vector<float> star_vertex_data(star_count * star_vertex_stride / sizeof(float));
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
			debug::log_warning("Invalid star catalog item on row {}", i);
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
		math::fvec3 color_rgb =  color::bt2020<float>.xyz_to_rgb(color_xyz);
		
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

	// Load star material
	stars_model->materials().emplace_back(ctx.resource_manager->load<render::material>("fixed-star.mtl"));
	
	// Construct stars VAO
	constexpr gl::vertex_input_attribute star_vertex_attributes[] =
	{
		{
			render::vertex_attribute_location::position,
			0,
			gl::format::r32g32b32_sfloat,
			0
		},
		{
			render::vertex_attribute_location::color,
			0,
			gl::format::r32g32b32a32_sfloat,
			3 * sizeof(float)
		}
	};
	auto& vao = stars_model->get_vertex_array();
	vao = std::make_unique<gl::vertex_array>(star_vertex_attributes);
	
	// Construct stars VBO
	auto& vbo = stars_model->get_vertex_buffer();
	vbo = std::make_unique<gl::vertex_buffer>(gl::buffer_usage::static_draw, std::as_bytes(std::span{star_vertex_data}));
	stars_model->set_vertex_offset(0);
	stars_model->set_vertex_stride(star_vertex_stride);
	
	// Construct star model group
	stars_model->get_groups().resize(1);
	render::model_group& stars_model_group = stars_model->get_groups().front();
	stars_model_group.id = "stars"_fnv1a32;
	stars_model_group.primitive_topology = gl::primitive_topology::point_list;
	stars_model_group.first_vertex = 0;
	stars_model_group.vertex_count = static_cast<std::uint32_t>(star_count);
	stars_model_group.material_index = 0;
	
	// Pass stars model to sky pass
	ctx.sky_pass->set_stars_model(stars_model);
	
	// Pass starlight illuminance to astronomy system
	ctx.m_astronomy_system->set_starlight_illuminance(starlight_illuminance);
	
	debug::log_trace("Generating fixed stars... OK");
}

void create_sun(::game& ctx)
{
	debug::log_trace("Generating Sun...");
	
	{
		// Create sun entity
		auto sun_archetype = ctx.resource_manager->load<entity::archetype>("sun.ent");
		entity::id sun_eid = sun_archetype->create(*ctx.entity_registry);
		ctx.entities["sun"_fnv1a32] = sun_eid;
		
		// Create sun directional light scene object
		ctx.sun_light = std::make_unique<scene::directional_light>();
		ctx.sun_light->set_shadow_caster(true);
		ctx.sun_light->set_shadow_framebuffer(ctx.shadow_map_framebuffer);
		ctx.sun_light->set_shadow_bias(0.005f);
		ctx.sun_light->set_shadow_max_distance(20.0f);
		ctx.sun_light->set_shadow_fade_range(5.0f);
		ctx.sun_light->set_shadow_cascade_count(4);
		ctx.sun_light->set_shadow_cascade_distribution(0.8f);
		
		// Add sun light to exterior scene
		ctx.exterior_scene->add_object(*ctx.sun_light);
		
		// Pass direct sun light scene object to shadow map pass and astronomy system
		ctx.m_astronomy_system->set_sun_light(ctx.sun_light.get());
	}
	
	debug::log_trace("Generating Sun... OK");
}

void create_earth_moon_system(::game& ctx)
{
	debug::log_trace("Generating Earth-Moon system...");
	
	{
		// Create Earth-Moon barycenter entity
		auto em_bary_archetype = ctx.resource_manager->load<entity::archetype>("em-bary.ent");
		entity::id em_bary_eid = em_bary_archetype->create(*ctx.entity_registry);
		ctx.entities["em_bary"_fnv1a32] = em_bary_eid;
		
		// Create Earth
		create_earth(ctx);
		
		// Create Moon
		create_moon(ctx);
	}
	
	debug::log_trace("Generating Earth-Moon system... OK");
}

void create_earth(::game& ctx)
{
	debug::log_trace("Generating Earth...");
	
	{
		// Create earth entity
		auto earth_archetype = ctx.resource_manager->load<entity::archetype>("earth.ent");
		entity::id earth_eid = earth_archetype->create(*ctx.entity_registry);
		ctx.entities["earth"_fnv1a32] = earth_eid;
		
		// Assign orbital parent
		ctx.entity_registry->get<::orbit_component>(earth_eid).parent = ctx.entities["em_bary"_fnv1a32];
	}
	
	debug::log_trace("Generating Earth... OK");
}

void create_moon(::game& ctx)
{
	debug::log_trace("Generating Moon...");
	
	{
		// Create lunar entity
		auto moon_archetype = ctx.resource_manager->load<entity::archetype>("moon.ent");
		entity::id moon_eid = moon_archetype->create(*ctx.entity_registry);
		ctx.entities["moon"_fnv1a32] = moon_eid;
		
		// Assign orbital parent
		ctx.entity_registry->get<::orbit_component>(moon_eid).parent = ctx.entities["em_bary"_fnv1a32];
		
		// Pass moon model to sky pass
		ctx.sky_pass->set_moon_model(ctx.resource_manager->load<render::model>("moon.mdl"));
		
		// Create moon directional light scene object
		ctx.moon_light = std::make_unique<scene::directional_light>();
		
		// Add moon light scene objects to exterior scene
		ctx.exterior_scene->add_object(*ctx.moon_light);
		
		// Pass moon light scene object to astronomy system
		ctx.m_astronomy_system->set_moon_light(ctx.moon_light.get());
	}
	
	debug::log_trace("Generating Moon... OK");
}

void enter_ecoregion(::game& ctx, const ecoregion& ecoregion)
{	
	debug::log_trace("Entering ecoregion {}...", ecoregion.name);
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
				
				// const float fbm = noise::fbm
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
	
	debug::log_trace("Entering ecoregion {}... OK", ecoregion.name);
}

} // namespace world
