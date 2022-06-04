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

#include "game/states/boot.hpp"
#include "animation/animation.hpp"
#include "animation/animator.hpp"
#include "animation/ease.hpp"
#include "animation/screen-transition.hpp"
#include "animation/timeline.hpp"
#include "application.hpp"
#include "debug/cli.hpp"
#include "debug/console-commands.hpp"
#include "debug/logger.hpp"
#include "game/context.hpp"
#include "gl/framebuffer.hpp"
#include "gl/pixel-format.hpp"
#include "gl/pixel-type.hpp"
#include "gl/rasterizer.hpp"
#include "gl/texture-2d.hpp"
#include "gl/texture-filter.hpp"
#include "gl/texture-wrapping.hpp"
#include "gl/vertex-array.hpp"
#include "gl/vertex-attribute.hpp"
#include "gl/vertex-buffer.hpp"
#include "render/material-flags.hpp"
#include "render/material-property.hpp"
#include "render/passes/bloom-pass.hpp"
#include "render/passes/clear-pass.hpp"
#include "render/passes/final-pass.hpp"
#include "render/passes/material-pass.hpp"
#include "render/passes/outline-pass.hpp"
#include "render/passes/shadow-map-pass.hpp"
#include "render/passes/sky-pass.hpp"
#include "render/passes/simple-pass.hpp"
#include "render/vertex-attribute.hpp"
#include "render/compositor.hpp"
#include "render/renderer.hpp"
#include "resources/resource-manager.hpp"
#include "resources/file-buffer.hpp"
#include "scene/scene.hpp"
#include "game/states/loading.hpp"
#include "entity/systems/behavior.hpp"
#include "entity/systems/camera.hpp"
#include "entity/systems/collision.hpp"
#include "entity/systems/constraint.hpp"
#include "entity/systems/locomotion.hpp"
#include "entity/systems/snapping.hpp"
#include "entity/systems/render.hpp"
#include "entity/systems/samara.hpp"
#include "entity/systems/subterrain.hpp"
#include "entity/systems/terrain.hpp"
#include "entity/systems/vegetation.hpp"
#include "entity/systems/spatial.hpp"
#include "entity/systems/painting.hpp"
#include "entity/systems/astronomy.hpp"
#include "entity/systems/blackbody.hpp"
#include "entity/systems/atmosphere.hpp"
#include "entity/systems/orbit.hpp"
#include "entity/systems/proteome.hpp"
#include "entity/commands.hpp"
#include "utility/paths.hpp"
#include "event/event-dispatcher.hpp"
#include "input/event-router.hpp"
#include "input/mapper.hpp"
#include "input/listener.hpp"
#include "input/gamepad.hpp"
#include "input/mouse.hpp"
#include "input/keyboard.hpp"
#include "configuration.hpp"
#include "input/scancode.hpp"
#include <cxxopts.hpp>
#include <dirent.h>
#include <entt/entt.hpp>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>
#include <execution>
#include <algorithm>

