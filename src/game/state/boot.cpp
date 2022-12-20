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

#include "game/state/boot.hpp"
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
#include "render/passes/ground-pass.hpp"
#include "render/passes/simple-pass.hpp"
#include "render/vertex-attribute.hpp"
#include "render/compositor.hpp"
#include "render/renderer.hpp"
#include "resources/resource-manager.hpp"
#include "resources/file-buffer.hpp"
#include "scene/scene.hpp"
#include "game/state/splash.hpp"
#include "game/system/behavior.hpp"
#include "game/system/camera.hpp"
#include "game/system/collision.hpp"
#include "game/system/constraint.hpp"
#include "game/system/locomotion.hpp"
#include "game/system/render.hpp"
#include "game/system/subterrain.hpp"
#include "game/system/terrain.hpp"
#include "game/system/vegetation.hpp"
#include "game/system/spatial.hpp"
#include "game/system/painting.hpp"
#include "game/system/astronomy.hpp"
#include "game/system/blackbody.hpp"
#include "game/system/atmosphere.hpp"
#include "game/system/orbit.hpp"
#include "game/system/proteome.hpp"
#include "game/system/steering.hpp"
#include "game/system/spring.hpp"
#include "entity/commands.hpp"
#include "utility/paths.hpp"
#include "event/event-dispatcher.hpp"
#include "input/event-router.hpp"
#include "input/mapper.hpp"
#include "input/listener.hpp"
#include "input/gamepad.hpp"
#include "input/mouse.hpp"
#include "input/keyboard.hpp"
#include "config.hpp"
#include "input/scancode.hpp"
#include "game/fonts.hpp"
#include "game/controls.hpp"
#include "game/save.hpp"
#include "game/menu.hpp"
#include "game/graphics.hpp"
#include "utility/timestamp.hpp"
#include "color/color.hpp"
#include <cxxopts.hpp>
#include <entt/entt.hpp>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>
#include <execution>
#include <algorithm>

