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
#include "entity/archetype.hpp"
#include "game/states/nuptial-flight.hpp"
#include "game/states/splash.hpp"
#include "game/states/forage.hpp"
#include "game/controls.hpp"
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
#include "utility/timestamp.hpp"
#include "type/type.hpp"
#include "configuration.hpp"
#include <stb/stb_image_write.h>
#include <unordered_set>
#include <codecvt>

#include "gl/texture-wrapping.hpp"
#include "gl/texture-filter.hpp"
#include "scene/text.hpp"
#include "render/material-flags.hpp"

namespace game {
namespace state {
namespace loading {

/// Loads control profile and calibrates gamepads
static void load_controls(game::context* ctx);

/// Loads typefaces and builds fonts
static void load_fonts(game::context* ctx);

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
	
	// Load fonts
	ctx->logger->push_task("Loading fonts");
	try
	{
		load_fonts(ctx);
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
	// If a control profile is set in the config file
	if (ctx->config->contains("control_profile"))
	{
		// Load control profile
		json* profile = ctx->resource_manager->load<json>((*ctx->config)["control_profile"].get<std::string>());
		
		// Apply control profile
		if (profile)
		{
			game::apply_control_profile(ctx, *profile);
		}
	}
	
	// Calibrate gamepads
	for (input::gamepad* gamepad: ctx->app->get_gamepads())
	{
		ctx->logger->push_task("Loading calibration for gamepad " + gamepad->get_guid());
		json* calibration = game::load_gamepad_calibration(ctx, gamepad);
		if (!calibration)
		{
			ctx->logger->pop_task(EXIT_FAILURE);
			
			ctx->logger->push_task("Generating default calibration for gamepad " + gamepad->get_guid());
			json default_calibration = game::default_gamepad_calibration();
			apply_gamepad_calibration(gamepad, default_calibration);
			
			if (!save_gamepad_calibration(ctx, gamepad, default_calibration))
				ctx->logger->pop_task(EXIT_FAILURE);
			else
				ctx->logger->pop_task(EXIT_SUCCESS);
		}
		else
		{
			ctx->logger->pop_task(EXIT_SUCCESS);
			apply_gamepad_calibration(gamepad, *calibration);
		}
	}
	
	// Toggle fullscreen
	ctx->controls["toggle_fullscreen"]->set_activated_callback
	(
		[ctx]()
		{
			bool fullscreen = !ctx->app->is_fullscreen();
			
			ctx->app->set_fullscreen(fullscreen);
			
			if (!fullscreen)
			{
				int2 resolution;
				resolution.x = (*ctx->config)["windowed_resolution"][0].get<int>();
				resolution.y = (*ctx->config)["windowed_resolution"][1].get<int>();
				
				ctx->app->resize_window(resolution.x, resolution.y);
			}
			
			(*ctx->config)["fullscreen"] = fullscreen;
		}
	);
	
	// Screenshot
	ctx->controls["screenshot"]->set_activated_callback
	(
		[ctx]()
		{
			std::string path = ctx->screenshots_path + "antkeeper-" + timestamp() + ".png";
			ctx->app->save_frame(path);
		}
	);
	
	// Menu back
	ctx->controls["menu_back"]->set_activated_callback
	(
		std::bind(&application::close, ctx->app, 0)
	);
	
	// Set activation threshold for menu navigation controls to mitigate drifting gamepad axes
	const float menu_activation_threshold = 0.1f;
	ctx->controls["menu_up"]->set_activation_threshold(menu_activation_threshold);
	ctx->controls["menu_down"]->set_activation_threshold(menu_activation_threshold);
	ctx->controls["menu_left"]->set_activation_threshold(menu_activation_threshold);
	ctx->controls["menu_right"]->set_activation_threshold(menu_activation_threshold);
}

static void build_bitmap_font(const type::typeface& typeface, float size, const std::unordered_set<char32_t>& charset, type::bitmap_font& font, render::material& material, gl::shader_program* shader_program)
{
	// Get font metrics for given size
	if (type::font_metrics metrics; typeface.get_metrics(size, metrics))
		font.set_font_metrics(metrics);
	
	// Format font bitmap
	image& font_bitmap = font.get_bitmap();
	font_bitmap.format(sizeof(std::byte), 1);
	
	// For each UTF-32 character code in the character set
	for (char32_t code: charset)
	{
		// Skip missing glyphs
		if (!typeface.has_glyph(code))
			continue;
		
		// Add glyph to font
		type::bitmap_glyph& glyph = font[code];
		typeface.get_metrics(size, code, glyph.metrics);
		typeface.get_bitmap(size, code, glyph.bitmap);
	}
	
	// Pack glyph bitmaps into the font bitmap
	font.pack();
	
	// Create font texture from bitmap
	gl::texture_2d* font_texture = new gl::texture_2d(font_bitmap.get_width(), font_bitmap.get_height(), gl::pixel_type::uint_8, gl::pixel_format::r, gl::color_space::linear, font_bitmap.get_pixels());
	font_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	font_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	
	// Create font material
	material.set_flags(MATERIAL_FLAG_TRANSLUCENT);
	material.add_property<const gl::texture_2d*>("font_bitmap")->set_value(font_texture);
	material.set_shader_program(shader_program);
}

void load_fonts(game::context* ctx)
{
	// Load typefaces
	if (auto it = ctx->strings->find("font_serif"); it != ctx->strings->end())
		ctx->typefaces["serif"] = ctx->resource_manager->load<type::typeface>(it->second);
	if (auto it = ctx->strings->find("font_sans_serif"); it != ctx->strings->end())
		ctx->typefaces["sans_serif"] = ctx->resource_manager->load<type::typeface>(it->second);
	if (auto it = ctx->strings->find("font_monospace"); it != ctx->strings->end())
		ctx->typefaces["monospace"] = ctx->resource_manager->load<type::typeface>(it->second);
	
	// Build character set
	std::unordered_set<char32_t> charset;
	{
		// Add all character codes from the basic Latin unicode block
		for (char32_t code = type::unicode::block::basic_latin.first; code <= type::unicode::block::basic_latin.last; ++code)
			charset.insert(code);
		
		// Add all character codes from game strings
		for (auto it = ctx->strings->begin(); it != ctx->strings->end(); ++it)
		{
			// Convert UTF-8 string to UTF-32
			std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
			std::u32string u32 = convert.from_bytes(it->second);
			
			/// Insert each character code from the UTF-32 string into the character set
			for (char32_t code: u32)
				charset.insert(code);
		}
	}
	
	// Load bitmap font shader
	gl::shader_program* bitmap_font_shader = ctx->resource_manager->load<gl::shader_program>("bitmap-font.glsl");
	
	// Build debug font
	if (auto it = ctx->typefaces.find("monospace"); it != ctx->typefaces.end())
	{
		build_bitmap_font(*it->second, 22.0f, charset, ctx->debug_font, ctx->debug_font_material, bitmap_font_shader);
	}
	
	// Build menu font
	if (auto it = ctx->typefaces.find("sans_serif"); it != ctx->typefaces.end())
	{
		build_bitmap_font(*it->second, 28.0f, charset, ctx->menu_font, ctx->menu_font_material, bitmap_font_shader);
	}
	
	// Build title font
	if (auto it = ctx->typefaces.find("serif"); it != ctx->typefaces.end())
	{
		build_bitmap_font(*it->second, 96.0f, charset, ctx->title_font, ctx->title_font_material, bitmap_font_shader);
	}
	
	// Create title text
	scene::text* title_text = new scene::text();
	title_text->set_material(&ctx->title_font_material);
	title_text->set_font(&ctx->title_font);
	title_text->set_color({1.0f, 1.0f, 1.0f, 1.0f});
	title_text->set_content((*ctx->strings)["title"]);
	ctx->ui_scene->add_object(title_text);
	
	// Align title text
	const auto& title_aabb = static_cast<const geom::aabb<float>&>(title_text->get_local_bounds());
	float title_w = title_aabb.max_point.x - title_aabb.min_point.x;
	float title_h = title_aabb.max_point.y - title_aabb.min_point.y;
	title_text->set_translation({std::round(-title_w * 0.5f), std::round(-title_h * 0.5f), 0.0f});
	
	// Create version string text
	scene::text* version_text = new scene::text();
	version_text->set_material(&ctx->debug_font_material);
	version_text->set_font(&ctx->debug_font);
	version_text->set_color({1.0f, 1.0f, 1.0f, 1.0f});
	version_text->set_content(ANTKEEPER_VERSION_STRING);
	ctx->ui_scene->add_object(version_text);
	
	// Align version string
	const auto& version_aabb = static_cast<const geom::aabb<float>&>(version_text->get_local_bounds());
	float version_w = version_aabb.max_point.x - version_aabb.min_point.x;
	float version_h = version_aabb.max_point.y - version_aabb.min_point.y;
	const float version_padding = 12.0f;
	auto viewport = ctx->app->get_viewport_dimensions();
	version_text->set_translation({viewport[0] * 0.5f - version_w - version_padding, -viewport[1] * 0.5f + version_padding, 0.0f});
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
	entity::archetype* sun_archetype = ctx->resource_manager->load<entity::archetype>("sun.ent");
	entity::id sun_eid = sun_archetype->create(*ctx->entity_registry);
	ctx->entities["sun"] = sun_eid;
	
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
	entity::archetype* planet_archetype = ctx->resource_manager->load<entity::archetype>("planet.ent");
	entity::id planet_eid = planet_archetype->create(*ctx->entity_registry);
	ctx->entities["planet"] = planet_eid;
	
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
	
	// Pass planet to astronomy system as reference body
	ctx->astronomy_system->set_reference_body(planet_eid);
	
	// Load sky model
	ctx->surface_sky_pass->set_sky_model(ctx->resource_manager->load<render::model>("sky-dome.mdl"));
}

void selenogenesis(game::context* ctx)
{
	// Create lunar entity
	entity::id moon_eid = ctx->entity_registry->create();
	ctx->entities["moon"] = moon_eid;
	
	// Pass moon model to sky pass
	ctx->surface_sky_pass->set_moon_model(ctx->resource_manager->load<render::model>("moon.mdl"));
}

void extrasolar_heliogenesis(game::context* ctx)
{
	// Load star catalog
	string_table* star_catalog = ctx->resource_manager->load<string_table>("stars.csv");
	
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
	ctx->resource_manager->unload("stars.csv");
	
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
	render::material* star_material = ctx->resource_manager->load<render::material>("fixed-star.mtl");
	
	// Create model group
	render::model_group* stars_model_group = stars_model->add_group("stars");
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