namespace game {
namespace state {
namespace boot {

static constexpr double seconds_per_day = 24.0 * 60.0 * 60.0;

static void parse_options(game::context* ctx, int argc, char** argv);
static void setup_resources(game::context* ctx);
static void load_config(game::context* ctx);
static void load_strings(game::context* ctx);
static void setup_window(game::context* ctx);
static void setup_rendering(game::context* ctx);
static void setup_scenes(game::context* ctx);
static void setup_animation(game::context* ctx);
static void setup_entities(game::context* ctx);
static void setup_systems(game::context* ctx);
static void setup_controls(game::context* ctx);
static void setup_cli(game::context* ctx);
static void setup_callbacks(game::context* ctx);

void enter(application* app, int argc, char** argv)
{
	// Get application logger
	debug::logger* logger = app->get_logger();
	
	logger->push_task("Running application bootloader");
	
	// Allocate game context
	game::context* ctx = new game::context();
	ctx->app = app;
	ctx->logger = logger;
	
	// Init game context
	try
	{
		parse_options(ctx, argc, argv);
		setup_resources(ctx);
		load_config(ctx);
		load_strings(ctx);
		setup_window(ctx);
		setup_rendering(ctx);
		setup_scenes(ctx);
		setup_animation(ctx);
		setup_entities(ctx);
		setup_systems(ctx);
		setup_controls(ctx);
		setup_cli(ctx);
		setup_callbacks(ctx);
	}
	catch (const std::exception& e)
	{
		logger->error("Caught exception: \"" + std::string(e.what()) + "\"");
		logger->pop_task(EXIT_FAILURE);
		return;
	}
	
	logger->pop_task(EXIT_SUCCESS);
	
	// Set update rate
	if (ctx->config->contains("update_rate"))
	{
		app->set_update_rate((*ctx->config)["update_rate"].get<double>());
	}
	
	// Setup initial application state
	application::state initial_state;
	initial_state.name = "loading";
	initial_state.enter = std::bind(game::state::loading::enter, ctx);
	initial_state.exit = std::bind(game::state::loading::exit, ctx);
	
	// Enter initial application state
	app->change_state(initial_state);
	
	return;
}

void exit(application* app)
{

}


void parse_options(game::context* ctx, int argc, char** argv)
{
	debug::logger* logger = ctx->logger;
	logger->push_task("Parsing command line options");
	
	try
	{
		cxxopts::Options options("Antkeeper", "Ant colony simulation game");
		options.add_options()
			("c,continue", "Continues from the last save")
			("d,data", "Sets the data package path", cxxopts::value<std::string>())
			("f,fullscreen", "Starts in fullscreen mode")
			("n,new-game", "Starts a new game")
			("q,quick-start", "Skips to the main menu")
			("r,reset", "Restores all settings to default")
			("v,vsync", "Enables or disables v-sync", cxxopts::value<int>())
			("w,windowed", "Starts in windowed mode");
		auto result = options.parse(argc, argv);
		
		// --continue
		if (result.count("continue"))
			ctx->option_continue = true;
		
		// --data
		if (result.count("data"))
			ctx->option_data = result["data"].as<std::string>();
		
		// --fullscreen
		if (result.count("fullscreen"))
			ctx->option_fullscreen = true;
		
		// --new-game
		if (result.count("new-game"))
			ctx->option_new_game = true;
		
		// --quick-start
		if (result.count("quick-start"))
			ctx->option_quick_start = true;
		
		// --reset
		if (result.count("reset"))
			ctx->option_reset = true;
		
		// --vsync
		if (result.count("vsync"))
			ctx->option_vsync = (result["vsync"].as<int>()) ? true : false;
		
		// --windowed
		if (result.count("windowed"))
			ctx->option_windowed = true; 
	}
	catch (const std::exception& e)
	{
		logger->error("Exception caught: \"" + std::string(e.what()) + "\"");
		logger->pop_task(EXIT_FAILURE);
		return;
	}
	
	logger->pop_task(EXIT_SUCCESS);
}

void setup_resources(game::context* ctx)
{
	debug::logger* logger = ctx->logger;
	
	// Setup resource manager
	ctx->resource_manager = new resource_manager(logger);
	
	// Determine application name
	std::string application_name;
	#if defined(_WIN32) || defined(__APPLE__)
		application_name = "Antkeeper";
	#else
		application_name = "antkeeper";
	#endif
	
	// Detect paths
	ctx->data_path = get_data_path(application_name);
	ctx->config_path = get_config_path(application_name);
	ctx->mods_path = ctx->config_path + "mods/";
	ctx->saves_path = ctx->config_path + "saves/";
	ctx->screenshots_path = ctx->config_path + "gallery/";
	ctx->controls_path = ctx->config_path + "controls/";
	
	// Log resource paths
	logger->log("Detected data path as \"" + ctx->data_path + "\"");
	logger->log("Detected config path as \"" + ctx->config_path + "\"");
	
	// Create nonexistent config directories
	std::vector<std::string> config_paths;
	config_paths.push_back(ctx->config_path);
	config_paths.push_back(ctx->mods_path);
	config_paths.push_back(ctx->saves_path);
	config_paths.push_back(ctx->screenshots_path);
	config_paths.push_back(ctx->controls_path);
	for (const std::string& path: config_paths)
	{
		if (!path_exists(path))
		{
			logger->push_task("Creating directory \"" + path + "\"");
			if (create_directory(path))
			{
				logger->pop_task(EXIT_SUCCESS);
			}
			else
			{
				logger->pop_task(EXIT_FAILURE);
			}
		}
	}
	
	// Redirect logger output to log file on non-debug builds
	#if defined(NDEBUG)
		std::string log_filename = config_path + "log.txt";
		ctx->log_filestream.open(log_filename.c_str());
		ctx->log_filestream << logger->get_history();
		logger->redirect(&log_filestream);
	#endif
	
	// Scan for mods
	std::vector<std::string> mods;
	struct dirent** files = nullptr;
	if (int n = scandir(ctx->mods_path.c_str(), &files, NULL, alphasort); n >= 0)
	{
		for (int i = 0; i < n; ++i)
		{
			struct dirent* file = files[i];
			
			switch (file->d_type)
			{
				case DT_REG:
				case DT_DIR:
				{
					std::string mod_name = file->d_name;
					
					// Skip hidden files and directories
					if (mod_name.front() == '.')
						break;
					
					mods.push_back(mod_name);
				}
				
				default:
					break;
			}
		}
	}
	
	// Determine data package path
	if (ctx->option_data.has_value())
	{
		ctx->data_package_path = ctx->option_data.value();
		if (std::filesystem::path(ctx->data_package_path).is_relative())
			ctx->data_package_path = ctx->data_path + ctx->data_package_path;
	}
	else
	{
		ctx->data_package_path = ctx->data_path + "data.zip";
	}
	
	// Mount mods
	for (const std::string& mod_name: mods)
		ctx->resource_manager->mount(ctx->mods_path + mod_name);
	
	// Mount config path
	ctx->resource_manager->mount(ctx->config_path);
	
	// Mount data package
	ctx->resource_manager->mount(ctx->data_package_path);
	
	// Include resource search paths in order of priority
	ctx->resource_manager->include("/shaders/");
	ctx->resource_manager->include("/models/");
	ctx->resource_manager->include("/images/");
	ctx->resource_manager->include("/textures/");
	ctx->resource_manager->include("/materials/");
	ctx->resource_manager->include("/entities/");
	ctx->resource_manager->include("/behaviors/");
	ctx->resource_manager->include("/controls/");
	ctx->resource_manager->include("/localization/");
	ctx->resource_manager->include("/localization/fonts/");
	ctx->resource_manager->include("/biomes/");
	ctx->resource_manager->include("/traits/");
	ctx->resource_manager->include("/");
}

void load_config(game::context* ctx)
{
	debug::logger* logger = ctx->logger;
	logger->push_task("Loading config");
	
	// Load config file
	ctx->config = ctx->resource_manager->load<json>("config.json");
	if (!ctx->config)
	{
		logger->pop_task(EXIT_FAILURE);
		return;
	}
	
	logger->pop_task(EXIT_SUCCESS);
}

void load_strings(game::context* ctx)
{
	debug::logger* logger = ctx->logger;
	logger->push_task("Loading strings");
	
	ctx->string_table = ctx->resource_manager->load<string_table>("strings.csv");
	
	build_string_table_map(&ctx->string_table_map, *ctx->string_table);
	
	ctx->language_code = (*ctx->config)["language"].get<std::string>();
	ctx->language_index = -1;
	for (int i = 2; i < (*ctx->string_table)[0].size(); ++i)
	{
		if ((*ctx->string_table)[0][i] == ctx->language_code)
			ctx->language_index = i;
	}
	
	logger->log("lang index: " + std::to_string(ctx->language_index));
	
	ctx->strings = &ctx->string_table_map[ctx->language_code];
	
	logger->pop_task(EXIT_SUCCESS);
}

void setup_window(game::context* ctx)
{
	debug::logger* logger = ctx->logger;
	logger->push_task("Setting up window");
	
	application* app = ctx->app;
	json* config = ctx->config;
	
	// Set fullscreen or windowed mode
	bool fullscreen = true;
	if (ctx->option_fullscreen.has_value())
		fullscreen = true;
	else if (ctx->option_windowed.has_value())
		fullscreen = false;
	else if (config->contains("fullscreen"))
		fullscreen = (*config)["fullscreen"].get<bool>();
	app->set_fullscreen(fullscreen);
	
	// Set resolution
	const auto& display_dimensions = ctx->app->get_display_dimensions();
	int2 resolution = {display_dimensions[0], display_dimensions[1]};
	if (fullscreen)
	{
		if (config->contains("fullscreen_resolution"))
		{
			resolution.x = (*config)["fullscreen_resolution"][0].get<int>();
			resolution.y = (*config)["fullscreen_resolution"][1].get<int>();
		}
	}
	else
	{
		if (config->contains("windowed_resolution"))
		{
			resolution.x = (*config)["windowed_resolution"][0].get<int>();
			resolution.y = (*config)["windowed_resolution"][1].get<int>();
		}
	}
	app->resize_window(resolution.x, resolution.y);
	
	// Set v-sync
	bool vsync = true;
	if (ctx->option_vsync.has_value())
		vsync = (ctx->option_vsync.value() != 0);
	else if (config->contains("vsync"))
		vsync = (*config)["vsync"].get<bool>();
	app->set_vsync(vsync);
	
	// Set title
	app->set_title((*ctx->strings)["title"]);
	
	// Show window
	ctx->app->get_rasterizer()->set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
	ctx->app->get_rasterizer()->clear_framebuffer(true, false, false);
	app->show_window();
	ctx->app->swap_buffers();
	
	logger->pop_task(EXIT_SUCCESS);
}

void setup_rendering(game::context* ctx)
{
	debug::logger* logger = ctx->logger;
	logger->push_task("Setting up rendering");
	
	// Get rasterizer from application
	ctx->rasterizer = ctx->app->get_rasterizer();
	
	// Get default framebuffer
	const gl::framebuffer& default_framebuffer = ctx->rasterizer->get_default_framebuffer();
	const auto& viewport_dimensions = default_framebuffer.get_dimensions();
	
	// Create HDR framebuffer (32F color, 32F depth)
	ctx->framebuffer_hdr_color = new gl::texture_2d(viewport_dimensions[0], viewport_dimensions[1], gl::pixel_type::float_32, gl::pixel_format::rgb);
	ctx->framebuffer_hdr_color->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx->framebuffer_hdr_color->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx->framebuffer_hdr_color->set_max_anisotropy(0.0f);
	ctx->framebuffer_hdr_depth = new gl::texture_2d(viewport_dimensions[0], viewport_dimensions[1], gl::pixel_type::float_32, gl::pixel_format::ds);
	ctx->framebuffer_hdr_depth->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx->framebuffer_hdr_depth->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx->framebuffer_hdr_depth->set_max_anisotropy(0.0f);
	ctx->framebuffer_hdr = new gl::framebuffer(viewport_dimensions[0], viewport_dimensions[1]);
	ctx->framebuffer_hdr->attach(gl::framebuffer_attachment_type::color, ctx->framebuffer_hdr_color);
	ctx->framebuffer_hdr->attach(gl::framebuffer_attachment_type::depth, ctx->framebuffer_hdr_depth);
	ctx->framebuffer_hdr->attach(gl::framebuffer_attachment_type::stencil, ctx->framebuffer_hdr_depth);
	
	// Create shadow map framebuffer
	int shadow_map_resolution = 4096;
	if (ctx->config->contains("shadow_map_resolution"))
	{
		shadow_map_resolution = (*ctx->config)["shadow_map_resolution"].get<int>();
	}
	ctx->shadow_map_depth_texture = new gl::texture_2d(shadow_map_resolution, shadow_map_resolution, gl::pixel_type::float_32, gl::pixel_format::d);
	ctx->shadow_map_depth_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx->shadow_map_depth_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx->shadow_map_depth_texture->set_max_anisotropy(0.0f);
	ctx->shadow_map_framebuffer = new gl::framebuffer(shadow_map_resolution, shadow_map_resolution);
	ctx->shadow_map_framebuffer->attach(gl::framebuffer_attachment_type::depth, ctx->shadow_map_depth_texture);
	
	// Create bloom pingpong framebuffers (16F color, no depth)
	int bloom_width = viewport_dimensions[0] >> 1;
	int bloom_height = viewport_dimensions[1] >> 1;
	ctx->bloom_texture = new gl::texture_2d(bloom_width, bloom_height, gl::pixel_type::float_16, gl::pixel_format::rgb);
	ctx->bloom_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx->bloom_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx->bloom_texture->set_max_anisotropy(0.0f);
	ctx->framebuffer_bloom = new gl::framebuffer(bloom_width, bloom_height);
	ctx->framebuffer_bloom->attach(gl::framebuffer_attachment_type::color, ctx->bloom_texture);
	
	// Load blue noise texture
	gl::texture_2d* blue_noise_map = ctx->resource_manager->load<gl::texture_2d>("blue-noise.tex");
	
	// Load fallback material
	ctx->fallback_material = ctx->resource_manager->load<render::material>("fallback.mtl");
	
	// Setup common render passes
	{
		ctx->common_bloom_pass = new render::bloom_pass(ctx->rasterizer, ctx->framebuffer_bloom, ctx->resource_manager);
		ctx->common_bloom_pass->set_source_texture(ctx->framebuffer_hdr_color);
		ctx->common_bloom_pass->set_brightness_threshold(1.0f);
		ctx->common_bloom_pass->set_blur_iterations(5);
		
		ctx->common_final_pass = new render::final_pass(ctx->rasterizer, &ctx->rasterizer->get_default_framebuffer(), ctx->resource_manager);
		ctx->common_final_pass->set_color_texture(ctx->framebuffer_hdr_color);
		ctx->common_final_pass->set_bloom_texture(ctx->bloom_texture);
		ctx->common_final_pass->set_blue_noise_texture(blue_noise_map);
	}
	
	// Setup UI compositor
	{
		ctx->ui_clear_pass = new render::clear_pass(ctx->rasterizer, &ctx->rasterizer->get_default_framebuffer());
		ctx->ui_clear_pass->set_cleared_buffers(false, true, false);
		ctx->ui_clear_pass->set_clear_depth(0.0f);
		
		ctx->ui_material_pass = new render::material_pass(ctx->rasterizer, &ctx->rasterizer->get_default_framebuffer(), ctx->resource_manager);
		ctx->ui_material_pass->set_fallback_material(ctx->fallback_material);
		
		ctx->ui_compositor = new render::compositor();
		ctx->ui_compositor->add_pass(ctx->ui_clear_pass);
		ctx->ui_compositor->add_pass(ctx->ui_material_pass);
	}
	
	// Setup underground compositor
	{
		ctx->underground_clear_pass = new render::clear_pass(ctx->rasterizer, ctx->framebuffer_hdr);
		ctx->underground_clear_pass->set_cleared_buffers(true, true, false);
		ctx->underground_clear_pass->set_clear_color({1, 0, 1, 0});
		ctx->underground_clear_pass->set_clear_depth(0.0f);
		
		ctx->underground_material_pass = new render::material_pass(ctx->rasterizer, ctx->framebuffer_hdr, ctx->resource_manager);
		ctx->underground_material_pass->set_fallback_material(ctx->fallback_material);
		ctx->app->get_event_dispatcher()->subscribe<mouse_moved_event>(ctx->underground_material_pass);
		
		ctx->underground_compositor = new render::compositor();
		ctx->underground_compositor->add_pass(ctx->underground_clear_pass);
		ctx->underground_compositor->add_pass(ctx->underground_material_pass);
		ctx->underground_compositor->add_pass(ctx->common_bloom_pass);
		ctx->underground_compositor->add_pass(ctx->common_final_pass);
	}
	
	// Setup surface compositor
	{
		ctx->surface_shadow_map_clear_pass = new render::clear_pass(ctx->rasterizer, ctx->shadow_map_framebuffer);
		ctx->surface_shadow_map_clear_pass->set_cleared_buffers(false, true, false);
		ctx->surface_shadow_map_clear_pass->set_clear_depth(1.0f);
		
		ctx->surface_shadow_map_pass = new render::shadow_map_pass(ctx->rasterizer, ctx->shadow_map_framebuffer, ctx->resource_manager);
		ctx->surface_shadow_map_pass->set_split_scheme_weight(0.75f);
		
		ctx->surface_clear_pass = new render::clear_pass(ctx->rasterizer, ctx->framebuffer_hdr);
		ctx->surface_clear_pass->set_cleared_buffers(true, true, true);
		ctx->surface_clear_pass->set_clear_depth(0.0f);
		
		ctx->surface_sky_pass = new render::sky_pass(ctx->rasterizer, ctx->framebuffer_hdr, ctx->resource_manager);
		ctx->app->get_event_dispatcher()->subscribe<mouse_moved_event>(ctx->surface_sky_pass);
		
		ctx->surface_material_pass = new render::material_pass(ctx->rasterizer, ctx->framebuffer_hdr, ctx->resource_manager);
		ctx->surface_material_pass->set_fallback_material(ctx->fallback_material);
		ctx->surface_material_pass->shadow_map_pass = ctx->surface_shadow_map_pass;
		ctx->surface_material_pass->shadow_map = ctx->shadow_map_depth_texture;
		ctx->app->get_event_dispatcher()->subscribe<mouse_moved_event>(ctx->surface_material_pass);
		
		ctx->surface_outline_pass = new render::outline_pass(ctx->rasterizer, ctx->framebuffer_hdr, ctx->resource_manager);
		ctx->surface_outline_pass->set_outline_width(0.25f);
		ctx->surface_outline_pass->set_outline_color(float4{1.0f, 1.0f, 1.0f, 1.0f});
		
		ctx->surface_compositor = new render::compositor();
		ctx->surface_compositor->add_pass(ctx->surface_shadow_map_clear_pass);
		ctx->surface_compositor->add_pass(ctx->surface_shadow_map_pass);
		ctx->surface_compositor->add_pass(ctx->surface_clear_pass);
		ctx->surface_compositor->add_pass(ctx->surface_sky_pass);
		ctx->surface_compositor->add_pass(ctx->surface_material_pass);
		//ctx->surface_compositor->add_pass(ctx->surface_outline_pass);
		ctx->surface_compositor->add_pass(ctx->common_bloom_pass);
		ctx->surface_compositor->add_pass(ctx->common_final_pass);
	}
	
	// Create billboard VAO
	{
		const float billboard_vertex_data[] =
		{
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		std::size_t billboard_vertex_size = 8;
		std::size_t billboard_vertex_stride = sizeof(float) * billboard_vertex_size;
		std::size_t billboard_vertex_count = 6;

		ctx->billboard_vbo = new gl::vertex_buffer(sizeof(float) * billboard_vertex_size * billboard_vertex_count, billboard_vertex_data);
		ctx->billboard_vao = new gl::vertex_array();
		
		std::size_t attribute_offset = 0;
		
		// Define position vertex attribute
		gl::vertex_attribute position_attribute;
		position_attribute.buffer = ctx->billboard_vbo;
		position_attribute.offset = attribute_offset;
		position_attribute.stride = billboard_vertex_stride;
		position_attribute.type = gl::vertex_attribute_type::float_32;
		position_attribute.components = 3;
		attribute_offset += position_attribute.components * sizeof(float);
		
		// Define UV vertex attribute
		gl::vertex_attribute uv_attribute;
		uv_attribute.buffer = ctx->billboard_vbo;
		uv_attribute.offset = attribute_offset;
		uv_attribute.stride = billboard_vertex_stride;
		uv_attribute.type = gl::vertex_attribute_type::float_32;
		uv_attribute.components = 2;
		attribute_offset += uv_attribute.components * sizeof(float);
		
		// Define barycentric vertex attribute
		gl::vertex_attribute barycentric_attribute;
		barycentric_attribute.buffer = ctx->billboard_vbo;
		barycentric_attribute.offset = attribute_offset;
		barycentric_attribute.stride = billboard_vertex_stride;
		barycentric_attribute.type = gl::vertex_attribute_type::float_32;
		barycentric_attribute.components = 3;
		attribute_offset += barycentric_attribute.components * sizeof(float);
			
		// Bind vertex attributes to VAO
		ctx->billboard_vao->bind(render::vertex_attribute::position, position_attribute);
		ctx->billboard_vao->bind(render::vertex_attribute::uv, uv_attribute);
		ctx->billboard_vao->bind(render::vertex_attribute::barycentric, barycentric_attribute);
	}
	
	// Create renderer
	ctx->renderer = new render::renderer();
	ctx->renderer->set_billboard_vao(ctx->billboard_vao);
	
	logger->pop_task(EXIT_SUCCESS);
}

void setup_scenes(game::context* ctx)
{
	debug::logger* logger = ctx->logger;
	logger->push_task("Setting up scenes");
	
	// Get default framebuffer
	const auto& viewport_dimensions = ctx->rasterizer->get_default_framebuffer().get_dimensions();
	const float viewport_aspect_ratio = static_cast<float>(viewport_dimensions[0]) / static_cast<float>(viewport_dimensions[1]);
	
	// Create infinite culling mask
	const float inf = std::numeric_limits<float>::infinity();
	ctx->no_cull = {{-inf, -inf, -inf}, {inf, inf, inf}};
	
	// Setup UI camera
	ctx->ui_camera = new scene::camera();
	ctx->ui_camera->set_compositor(ctx->ui_compositor);
	auto viewport = ctx->app->get_viewport_dimensions();
	float clip_left = -viewport[0] * 0.5f;
	float clip_right = viewport[0] * 0.5f;
	float clip_top = -viewport[1] * 0.5f;
	float clip_bottom = viewport[1] * 0.5f;
	float clip_near = 0.0f;
	float clip_far = 1000.0f;
	ctx->ui_camera->set_orthographic(clip_left, clip_right, clip_top, clip_bottom, clip_near, clip_far);
	
	// Setup underground camera
	ctx->underground_camera = new scene::camera();
	ctx->underground_camera->set_perspective(math::radians<float>(45.0f), viewport_aspect_ratio, 0.1f, 1000.0f);
	ctx->underground_camera->set_compositor(ctx->underground_compositor);
	ctx->underground_camera->set_composite_index(0);
	ctx->underground_camera->set_active(false);
	
	// Setup surface camera
	ctx->surface_camera = new scene::camera();
	ctx->surface_camera->set_perspective(math::radians<float>(45.0f), viewport_aspect_ratio, 0.1f, 1000.0f);
	ctx->surface_camera->set_compositor(ctx->surface_compositor);
	ctx->surface_camera->set_composite_index(0);
	ctx->surface_camera->set_active(false);
	
	// Setup UI scene
	{
		ctx->ui_scene = new scene::collection();
		
		const gl::texture_2d* splash_texture = ctx->resource_manager->load<gl::texture_2d>("splash.tex");
		auto splash_dimensions = splash_texture->get_dimensions();
		ctx->splash_billboard_material = new render::material();
		ctx->splash_billboard_material->set_shader_program(ctx->resource_manager->load<gl::shader_program>("ui-element-textured.glsl"));
		ctx->splash_billboard_material->add_property<const gl::texture_2d*>("background")->set_value(splash_texture);
		ctx->splash_billboard_material->add_property<float4>("tint")->set_value(float4{1, 1, 1, 1});
		ctx->splash_billboard_material->update_tweens();
		ctx->splash_billboard = new scene::billboard();
		ctx->splash_billboard->set_material(ctx->splash_billboard_material);
		ctx->splash_billboard->set_scale({(float)std::get<0>(splash_dimensions) * 0.5f, (float)std::get<1>(splash_dimensions) * 0.5f, 1.0f});
		ctx->splash_billboard->set_translation({0.0f, 0.0f, 0.0f});
		ctx->splash_billboard->update_tweens();
		
		// Create camera flash billboard
		
		render::material* flash_material = new render::material();
		flash_material->set_shader_program(ctx->resource_manager->load<gl::shader_program>("ui-element-untextured.glsl"));
		auto flash_tint = flash_material->add_property<float4>("tint");
		flash_tint->set_value(float4{1, 1, 1, 1});
		//flash_tint->set_tween_interpolator(ease<float4>::out_quad);
		
		flash_material->set_flags(MATERIAL_FLAG_TRANSLUCENT);
		flash_material->update_tweens();
		
		ctx->camera_flash_billboard = new scene::billboard();
		ctx->camera_flash_billboard->set_material(flash_material);
		ctx->camera_flash_billboard->set_scale({(float)viewport_dimensions[0] * 0.5f, (float)viewport_dimensions[1] * 0.5f, 1.0f});
		ctx->camera_flash_billboard->set_translation({0.0f, 0.0f, 0.0f});
		ctx->camera_flash_billboard->update_tweens();
		
		// Create depth debug billboard
		/*
		material* depth_debug_material = new material();
		depth_debug_material->set_shader_program(ctx->resource_manager->load<gl::shader_program>("ui-element-textured.glsl"));
		depth_debug_material->add_property<const gl::texture_2d*>("background")->set_value(shadow_map_depth_texture);
		depth_debug_material->add_property<float4>("tint")->set_value(float4{1, 1, 1, 1});
		billboard* depth_debug_billboard = new billboard();
		depth_debug_billboard->set_material(depth_debug_material);
		depth_debug_billboard->set_scale({128, 128, 1});
		depth_debug_billboard->set_translation({-960 + 128, 1080 * 0.5f - 128, 0});
		depth_debug_billboard->update_tweens();
		ui_system->get_scene()->add_object(depth_debug_billboard);
		*/
		
		ctx->ui_scene->add_object(ctx->ui_camera);
	}
	
	// Setup underground scene
	{
		ctx->underground_scene = new scene::collection();
		
		ctx->underground_ambient_light = new scene::ambient_light();
		ctx->underground_ambient_light->set_color({1, 1, 1});
		ctx->underground_ambient_light->set_intensity(0.1f);
		ctx->underground_ambient_light->update_tweens();
		
		ctx->flashlight_spot_light = new scene::spot_light();
		ctx->flashlight_spot_light->set_color({1, 1, 1});
		ctx->flashlight_spot_light->set_intensity(1.0f);
		ctx->flashlight_spot_light->set_attenuation({1.0f, 0.0f, 0.0f});
		ctx->flashlight_spot_light->set_cutoff({math::radians(10.0f), math::radians(19.0f)});
		
		ctx->underground_scene->add_object(ctx->underground_camera);
		ctx->underground_scene->add_object(ctx->underground_ambient_light);
		//ctx->underground_scene->add_object(ctx->flashlight_spot_light);
	}
	
	// Setup surface scene
	{
		ctx->surface_scene = new scene::collection();
		ctx->surface_scene->add_object(ctx->surface_camera);
	}
	
	// Clear active scene
	ctx->active_scene = nullptr;
	
	logger->pop_task(EXIT_SUCCESS);
}

void setup_animation(game::context* ctx)
{
	// Setup timeline system
	ctx->timeline = new timeline();
	ctx->timeline->set_autoremove(true);

	// Setup animator
	ctx->animator = new animator();
	
	// Create fade transition
	ctx->fade_transition = new screen_transition();
	ctx->fade_transition->get_material()->set_shader_program(ctx->resource_manager->load<gl::shader_program>("fade-transition.glsl"));
	ctx->fade_transition_color = ctx->fade_transition->get_material()->add_property<float3>("color");
	ctx->fade_transition_color->set_value({0, 0, 0});
	ctx->ui_scene->add_object(ctx->fade_transition->get_billboard());
	ctx->animator->add_animation(ctx->fade_transition->get_animation());
	
	// Create inner radial transition
	ctx->radial_transition_inner = new screen_transition();
	ctx->radial_transition_inner->get_material()->set_shader_program(ctx->resource_manager->load<gl::shader_program>("radial-transition-inner.glsl"));
	ctx->ui_scene->add_object(ctx->radial_transition_inner->get_billboard());
	ctx->animator->add_animation(ctx->radial_transition_inner->get_animation());
	
	// Create outer radial transition
	ctx->radial_transition_outer = new screen_transition();
	ctx->radial_transition_outer->get_material()->set_shader_program(ctx->resource_manager->load<gl::shader_program>("radial-transition-outer.glsl"));
	ctx->ui_scene->add_object(ctx->radial_transition_outer->get_billboard());
	ctx->animator->add_animation(ctx->radial_transition_outer->get_animation());
	
	// Create camera flash animation
	ctx->camera_flash_animation = new animation<float>();
	{
		ctx->camera_flash_animation->set_interpolator(ease<float>::out_sine);
		const float duration = 0.5f;
		animation_channel<float>* channel = ctx->camera_flash_animation->add_channel(0);
		channel->insert_keyframe({0.0f, 1.0f});
		channel->insert_keyframe({duration, 0.0f});
	}
}

void setup_entities(game::context* ctx)
{
	// Create entity registry
	ctx->entity_registry = new entt::registry();
}

void setup_systems(game::context* ctx)
{
	event_dispatcher* event_dispatcher = ctx->app->get_event_dispatcher();

	const auto& viewport_dimensions = ctx->app->get_viewport_dimensions();
	float4 viewport = {0.0f, 0.0f, static_cast<float>(viewport_dimensions[0]), static_cast<float>(viewport_dimensions[1])};
	
	// RGB wavelengths determined by matching wavelengths to XYZ, transforming XYZ to ACEScg, then selecting the max wavelengths for R, G, and B.
	const double3 rgb_wavelengths_nm = {602.224, 541.069, 448.143};
	
	// Setup terrain system
	ctx->terrain_system = new entity::system::terrain(*ctx->entity_registry);
	ctx->terrain_system->set_patch_subdivisions(30);
	ctx->terrain_system->set_patch_scene_collection(ctx->surface_scene);
	ctx->terrain_system->set_max_error(200.0);
	
	// Setup vegetation system
	//ctx->vegetation_system = new entity::system::vegetation(*ctx->entity_registry);
	//ctx->vegetation_system->set_terrain_patch_size(TERRAIN_PATCH_SIZE);
	//ctx->vegetation_system->set_vegetation_patch_resolution(VEGETATION_PATCH_RESOLUTION);
	//ctx->vegetation_system->set_vegetation_density(1.0f);
	//ctx->vegetation_system->set_vegetation_model(ctx->resource_manager->load<model>("grass-tuft.mdl"));
	//ctx->vegetation_system->set_scene(ctx->surface_scene);
	
	// Setup camera system
	ctx->camera_system = new entity::system::camera(*ctx->entity_registry);
	ctx->camera_system->set_viewport(viewport);
	event_dispatcher->subscribe<window_resized_event>(ctx->camera_system);
	
	// Setup subterrain system
	ctx->subterrain_system = new entity::system::subterrain(*ctx->entity_registry, ctx->resource_manager);
	ctx->subterrain_system->set_scene(ctx->underground_scene);
	
	// Setup collision system
	ctx->collision_system = new entity::system::collision(*ctx->entity_registry);
	
	// Setup samara system
	ctx->samara_system = new entity::system::samara(*ctx->entity_registry);
	
	// Setup snapping system
	ctx->snapping_system = new entity::system::snapping(*ctx->entity_registry);
	
	// Setup behavior system
	ctx->behavior_system = new entity::system::behavior(*ctx->entity_registry);
	
	// Setup locomotion system
	ctx->locomotion_system = new entity::system::locomotion(*ctx->entity_registry);
	
	// Setup spatial system
	ctx->spatial_system = new entity::system::spatial(*ctx->entity_registry);
	
	// Setup constraint system
	ctx->constraint_system = new entity::system::constraint(*ctx->entity_registry);
	
	// Setup painting system
	ctx->painting_system = new entity::system::painting(*ctx->entity_registry, event_dispatcher, ctx->resource_manager);
	ctx->painting_system->set_scene(ctx->surface_scene);
	
	// Setup orbit system
	ctx->orbit_system = new entity::system::orbit(*ctx->entity_registry);
	
	// Setup blackbody system
	ctx->blackbody_system = new entity::system::blackbody(*ctx->entity_registry);
	ctx->blackbody_system->set_rgb_wavelengths(rgb_wavelengths_nm);
	
	// Setup atmosphere system
	ctx->atmosphere_system = new entity::system::atmosphere(*ctx->entity_registry);
	ctx->atmosphere_system->set_rgb_wavelengths(rgb_wavelengths_nm);
	
	// Setup astronomy system
	ctx->astronomy_system = new entity::system::astronomy(*ctx->entity_registry);
	ctx->astronomy_system->set_sky_pass(ctx->surface_sky_pass);
	
	// Setup proteome system
	ctx->proteome_system = new entity::system::proteome(*ctx->entity_registry);
	
	// Set time scale
	double time_scale = 60.0;
	if (ctx->config->contains("time_scale"))
	{
		time_scale = (*ctx->config)["time_scale"].get<double>();
	}
	
	ctx->orbit_system->set_time_scale(time_scale / seconds_per_day);
	ctx->astronomy_system->set_time_scale(time_scale / seconds_per_day);
	
	// Setup render system
	ctx->render_system = new entity::system::render(*ctx->entity_registry);
	ctx->render_system->add_layer(ctx->underground_scene);
	ctx->render_system->add_layer(ctx->surface_scene);
	ctx->render_system->add_layer(ctx->ui_scene);
	ctx->render_system->set_renderer(ctx->renderer);
}

void setup_controls(game::context* ctx)
{
	event_dispatcher* event_dispatcher = ctx->app->get_event_dispatcher();
	
	// Setup input event routing
	ctx->input_event_router = new input::event_router();
	ctx->input_event_router->set_event_dispatcher(event_dispatcher);
	
	// Setup input mapper
	ctx->input_mapper = new input::mapper();
	ctx->input_mapper->set_event_dispatcher(event_dispatcher);
	
	// Setup input listener
	ctx->input_listener = new input::listener();
	ctx->input_listener->set_event_dispatcher(event_dispatcher);
	
	// Load SDL game controller mappings database
	ctx->logger->push_task("Loading SDL game controller mappings from database");
	file_buffer* game_controller_db = ctx->resource_manager->load<file_buffer>("gamecontrollerdb.txt");
	if (!game_controller_db)
	{
		ctx->logger->pop_task(EXIT_FAILURE);
	}
	else
	{
		ctx->app->add_game_controller_mappings(game_controller_db->data(), game_controller_db->size());
		ctx->resource_manager->unload("gamecontrollerdb.txt");
		ctx->logger->pop_task(EXIT_SUCCESS);
	}	
}

void setup_cli(game::context* ctx)
{
	ctx->cli = new debug::cli();
	ctx->cli->register_command("echo", debug::cc::echo);
	ctx->cli->register_command("exit", std::function<std::string()>(std::bind(&debug::cc::exit, ctx)));
	ctx->cli->register_command("scrot", std::function<std::string()>(std::bind(&debug::cc::scrot, ctx)));
	ctx->cli->register_command("cue", std::function<std::string(float, std::string)>(std::bind(&debug::cc::cue, ctx, std::placeholders::_1, std::placeholders::_2)));
	//std::string cmd = "cue 20 exit";
	//logger->log(cmd);
	//logger->log(cli.interpret(cmd));
}

void setup_callbacks(game::context* ctx)
{
	// Set update callback
	ctx->app->set_update_callback
	(
		[ctx](double t, double dt)
		{
			// Update controls
			for (const auto& control: ctx->controls)
				control.second->update();
			
			// Update processes
			std::for_each
			(
				std::execution::par,
				ctx->processes.begin(),
				ctx->processes.end(),
				[t, dt](const auto& process)
				{
					process.second(t, dt);
				}
			);
			
			// Update tweens
			ctx->surface_sky_pass->update_tweens();
			ctx->surface_scene->update_tweens();
			ctx->underground_scene->update_tweens();
			ctx->ui_scene->update_tweens();
			
			ctx->timeline->advance(dt);
			



			
			ctx->terrain_system->update(t, dt);
			//ctx->vegetation_system->update(t, dt);
			ctx->snapping_system->update(t, dt);
			ctx->subterrain_system->update(t, dt);
			ctx->collision_system->update(t, dt);
			ctx->samara_system->update(t, dt);
			ctx->behavior_system->update(t, dt);
			ctx->locomotion_system->update(t, dt);
			ctx->camera_system->update(t, dt);
			
			ctx->orbit_system->update(t, dt);
			ctx->blackbody_system->update(t, dt);
			ctx->atmosphere_system->update(t, dt);
			ctx->astronomy_system->update(t, dt);
			ctx->spatial_system->update(t, dt);
			ctx->constraint_system->update(t, dt);
			ctx->painting_system->update(t, dt);
			ctx->proteome_system->update(t, dt);
			
			ctx->render_system->update(t, dt);
			ctx->animator->animate(dt);
		}
	);
	
	// Set render callback
	ctx->app->set_render_callback
	(
		[ctx](double alpha)
		{
			ctx->render_system->draw(alpha);
		}
	);
}

} // namespace boot
} // namespace state
} // namespace game