namespace game {
namespace state {

boot::boot(game::context& ctx, int argc, char** argv):
	game::state::base(ctx)
{
	// Allocate application
	ctx.app = new application();
	
	// Get application logger
	ctx.logger = ctx.app->get_logger();
	
	// Boot process
	ctx.logger->push_task("Entering boot state");
	try
	{
		// Parse command line options
		parse_options(argc, argv);
		setup_resources();
		load_config();
		load_strings();
		setup_window();
		setup_rendering();
		setup_sound();
		setup_scenes();
		setup_animation();
		setup_entities();
		setup_systems();
		setup_controls();
		setup_ui();
		setup_debugging();
		setup_loop();
	}
	catch (const std::exception& e)
	{
		ctx.logger->error("Caught exception: \"" + std::string(e.what()) + "\"");
		ctx.logger->pop_task(EXIT_FAILURE);
		return;
	}
	ctx.logger->pop_task(EXIT_SUCCESS);
	
	// Push splash state
	ctx.state_machine.emplace(new game::state::splash(ctx));
	
	// Enter main loop
	loop();
}

boot::~boot()
{
	ctx.logger->push_task("Exiting boot state");
	
	// Close application
	delete ctx.app;
	ctx.app = nullptr;
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

void boot::parse_options(int argc, char** argv)
{
	debug::logger* logger = ctx.logger;
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
			option_continue = true;
		
		// --data
		if (result.count("data"))
			option_data = result["data"].as<std::string>();
		
		// --fullscreen
		if (result.count("fullscreen"))
			option_fullscreen = true;
		
		// --new-game
		if (result.count("new-game"))
			option_new_game = true;
		
		// --quick-start
		if (result.count("quick-start"))
			option_quick_start = true;
		
		// --reset
		if (result.count("reset"))
			option_reset = true;
		
		// --vsync
		if (result.count("vsync"))
			option_v_sync = (result["vsync"].as<int>()) ? true : false;
		
		// --windowed
		if (result.count("windowed"))
			option_windowed = true; 
	}
	catch (const std::exception& e)
	{
		logger->error("Exception caught: \"" + std::string(e.what()) + "\"");
		logger->pop_task(EXIT_FAILURE);
		return;
	}
	
	logger->pop_task(EXIT_SUCCESS);
}

void boot::setup_resources()
{
	debug::logger* logger = ctx.logger;
	
	// Setup resource manager
	ctx.resource_manager = new resource_manager(logger);
	
	// Determine application name
	std::string application_name;
	#if defined(_WIN32) || defined(__APPLE__)
		application_name = "Antkeeper";
	#else
		application_name = "antkeeper";
	#endif
	
	// Detect paths
	ctx.data_path = get_data_path(application_name);
	ctx.config_path = get_config_path(application_name);
	ctx.mods_path = ctx.config_path / "mods";
	ctx.saves_path = ctx.config_path / "saves";
	ctx.screenshots_path = ctx.config_path / "gallery";
	ctx.controls_path = ctx.config_path / "controls";
	
	// Log resource paths
	logger->log("Detected data path as \"" + ctx.data_path.string());
	logger->log("Detected config path as \"" + ctx.config_path.string());
	
	// Create nonexistent config directories
	std::vector<std::filesystem::path> config_paths;
	config_paths.push_back(ctx.config_path);
	config_paths.push_back(ctx.mods_path);
	config_paths.push_back(ctx.saves_path);
	config_paths.push_back(ctx.screenshots_path);
	config_paths.push_back(ctx.controls_path);
	for (const std::filesystem::path& path: config_paths)
	{
		if (!std::filesystem::exists(path))
		{
			logger->push_task("Creating directory \"" + path.string());
			if (std::filesystem::create_directories(path))
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
		std::filesystem::path log_path = ctx.config_path / "log.txt";
		ctx.log_filestream.open(log_path);
		ctx.log_filestream << logger->get_history();
		logger->redirect(&ctx.log_filestream);
	#endif
	
	// Scan for mods
	std::vector<std::filesystem::path> mod_paths;
	for (const auto& directory_entry: std::filesystem::directory_iterator(ctx.mods_path))
	{
		if (directory_entry.is_directory())
			mod_paths.push_back(directory_entry.path());
	}
	
	// Determine data package path
	if (option_data.has_value())
	{
		ctx.data_package_path = std::filesystem::path(option_data.value());
		if (ctx.data_package_path.is_relative())
			ctx.data_package_path = ctx.data_path / ctx.data_package_path;
	}
	else
	{
		ctx.data_package_path = ctx.data_path / "antkeeper.dat";
	}
	
	// Mount mods
	for (const std::filesystem::path& mod_path: mod_paths)
		ctx.resource_manager->mount(ctx.mods_path / mod_path);
	
	// Mount config path
	ctx.resource_manager->mount(ctx.config_path);
	
	// Mount data package
	ctx.resource_manager->mount(ctx.data_package_path);
	
	// Include resource search paths in order of priority
	ctx.resource_manager->include("/controls/");
	ctx.resource_manager->include("/");
}

void boot::load_config()
{
	debug::logger* logger = ctx.logger;
	logger->push_task("Loading config");
	
	// Load config file
	ctx.config = ctx.resource_manager->load<json>("config.json");
	if (!ctx.config)
	{
		logger->pop_task(EXIT_FAILURE);
		return;
	}
	
	logger->pop_task(EXIT_SUCCESS);
}

void boot::load_strings()
{
	debug::logger* logger = ctx.logger;
	logger->push_task("Loading strings");
	
	ctx.string_table = ctx.resource_manager->load<string_table>("strings.csv");
	
	build_string_table_map(&ctx.string_table_map, *ctx.string_table);
	
	ctx.language_code = (*ctx.config)["language"].get<std::string>();
	ctx.language_index = -1;
	for (int i = 2; i < (*ctx.string_table)[0].size(); ++i)
	{
		if ((*ctx.string_table)[0][i] == ctx.language_code)
			ctx.language_index = i - 2;
	}
	
	ctx.language_count = (*ctx.string_table)[0].size() - 2;
	logger->log("language count: " + std::to_string(ctx.language_count));
	logger->log("language index: " + std::to_string(ctx.language_index));
	logger->log("language code: " + ctx.language_code);
	
	ctx.strings = &ctx.string_table_map[ctx.language_code];
	
	logger->pop_task(EXIT_SUCCESS);
}

void boot::setup_window()
{
	debug::logger* logger = ctx.logger;
	logger->push_task("Setting up window");
	
	application* app = ctx.app;
	json* config = ctx.config;
	
	// Set fullscreen or windowed mode
	bool fullscreen = true;
	if (option_fullscreen.has_value())
		fullscreen = true;
	else if (option_windowed.has_value())
		fullscreen = false;
	else if (config->contains("fullscreen"))
		fullscreen = (*config)["fullscreen"].get<bool>();
	app->set_fullscreen(fullscreen);
	
	// Set resolution
	const auto& display_dimensions = ctx.app->get_display_dimensions();
	int2 resolution = {display_dimensions[0], display_dimensions[1]};
	if (fullscreen)
	{
		if (config->contains("fullscreen_resolution"))
		{
			resolution.x() = (*config)["fullscreen_resolution"][0].get<int>();
			resolution.y() = (*config)["fullscreen_resolution"][1].get<int>();
		}
	}
	else
	{
		if (config->contains("windowed_resolution"))
		{
			resolution.x() = (*config)["windowed_resolution"][0].get<int>();
			resolution.y() = (*config)["windowed_resolution"][1].get<int>();
		}
	}
	app->resize_window(resolution.x(), resolution.y());
	
	// Set v-sync
	bool v_sync = true;
	if (option_v_sync.has_value())
		v_sync = (option_v_sync.value() != 0);
	else if (config->contains("v_sync"))
		v_sync = (*config)["v_sync"].get<bool>();
	app->set_v_sync(v_sync);
	
	// Set title
	app->set_title((*ctx.strings)["application_title"]);
	
	// Show window
	ctx.app->get_rasterizer()->set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
	ctx.app->get_rasterizer()->clear_framebuffer(true, false, false);
	app->show_window();
	ctx.app->swap_buffers();
	
	logger->pop_task(EXIT_SUCCESS);
}

void boot::setup_rendering()
{
	debug::logger* logger = ctx.logger;
	logger->push_task("Setting up rendering");
	
	// Get rasterizer from application
	ctx.rasterizer = ctx.app->get_rasterizer();
	
	// Create framebuffers
	game::graphics::create_framebuffers(ctx);
	
	// Load blue noise texture
	gl::texture_2d* blue_noise_map = ctx.resource_manager->load<gl::texture_2d>("blue-noise.tex");
	
	// Load fallback material
	ctx.fallback_material = ctx.resource_manager->load<render::material>("fallback.mtl");
	
	// Setup common render passes
	{
		ctx.common_bloom_pass = new render::bloom_pass(ctx.rasterizer, ctx.bloom_framebuffer, ctx.resource_manager);
		ctx.common_bloom_pass->set_source_texture(ctx.hdr_color_texture);
		ctx.common_bloom_pass->set_brightness_threshold(1.0f);
		ctx.common_bloom_pass->set_blur_iterations(5);
		
		ctx.common_final_pass = new render::final_pass(ctx.rasterizer, &ctx.rasterizer->get_default_framebuffer(), ctx.resource_manager);
		ctx.common_final_pass->set_color_texture(ctx.hdr_color_texture);
		ctx.common_final_pass->set_bloom_texture(ctx.bloom_color_texture);
		ctx.common_final_pass->set_blue_noise_texture(blue_noise_map);
	}
	
	// Setup UI compositor
	{
		ctx.ui_clear_pass = new render::clear_pass(ctx.rasterizer, &ctx.rasterizer->get_default_framebuffer());
		ctx.ui_clear_pass->set_cleared_buffers(false, true, false);
		ctx.ui_clear_pass->set_clear_depth(-1.0f);
		
		ctx.ui_material_pass = new render::material_pass(ctx.rasterizer, &ctx.rasterizer->get_default_framebuffer(), ctx.resource_manager);
		ctx.ui_material_pass->set_fallback_material(ctx.fallback_material);
		
		ctx.ui_compositor = new render::compositor();
		ctx.ui_compositor->add_pass(ctx.ui_clear_pass);
		ctx.ui_compositor->add_pass(ctx.ui_material_pass);
	}
	
	// Setup underground compositor
	{
		ctx.underground_clear_pass = new render::clear_pass(ctx.rasterizer, ctx.hdr_framebuffer);
		ctx.underground_clear_pass->set_cleared_buffers(true, true, false);
		ctx.underground_clear_pass->set_clear_color({1, 0, 1, 0});
		ctx.underground_clear_pass->set_clear_depth(-1.0f);
		
		ctx.underground_material_pass = new render::material_pass(ctx.rasterizer, ctx.hdr_framebuffer, ctx.resource_manager);
		ctx.underground_material_pass->set_fallback_material(ctx.fallback_material);
		ctx.app->get_event_dispatcher()->subscribe<mouse_moved_event>(ctx.underground_material_pass);
		
		ctx.underground_compositor = new render::compositor();
		ctx.underground_compositor->add_pass(ctx.underground_clear_pass);
		ctx.underground_compositor->add_pass(ctx.underground_material_pass);
		ctx.underground_compositor->add_pass(ctx.common_bloom_pass);
		ctx.underground_compositor->add_pass(ctx.common_final_pass);
	}
	
	// Setup surface compositor
	{
		ctx.surface_shadow_map_clear_pass = new render::clear_pass(ctx.rasterizer, ctx.shadow_map_framebuffer);
		ctx.surface_shadow_map_clear_pass->set_cleared_buffers(false, true, false);
		ctx.surface_shadow_map_clear_pass->set_clear_depth(1.0f);
		
		ctx.surface_shadow_map_pass = new render::shadow_map_pass(ctx.rasterizer, ctx.shadow_map_framebuffer, ctx.resource_manager);
		ctx.surface_shadow_map_pass->set_split_scheme_weight(0.75f);
		
		ctx.surface_clear_pass = new render::clear_pass(ctx.rasterizer, ctx.hdr_framebuffer);
		ctx.surface_clear_pass->set_cleared_buffers(false, true, true);
		ctx.surface_clear_pass->set_clear_depth(-1.0f);
		
		ctx.sky_pass = new render::sky_pass(ctx.rasterizer, ctx.hdr_framebuffer, ctx.resource_manager);
		ctx.sky_pass->set_enabled(false);
		ctx.sky_pass->set_magnification(3.0f);
		ctx.app->get_event_dispatcher()->subscribe<mouse_moved_event>(ctx.sky_pass);
		
		ctx.ground_pass = new render::ground_pass(ctx.rasterizer, ctx.hdr_framebuffer, ctx.resource_manager);
		ctx.ground_pass->set_enabled(false);
		
		ctx.surface_material_pass = new render::material_pass(ctx.rasterizer, ctx.hdr_framebuffer, ctx.resource_manager);
		ctx.surface_material_pass->set_fallback_material(ctx.fallback_material);
		ctx.surface_material_pass->shadow_map_pass = ctx.surface_shadow_map_pass;
		ctx.surface_material_pass->shadow_map = ctx.shadow_map_depth_texture;
		ctx.app->get_event_dispatcher()->subscribe<mouse_moved_event>(ctx.surface_material_pass);
		
		ctx.surface_outline_pass = new render::outline_pass(ctx.rasterizer, ctx.hdr_framebuffer, ctx.resource_manager);
		ctx.surface_outline_pass->set_outline_width(0.25f);
		ctx.surface_outline_pass->set_outline_color(float4{1.0f, 1.0f, 1.0f, 1.0f});
		
		ctx.surface_compositor = new render::compositor();
		ctx.surface_compositor->add_pass(ctx.surface_shadow_map_clear_pass);
		ctx.surface_compositor->add_pass(ctx.surface_shadow_map_pass);
		ctx.surface_compositor->add_pass(ctx.surface_clear_pass);
		ctx.surface_compositor->add_pass(ctx.sky_pass);
		ctx.surface_compositor->add_pass(ctx.ground_pass);
		ctx.surface_compositor->add_pass(ctx.surface_material_pass);
		//ctx.surface_compositor->add_pass(ctx.surface_outline_pass);
		//ctx.surface_compositor->add_pass(ctx.common_bloom_pass);
		ctx.surface_compositor->add_pass(ctx.common_final_pass);
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

		ctx.billboard_vbo = new gl::vertex_buffer(sizeof(float) * billboard_vertex_size * billboard_vertex_count, billboard_vertex_data);
		ctx.billboard_vao = new gl::vertex_array();
		
		std::size_t attribute_offset = 0;
		
		// Define position vertex attribute
		gl::vertex_attribute position_attribute;
		position_attribute.buffer = ctx.billboard_vbo;
		position_attribute.offset = attribute_offset;
		position_attribute.stride = billboard_vertex_stride;
		position_attribute.type = gl::vertex_attribute_type::float_32;
		position_attribute.components = 3;
		attribute_offset += position_attribute.components * sizeof(float);
		
		// Define UV vertex attribute
		gl::vertex_attribute uv_attribute;
		uv_attribute.buffer = ctx.billboard_vbo;
		uv_attribute.offset = attribute_offset;
		uv_attribute.stride = billboard_vertex_stride;
		uv_attribute.type = gl::vertex_attribute_type::float_32;
		uv_attribute.components = 2;
		attribute_offset += uv_attribute.components * sizeof(float);
		
		// Define barycentric vertex attribute
		gl::vertex_attribute barycentric_attribute;
		barycentric_attribute.buffer = ctx.billboard_vbo;
		barycentric_attribute.offset = attribute_offset;
		barycentric_attribute.stride = billboard_vertex_stride;
		barycentric_attribute.type = gl::vertex_attribute_type::float_32;
		barycentric_attribute.components = 3;
		attribute_offset += barycentric_attribute.components * sizeof(float);
		
		// Bind vertex attributes to VAO
		ctx.billboard_vao->bind(render::vertex_attribute::position, position_attribute);
		ctx.billboard_vao->bind(render::vertex_attribute::uv, uv_attribute);
		ctx.billboard_vao->bind(render::vertex_attribute::barycentric, barycentric_attribute);
	}
	
	// Create renderer
	ctx.renderer = new render::renderer();
	ctx.renderer->set_billboard_vao(ctx.billboard_vao);
	
	logger->pop_task(EXIT_SUCCESS);
}

void boot::setup_sound()
{
	debug::logger* logger = ctx.logger;
	logger->push_task("Setting up sound");
	
	// Load master volume config
	ctx.master_volume = 1.0f;
	if (ctx.config->contains("master_volume"))
		ctx.master_volume = (*ctx.config)["master_volume"].get<float>();
	
	// Load ambience volume config
	ctx.ambience_volume = 1.0f;
	if (ctx.config->contains("ambience_volume"))
		ctx.ambience_volume = (*ctx.config)["ambience_volume"].get<float>();
	
	// Load effects volume config
	ctx.effects_volume = 1.0f;
	if (ctx.config->contains("effects_volume"))
		ctx.effects_volume = (*ctx.config)["effects_volume"].get<float>();
	
	// Load mono audio config
	ctx.mono_audio = false;
	if (ctx.config->contains("mono_audio"))
		ctx.mono_audio = (*ctx.config)["mono_audio"].get<bool>();
		
	// Load captions config
	ctx.captions = false;
	if (ctx.config->contains("captions"))
		ctx.captions = (*ctx.config)["captions"].get<bool>();
	
	// Load captions size config
	ctx.captions_size = 1.0f;
	if (ctx.config->contains("captions_size"))
		ctx.captions_size = (*ctx.config)["captions_size"].get<float>();
	
	logger->pop_task(EXIT_SUCCESS);
}

void boot::setup_scenes()
{
	debug::logger* logger = ctx.logger;
	logger->push_task("Setting up scenes");
	
	// Get default framebuffer
	const auto& viewport_dimensions = ctx.rasterizer->get_default_framebuffer().get_dimensions();
	const float viewport_aspect_ratio = static_cast<float>(viewport_dimensions[0]) / static_cast<float>(viewport_dimensions[1]);
	
	// Setup UI camera
	ctx.ui_camera = new scene::camera();
	ctx.ui_camera->set_compositor(ctx.ui_compositor);
	auto viewport = ctx.app->get_viewport_dimensions();
	float clip_left = -viewport[0] * 0.5f;
	float clip_right = viewport[0] * 0.5f;
	float clip_top = -viewport[1] * 0.5f;
	float clip_bottom = viewport[1] * 0.5f;
	float clip_near = 0.0f;
	float clip_far = 1000.0f;
	ctx.ui_camera->set_orthographic(clip_left, clip_right, clip_top, clip_bottom, clip_near, clip_far);
	ctx.ui_camera->look_at({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f});
	ctx.ui_camera->update_tweens();
	
	// Setup underground camera
	ctx.underground_camera = new scene::camera();
	ctx.underground_camera->set_perspective(math::radians<float>(45.0f), viewport_aspect_ratio, 0.1f, 1000.0f);
	ctx.underground_camera->set_compositor(ctx.underground_compositor);
	ctx.underground_camera->set_composite_index(0);
	ctx.underground_camera->set_active(false);
	
	// Setup surface camera
	ctx.surface_camera = new scene::camera();
	ctx.surface_camera->set_perspective(math::radians<float>(45.0f), viewport_aspect_ratio, 0.1f, 10000.0f);
	ctx.surface_camera->set_compositor(ctx.surface_compositor);
	ctx.surface_camera->set_composite_index(0);
	ctx.surface_camera->set_active(false);
	
	// Setup UI scene
	{
		ctx.ui_scene = new scene::collection();

		
		// Menu BG billboard
		render::material* menu_bg_material = new render::material();
		menu_bg_material->set_shader_program(ctx.resource_manager->load<gl::shader_program>("ui-element-untextured.glsl"));
		auto menu_bg_tint = menu_bg_material->add_property<float4>("tint");
		menu_bg_tint->set_value(float4{0.0f, 0.0f, 0.0f, 0.5f});
		menu_bg_material->set_flags(MATERIAL_FLAG_TRANSLUCENT);
		menu_bg_material->update_tweens();
		ctx.menu_bg_billboard = new scene::billboard();
		ctx.menu_bg_billboard->set_active(false);
		ctx.menu_bg_billboard->set_material(menu_bg_material);
		ctx.menu_bg_billboard->set_scale({(float)viewport_dimensions[0] * 0.5f, (float)viewport_dimensions[1] * 0.5f, 1.0f});
		ctx.menu_bg_billboard->set_translation({0.0f, 0.0f, -100.0f});
		ctx.menu_bg_billboard->update_tweens();
		
		// Create camera flash billboard
		
		render::material* flash_material = new render::material();
		flash_material->set_shader_program(ctx.resource_manager->load<gl::shader_program>("ui-element-untextured.glsl"));
		auto flash_tint = flash_material->add_property<float4>("tint");
		flash_tint->set_value(float4{1, 1, 1, 1});
		//flash_tint->set_tween_interpolator(ease<float4>::out_quad);
		
		flash_material->set_flags(MATERIAL_FLAG_TRANSLUCENT);
		flash_material->update_tweens();
		
		ctx.camera_flash_billboard = new scene::billboard();
		ctx.camera_flash_billboard->set_material(flash_material);
		ctx.camera_flash_billboard->set_scale({(float)viewport_dimensions[0] * 0.5f, (float)viewport_dimensions[1] * 0.5f, 1.0f});
		ctx.camera_flash_billboard->set_translation({0.0f, 0.0f, 0.0f});
		ctx.camera_flash_billboard->update_tweens();
		
		// Create depth debug billboard
		/*
		material* depth_debug_material = new material();
		depth_debug_material->set_shader_program(ctx.resource_manager->load<gl::shader_program>("ui-element-textured.glsl"));
		depth_debug_material->add_property<const gl::texture_2d*>("background")->set_value(shadow_map_depth_texture);
		depth_debug_material->add_property<float4>("tint")->set_value(float4{1, 1, 1, 1});
		billboard* depth_debug_billboard = new billboard();
		depth_debug_billboard->set_material(depth_debug_material);
		depth_debug_billboard->set_scale({128, 128, 1});
		depth_debug_billboard->set_translation({-960 + 128, 1080 * 0.5f - 128, 0});
		depth_debug_billboard->update_tweens();
		ui_system->get_scene()->add_object(depth_debug_billboard);
		*/
		
		ctx.ui_scene->add_object(ctx.ui_camera);
	}
	
	// Setup underground scene
	{
		ctx.underground_scene = new scene::collection();
		ctx.underground_scene->add_object(ctx.underground_camera);
	}
	
	// Setup surface scene
	{
		ctx.surface_scene = new scene::collection();
		ctx.surface_scene->add_object(ctx.surface_camera);
	}
	
	// Clear active scene
	ctx.active_scene = nullptr;
	
	logger->pop_task(EXIT_SUCCESS);
}

void boot::setup_animation()
{
	// Setup timeline system
	ctx.timeline = new timeline();
	ctx.timeline->set_autoremove(true);

	// Setup animator
	ctx.animator = new animator();
	
	// Create fade transition
	ctx.fade_transition = new screen_transition();
	ctx.fade_transition->get_material()->set_shader_program(ctx.resource_manager->load<gl::shader_program>("fade-transition.glsl"));
	ctx.fade_transition_color = ctx.fade_transition->get_material()->add_property<float3>("color");
	ctx.fade_transition_color->set_value({0, 0, 0});
	ctx.ui_scene->add_object(ctx.fade_transition->get_billboard());
	ctx.animator->add_animation(ctx.fade_transition->get_animation());
	
	// Create inner radial transition
	ctx.radial_transition_inner = new screen_transition();
	ctx.radial_transition_inner->get_material()->set_shader_program(ctx.resource_manager->load<gl::shader_program>("radial-transition-inner.glsl"));
	//ctx.ui_scene->add_object(ctx.radial_transition_inner->get_billboard());
	//ctx.animator->add_animation(ctx.radial_transition_inner->get_animation());
	
	// Create outer radial transition
	ctx.radial_transition_outer = new screen_transition();
	ctx.radial_transition_outer->get_material()->set_shader_program(ctx.resource_manager->load<gl::shader_program>("radial-transition-outer.glsl"));
	//ctx.ui_scene->add_object(ctx.radial_transition_outer->get_billboard());
	//ctx.animator->add_animation(ctx.radial_transition_outer->get_animation());
	
	// Menu BG animations
	{
		render::material_property<float4>* menu_bg_tint = static_cast<render::material_property<float4>*>(ctx.menu_bg_billboard->get_material()->get_property("tint"));
		auto menu_bg_frame_callback = [menu_bg_tint](int channel, const float& opacity)
		{
			menu_bg_tint->set_value(float4{0.0f, 0.0f, 0.0f, opacity});
		};
		
		// Create menu BG fade in animation
		ctx.menu_bg_fade_in_animation = new animation<float>();
		{
			ctx.menu_bg_fade_in_animation->set_interpolator(ease<float>::out_cubic);
			animation_channel<float>* channel = ctx.menu_bg_fade_in_animation->add_channel(0);
			channel->insert_keyframe({0.0f, 0.0f});
			channel->insert_keyframe({config::menu_fade_in_duration, config::menu_bg_opacity});
			ctx.menu_bg_fade_in_animation->set_frame_callback(menu_bg_frame_callback);
			ctx.menu_bg_fade_in_animation->set_start_callback
			(
				[&ctx = this->ctx, menu_bg_tint]()
				{
					ctx.ui_scene->add_object(ctx.menu_bg_billboard);
					
					menu_bg_tint->set_value(float4{0.0f, 0.0f, 0.0f, 0.0f});
					menu_bg_tint->update_tweens();
					ctx.menu_bg_billboard->set_active(true);
				}
			);
		}
		
		// Create menu BG fade out animation
		ctx.menu_bg_fade_out_animation = new animation<float>();
		{
			ctx.menu_bg_fade_out_animation->set_interpolator(ease<float>::out_cubic);
			animation_channel<float>* channel = ctx.menu_bg_fade_out_animation->add_channel(0);
			channel->insert_keyframe({0.0f, config::menu_bg_opacity});
			channel->insert_keyframe({config::menu_fade_out_duration, 0.0f});
			ctx.menu_bg_fade_out_animation->set_frame_callback(menu_bg_frame_callback);
			ctx.menu_bg_fade_out_animation->set_end_callback
			(
				[&ctx = this->ctx]()
				{
					ctx.ui_scene->remove_object(ctx.menu_bg_billboard);
					ctx.menu_bg_billboard->set_active(false);
				}
			);
		}
		
		ctx.animator->add_animation(ctx.menu_bg_fade_in_animation);
		ctx.animator->add_animation(ctx.menu_bg_fade_out_animation);
	}
	
	// Create camera flash animation
	ctx.camera_flash_animation = new animation<float>();
	{
		ctx.camera_flash_animation->set_interpolator(ease<float>::out_sine);
		const float duration = 0.5f;
		animation_channel<float>* channel = ctx.camera_flash_animation->add_channel(0);
		channel->insert_keyframe({0.0f, 1.0f});
		channel->insert_keyframe({duration, 0.0f});
	}
}

void boot::setup_entities()
{
	// Create entity registry
	ctx.entity_registry = new entt::registry();
}

void boot::setup_systems()
{
	event_dispatcher* event_dispatcher = ctx.app->get_event_dispatcher();

	const auto& viewport_dimensions = ctx.app->get_viewport_dimensions();
	float4 viewport = {0.0f, 0.0f, static_cast<float>(viewport_dimensions[0]), static_cast<float>(viewport_dimensions[1])};
	
	// Setup terrain system
	ctx.terrain_system = new game::system::terrain(*ctx.entity_registry);
	ctx.terrain_system->set_patch_side_length(31.0f);
	ctx.terrain_system->set_patch_subdivisions(31);
	ctx.terrain_system->set_scene_collection(ctx.surface_scene);
	
	// Setup vegetation system
	//ctx.vegetation_system = new game::system::vegetation(*ctx.entity_registry);
	//ctx.vegetation_system->set_terrain_patch_size(TERRAIN_PATCH_SIZE);
	//ctx.vegetation_system->set_vegetation_patch_resolution(VEGETATION_PATCH_RESOLUTION);
	//ctx.vegetation_system->set_vegetation_density(1.0f);
	//ctx.vegetation_system->set_vegetation_model(ctx.resource_manager->load<model>("grass-tuft.mdl"));
	//ctx.vegetation_system->set_scene(ctx.surface_scene);
	
	// Setup camera system
	ctx.camera_system = new game::system::camera(*ctx.entity_registry);
	ctx.camera_system->set_viewport(viewport);
	event_dispatcher->subscribe<window_resized_event>(ctx.camera_system);
	
	// Setup subterrain system
	ctx.subterrain_system = new game::system::subterrain(*ctx.entity_registry, ctx.resource_manager);
	ctx.subterrain_system->set_scene(ctx.underground_scene);
	
	// Setup collision system
	ctx.collision_system = new game::system::collision(*ctx.entity_registry);
	
	// Setup behavior system
	ctx.behavior_system = new game::system::behavior(*ctx.entity_registry);
	
	// Setup locomotion system
	ctx.locomotion_system = new game::system::locomotion(*ctx.entity_registry);
	
	// Setup steering system
	ctx.steering_system = new game::system::steering(*ctx.entity_registry);
	
	// Setup spring system
	ctx.spring_system = new game::system::spring(*ctx.entity_registry);
	
	// Setup spatial system
	ctx.spatial_system = new game::system::spatial(*ctx.entity_registry);
	
	// Setup constraint system
	ctx.constraint_system = new game::system::constraint(*ctx.entity_registry);
	
	// Setup painting system
	ctx.painting_system = new game::system::painting(*ctx.entity_registry, event_dispatcher, ctx.resource_manager);
	ctx.painting_system->set_scene(ctx.surface_scene);
	
	// Setup orbit system
	ctx.orbit_system = new game::system::orbit(*ctx.entity_registry);
	
	// Setup blackbody system
	ctx.blackbody_system = new game::system::blackbody(*ctx.entity_registry);
	ctx.blackbody_system->set_illuminant(color::illuminant::deg2::d55<double>);
	
	// RGB wavelengths for atmospheric scatteering
	ctx.rgb_wavelengths = {680, 550, 440};
	
	// Setup atmosphere system
	ctx.atmosphere_system = new game::system::atmosphere(*ctx.entity_registry);
	ctx.atmosphere_system->set_rgb_wavelengths(ctx.rgb_wavelengths * 1e-9);
	ctx.atmosphere_system->set_sky_pass(ctx.sky_pass);
	
	// Setup astronomy system
	ctx.astronomy_system = new game::system::astronomy(*ctx.entity_registry);
	ctx.astronomy_system->set_transmittance_samples(16);
	ctx.astronomy_system->set_sky_pass(ctx.sky_pass);
	
	// Setup proteome system
	ctx.proteome_system = new game::system::proteome(*ctx.entity_registry);
	
	// Setup render system
	ctx.render_system = new game::system::render(*ctx.entity_registry);
	//ctx.render_system->add_layer(ctx.underground_scene);
	ctx.render_system->add_layer(ctx.surface_scene);
	ctx.render_system->add_layer(ctx.ui_scene);
	ctx.render_system->set_renderer(ctx.renderer);
}

void boot::setup_controls()
{
	event_dispatcher* event_dispatcher = ctx.app->get_event_dispatcher();
	
	// Setup input event routing
	ctx.input_event_router = new input::event_router();
	ctx.input_event_router->set_event_dispatcher(event_dispatcher);
	
	// Setup input mapper
	ctx.input_mapper = new input::mapper();
	ctx.input_mapper->set_event_dispatcher(event_dispatcher);
	
	// Setup input listener
	ctx.input_listener = new input::listener();
	ctx.input_listener->set_event_dispatcher(event_dispatcher);
	
	// Load SDL game controller mappings database
	ctx.logger->push_task("Loading SDL game controller mappings from database");
	file_buffer* game_controller_db = ctx.resource_manager->load<file_buffer>("gamecontrollerdb.txt");
	if (!game_controller_db)
	{
		ctx.logger->pop_task(EXIT_FAILURE);
	}
	else
	{
		ctx.app->add_game_controller_mappings(game_controller_db->data(), game_controller_db->size());
		ctx.resource_manager->unload("gamecontrollerdb.txt");
		ctx.logger->pop_task(EXIT_SUCCESS);
	}
	
	// Load controls
	ctx.logger->push_task("Loading controls");
	try
	{
		// If a control profile is set in the config file
		if (ctx.config->contains("control_profile"))
		{
			// Load control profile
			json* profile = ctx.resource_manager->load<json>((*ctx.config)["control_profile"].get<std::string>());
			
			// Apply control profile
			if (profile)
			{
				game::apply_control_profile(ctx, *profile);
			}
		}
		
		// Calibrate gamepads
		for (input::gamepad* gamepad: ctx.app->get_gamepads())
		{
			ctx.logger->push_task("Loading calibration for gamepad " + gamepad->get_guid());
			json* calibration = game::load_gamepad_calibration(ctx, *gamepad);
			if (!calibration)
			{
				ctx.logger->pop_task(EXIT_FAILURE);
				
				ctx.logger->push_task("Generating default calibration for gamepad " + gamepad->get_guid());
				json default_calibration = game::default_gamepad_calibration();
				apply_gamepad_calibration(*gamepad, default_calibration);
				
				if (!save_gamepad_calibration(ctx, *gamepad, default_calibration))
					ctx.logger->pop_task(EXIT_FAILURE);
				else
					ctx.logger->pop_task(EXIT_SUCCESS);
			}
			else
			{
				ctx.logger->pop_task(EXIT_SUCCESS);
				apply_gamepad_calibration(*gamepad, *calibration);
			}
		}
		
		// Toggle fullscreen
		ctx.controls["toggle_fullscreen"]->set_activated_callback
		(
			[&ctx = this->ctx]()
			{
				bool fullscreen = !ctx.app->is_fullscreen();
				
				ctx.app->set_fullscreen(fullscreen);
				
				if (!fullscreen)
				{
					int2 resolution;
					resolution.x() = (*ctx.config)["windowed_resolution"][0].get<int>();
					resolution.y() = (*ctx.config)["windowed_resolution"][1].get<int>();
					
					ctx.app->resize_window(resolution.x(), resolution.y());
				}
				
				// Save display mode config
				(*ctx.config)["fullscreen"] = fullscreen;
				game::save::config(ctx);
			}
		);
		
		// Screenshot
		ctx.controls["screenshot"]->set_activated_callback(std::bind(game::graphics::save_screenshot, std::ref(ctx)));
		
		// Set activation threshold for menu navigation controls to mitigate drifting gamepad axes
		const float menu_activation_threshold = 0.1f;
		ctx.controls["menu_up"]->set_activation_threshold(menu_activation_threshold);
		ctx.controls["menu_down"]->set_activation_threshold(menu_activation_threshold);
		ctx.controls["menu_left"]->set_activation_threshold(menu_activation_threshold);
		ctx.controls["menu_right"]->set_activation_threshold(menu_activation_threshold);
	}
	catch (...)
	{
		ctx.logger->pop_task(EXIT_FAILURE);
	}
	ctx.logger->pop_task(EXIT_SUCCESS);
}

void boot::setup_ui()
{
	// Load font size config
	ctx.font_size = 1.0f;
	if (ctx.config->contains("font_size"))
		ctx.font_size = (*ctx.config)["font_size"].get<float>();
	
	// Load dyslexia font config
	ctx.dyslexia_font = false;
	if (ctx.config->contains("dyslexia_font"))
		ctx.dyslexia_font = (*ctx.config)["dyslexia_font"].get<bool>();
	
	// Load fonts
	ctx.logger->push_task("Loading fonts");
	try
	{
		game::load_fonts(ctx);
	}
	catch (...)
	{
		ctx.logger->pop_task(EXIT_FAILURE);
	}
	ctx.logger->pop_task(EXIT_SUCCESS);
	
	// Construct mouse tracker
	ctx.menu_mouse_tracker = new ui::mouse_tracker();
	ctx.app->get_event_dispatcher()->subscribe<mouse_moved_event>(ctx.menu_mouse_tracker);
	ctx.app->get_event_dispatcher()->subscribe<mouse_button_pressed_event>(ctx.menu_mouse_tracker);
	ctx.app->get_event_dispatcher()->subscribe<mouse_button_released_event>(ctx.menu_mouse_tracker);
	ctx.app->get_event_dispatcher()->subscribe<mouse_wheel_scrolled_event>(ctx.menu_mouse_tracker);
}

void boot::setup_debugging()
{
	// Setup performance sampling
	ctx.performance_sampler.set_sample_size(15);
	
	ctx.cli = new debug::cli();
	ctx.cli->register_command("echo", debug::cc::echo);
	ctx.cli->register_command("exit", std::function<std::string()>(std::bind(&debug::cc::exit, &ctx)));
	ctx.cli->register_command("scrot", std::function<std::string()>(std::bind(&debug::cc::scrot, &ctx)));
	ctx.cli->register_command("cue", std::function<std::string(float, std::string)>(std::bind(&debug::cc::cue, &ctx, std::placeholders::_1, std::placeholders::_2)));
	//std::string cmd = "cue 20 exit";
	//logger->log(cmd);
	//logger->log(cli.interpret(cmd));
}

void boot::setup_loop()
{
	// Set update frequency
	if (ctx.config->contains("update_frequency"))
	{
		ctx.loop.set_update_frequency((*ctx.config)["update_frequency"].get<double>());
	}
	
	// Set update callback
	ctx.loop.set_update_callback
	(
		[&ctx = this->ctx](double t, double dt)
		{
			// Update tweens
			ctx.sky_pass->update_tweens();
			ctx.surface_scene->update_tweens();
			ctx.underground_scene->update_tweens();
			ctx.ui_scene->update_tweens();
			
			// Process events
			ctx.app->process_events();
			ctx.app->get_event_dispatcher()->update(t);
			
			// Update controls
			for (const auto& control: ctx.controls)
				control.second->update();
			
			// Process function queue
			while (!ctx.function_queue.empty())
			{
				ctx.function_queue.front()();
				ctx.function_queue.pop();
			}
			
			// Update processes
			std::for_each
			(
				std::execution::par,
				ctx.processes.begin(),
				ctx.processes.end(),
				[t, dt](const auto& process)
				{
					process.second(t, dt);
				}
			);
			
			// Advance timeline
			ctx.timeline->advance(dt);
			
			// Update entity systems
			ctx.terrain_system->update(t, dt);
			//ctx.vegetation_system->update(t, dt);
			ctx.subterrain_system->update(t, dt);
			ctx.collision_system->update(t, dt);
			ctx.behavior_system->update(t, dt);
			ctx.steering_system->update(t, dt);
			ctx.locomotion_system->update(t, dt);
			ctx.camera_system->update(t, dt);
			ctx.orbit_system->update(t, dt);
			ctx.blackbody_system->update(t, dt);
			ctx.atmosphere_system->update(t, dt);
			ctx.astronomy_system->update(t, dt);
			ctx.spring_system->update(t, dt);
			ctx.spatial_system->update(t, dt);
			ctx.constraint_system->update(t, dt);
			ctx.painting_system->update(t, dt);
			ctx.proteome_system->update(t, dt);
			ctx.animator->animate(dt);
			ctx.render_system->update(t, dt);
		}
	);
	
	// Set render callback
	ctx.loop.set_render_callback
	(
		[&ctx = this->ctx](double alpha)
		{
			ctx.render_system->draw(alpha);
			ctx.app->swap_buffers();
		}
	);
}

void boot::loop()
{
	while (!ctx.app->was_closed())
	{
		// Execute main loop
		ctx.loop.tick();
		
		// Sample frame duration
		ctx.performance_sampler.sample(ctx.loop.get_frame_duration());
	}
	
	// Exit all active game states
	while (!ctx.state_machine.empty())
		ctx.state_machine.pop();
}

} // namespace state
} // namespace game
