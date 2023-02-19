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

#include "animation/animation.hpp"
#include "animation/animator.hpp"
#include "animation/ease.hpp"
#include "animation/screen-transition.hpp"
#include "animation/timeline.hpp"
#include "color/color.hpp"
#include "config.hpp"
#include "debug/cli.hpp"
#include "debug/log.hpp"
#include "entity/commands.hpp"
#include "game/context.hpp"
#include "game/controls.hpp"
#include "game/control-profile.hpp"
#include "game/fonts.hpp"
#include "game/graphics.hpp"
#include "game/menu.hpp"
#include "game/strings.hpp"
#include "game/state/boot.hpp"
#include "game/state/splash.hpp"
#include "game/state/main-menu.hpp"
#include "game/system/astronomy.hpp"
#include "game/system/atmosphere.hpp"
#include "game/system/behavior.hpp"
#include "game/system/blackbody.hpp"
#include "game/system/camera.hpp"
#include "game/system/collision.hpp"
#include "game/system/constraint.hpp"
#include "game/system/locomotion.hpp"
#include "game/system/orbit.hpp"
#include "game/system/render.hpp"
#include "game/system/spatial.hpp"
#include "game/system/spring.hpp"
#include "game/system/steering.hpp"
#include "game/system/subterrain.hpp"
#include "game/system/terrain.hpp"
#include "game/system/vegetation.hpp"
#include "game/settings.hpp"
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
#include "input/gamepad.hpp"
#include "input/keyboard.hpp"
#include "input/mapper.hpp"
#include "input/mouse.hpp"
#include "input/scancode.hpp"
#include "render/compositor.hpp"
#include "render/material-flags.hpp"
#include "render/material-property.hpp"
#include "render/passes/bloom-pass.hpp"
#include "render/passes/clear-pass.hpp"
#include "render/passes/final-pass.hpp"
#include "render/passes/fxaa-pass.hpp"
#include "render/passes/ground-pass.hpp"
#include "render/passes/material-pass.hpp"
#include "render/passes/outline-pass.hpp"
#include "render/passes/resample-pass.hpp"
#include "render/passes/shadow-map-pass.hpp"
#include "render/passes/sky-pass.hpp"
#include "render/renderer.hpp"
#include "render/vertex-attribute.hpp"
#include "resources/file-buffer.hpp"
#include "resources/resource-manager.hpp"
#include "scene/scene.hpp"
#include "utility/paths.hpp"
#include "utility/dict.hpp"
#include "utility/hash/fnv1a.hpp"
#include "input/application-events.hpp"
#include <algorithm>
#include <cctype>
#include <entt/entt.hpp>
#include <execution>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

// Prevent cxxopts from using RTTI
#define CXXOPTS_NO_RTTI
#include <cxxopts.hpp>

using namespace hash::literals;

namespace game {
namespace state {

boot::boot(game::context& ctx, int argc, const char* const* argv):
	game::state::base(ctx)
{
	// Boot process
	debug::log::trace("Booting up...");
	
	parse_options(argc, argv);
	setup_resources();
	load_settings();
	setup_window();
	setup_input();
	load_strings();
	setup_rendering();
	setup_audio();
	setup_scenes();
	setup_animation();
	setup_entities();
	setup_systems();
	setup_controls();
	setup_ui();
	setup_debugging();
	setup_loop();
	
	ctx.active_ecoregion = nullptr;
	
	debug::log::trace("Boot up complete");
	
	// Push next state
	ctx.state_machine.emplace(new game::state::main_menu(ctx, true));
	
	// Enter main loop
	debug::log::trace("Entered main loop");
	loop();
}

boot::~boot()
{
	debug::log::trace("Booting down...");
	
	// Update window settings
	const auto& windowed_position = ctx.window->get_windowed_position();
	const auto& windowed_size = ctx.window->get_windowed_size();
	const bool maximized = ctx.window->is_maximized();
	const bool fullscreen = ctx.window->is_fullscreen();
	(*ctx.settings)["window_x"_fnv1a32] = windowed_position.x();
	(*ctx.settings)["window_y"_fnv1a32] = windowed_position.y();
	(*ctx.settings)["window_w"_fnv1a32] = windowed_size.x();
	(*ctx.settings)["window_h"_fnv1a32] = windowed_size.y();
	(*ctx.settings)["maximized"_fnv1a32] = maximized;
	(*ctx.settings)["fullscreen"_fnv1a32] = fullscreen;
	
	// Destruct window
	delete ctx.window;
	
	// Save settings
	ctx.resource_manager->set_write_dir(ctx.shared_config_path);
	ctx.resource_manager->save(ctx.settings, "settings.cfg");
	
	// Destruct input and window managers
	delete ctx.input_manager;
	delete ctx.window_manager;
	
	// Shut down audio
	shutdown_audio();
	
	debug::log::trace("Boot down complete");
}

void boot::parse_options(int argc, const char* const* argv)
{
	if (argc <= 1)
	{
		// No command-line options specified
		return;
	}
	
	debug::log::trace("Parsing command-line options...");
	
	// Parse command-line options with cxxopts
	try
	{
		cxxopts::Options options(config::application_name, config::application_name);
		options.add_options()
			("c,continue", "Continues from the last save")
			("d,data", "Sets the data package path", cxxopts::value<std::string>())
			("f,fullscreen", "Starts in fullscreen mode")
			("n,new-game", "Starts a new game")
			("q,quick-start", "Skips to the main menu")
			("r,reset", "Resets all settings to default")
			("v,v-sync", "Enables or disables v-sync", cxxopts::value<int>())
			("w,windowed", "Starts in windowed mode");
		auto result = options.parse(argc, argv);
		
		// --continue
		if (result.count("continue"))
		{
			ctx.option_continue = true;
		}
		
		// --data
		if (result.count("data"))
		{
			ctx.option_data = result["data"].as<std::string>();
		}
		
		// --fullscreen
		if (result.count("fullscreen"))
		{
			ctx.option_fullscreen = true;
		}
		
		// --new-game
		if (result.count("new-game"))
		{
			ctx.option_new_game = true;
		}
		
		// --quick-start
		if (result.count("quick-start"))
		{
			ctx.option_quick_start = true;
		}
		
		// --reset
		if (result.count("reset"))
		{
			ctx.option_reset = true;
		}
		
		// --v-sync
		if (result.count("v-sync"))
		{
			ctx.option_v_sync = result["v-sync"].as<int>();
		}
		
		// --windowed
		if (result.count("windowed"))
		{
			ctx.option_windowed = true;
		}
		
		debug::log::info("Parsed {} command-line options", argc);
	}
	catch (const std::exception& e)
	{
		debug::log::error("An error occurred while parsing command-line options: {}", e.what());
	}
}

void boot::setup_resources()
{
	// Allocate resource manager
	ctx.resource_manager = new resource_manager();
	
	// Get executable data path
	const auto data_path = get_executable_data_path();
	
	// Determine data package path
	if (ctx.option_data)
	{
		// Handle command-line data path option
		ctx.data_package_path = ctx.option_data.value();
		if (ctx.data_package_path.is_relative())
		{
			ctx.data_package_path = data_path / ctx.data_package_path;
		}
	}
	else
	{
		ctx.data_package_path = data_path / (config::application_slug + std::string("-data.zip"));
	}
	
	// Determine mods path
	ctx.mods_path = data_path / "mods";
	
	// Determine config paths
	ctx.local_config_path = get_local_config_path() / config::application_name;
	ctx.shared_config_path = get_shared_config_path() / config::application_name;
	ctx.saves_path = ctx.shared_config_path / "saves";
	ctx.screenshots_path = ctx.shared_config_path / "gallery";
	ctx.controls_path = ctx.shared_config_path / "controls";
	
	// Log paths
	debug::log::info("Data package path: \"{}\"", ctx.data_package_path.string());
	debug::log::info("Local config path: \"{}\"", ctx.local_config_path.string());
	debug::log::info("Shared config path: \"{}\"", ctx.shared_config_path.string());
	debug::log::info("Mods path: \"{}\"", ctx.mods_path.string());
	
	// Create nonexistent config directories
	std::vector<std::filesystem::path> config_paths;
	config_paths.push_back(ctx.local_config_path);
	config_paths.push_back(ctx.shared_config_path);
	config_paths.push_back(ctx.saves_path);
	config_paths.push_back(ctx.screenshots_path);
	config_paths.push_back(ctx.controls_path);
	for (const auto& path: config_paths)
	{
		try
		{
			if (std::filesystem::create_directories(path))
			{
				debug::log::info("Created directory \"{}\"", path.string());
			}
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			debug::log::error("Failed to create directory \"{}\": {}", path.string(), e.what());
		}
	}
	
	// Scan for mods
	std::vector<std::filesystem::path> mod_paths;
	if (std::filesystem::is_directory(ctx.mods_path))
	{
		for (const auto& entry: std::filesystem::directory_iterator{ctx.mods_path})
		{
			if (entry.is_directory() || (entry.is_regular_file() && entry.path().extension() == ".zip"))
			{
				mod_paths.push_back(entry.path());
				debug::log::info("Found mod \"{}\"", entry.path().filename().string());
			}
		}
	}
	
	// Mount mod paths
	for (const std::filesystem::path& mod_path: mod_paths)
	{
		ctx.resource_manager->mount(ctx.mods_path / mod_path);
	}
	
	// Mount config path
	ctx.resource_manager->mount(ctx.local_config_path);
	ctx.resource_manager->mount(ctx.shared_config_path);
	
	// Mount data package path
	ctx.resource_manager->mount(ctx.data_package_path);
	
	// Include resource search paths in order of priority
	ctx.resource_manager->include("/controls/");
	ctx.resource_manager->include("/");
}

void boot::load_settings()
{
	if (ctx.option_reset)
	{
		// Command-line reset option found, reset settings
		ctx.settings = new dict<std::uint32_t>();
		ctx.resource_manager->set_write_dir(ctx.shared_config_path);
		ctx.resource_manager->save(ctx.settings, "settings.cfg");
		debug::log::info("Settings reset");
	}
	else
	{
		ctx.settings = ctx.resource_manager->load<dict<std::uint32_t>>("settings.cfg");
		if (!ctx.settings)
		{
			debug::log::info("Settings not found");
			ctx.settings = new dict<std::uint32_t>();
		}
	}
}

void boot::setup_window()
{
	// Construct window manager
	ctx.window_manager = app::window_manager::instance();
	
	// Default window settings
	std::string window_title = config::application_name;
	int window_x = -1;
	int window_y = -1;
	int window_w = -1;
	int window_h = -1;
	bool maximized = true;
	bool fullscreen = true;
	bool v_sync = true;
	
	// Read window settings
	bool resize = false;
	read_or_write_setting(ctx, "window_title"_fnv1a32, window_title);
	read_or_write_setting(ctx, "window_x"_fnv1a32, window_x);
	read_or_write_setting(ctx, "window_y"_fnv1a32, window_y);
	if (!read_or_write_setting(ctx, "window_w"_fnv1a32, window_w) ||
		!read_or_write_setting(ctx, "window_h"_fnv1a32, window_h))
	{
		resize = true;
	}
	read_or_write_setting(ctx, "maximized"_fnv1a32, maximized);
	read_or_write_setting(ctx, "fullscreen"_fnv1a32, fullscreen);
	read_or_write_setting(ctx, "v_sync"_fnv1a32, v_sync);
	
	// If window size not set, resize and reposition relative to default display
	if (resize)
	{
		const app::display& display = ctx.window_manager->get_display(0);
		const auto& usable_bounds = display.get_usable_bounds();
		const auto usable_bounds_center = usable_bounds.center();
		
		const float default_windowed_scale = 1.0f / 1.2f;
		
		window_w = static_cast<int>((usable_bounds.max.x() - usable_bounds.min.x()) * default_windowed_scale);
		window_h = static_cast<int>((usable_bounds.max.y() - usable_bounds.min.y()) * default_windowed_scale);
		window_x = usable_bounds_center.x() - window_w / 2;
		window_y = usable_bounds_center.y() - window_h / 2;
	}
	
	// Handle window-related command-line options
	if (ctx.option_windowed)
	{
		// Start in windowed mode
		maximized = false;
		fullscreen = false;
	}
	if (ctx.option_fullscreen)
	{
		// Start in fullscreen mode
		fullscreen = true;
	}
	if (ctx.option_v_sync)
	{
		v_sync = ctx.option_v_sync.value();
	}
	
	// Construct window
	ctx.window = ctx.window_manager->create_window
	(
		window_title,
		{window_x, window_y},
		{window_w, window_h},
		maximized,
		fullscreen,
		v_sync
	);
	
	// Restrict window size
	ctx.window->set_minimum_size({160, 144});
	
	// Setup window closed callback
	ctx.window_closed_subscription = ctx.window->get_closed_channel().subscribe
	(
		[&](const auto& event)
		{
			ctx.closed = true;
		}
	);
}

void boot::setup_input()
{
	// Construct input manager
	ctx.input_manager = app::input_manager::instance();
	
	// Process initial input events, such as connecting gamepads
	ctx.input_manager->update();
	
	// Setup application quit callback
	ctx.application_quit_subscription = ctx.input_manager->get_event_queue().subscribe<input::application_quit_event>
	(
		[&](const auto& event)
		{
			ctx.closed = true;
		}
	);
	
	// Gamepad deactivation function
	auto deactivate_gamepad = [&ctx = this->ctx](const auto& event)
	{
		if (ctx.gamepad_active)
		{
			ctx.gamepad_active = false;
			ctx.input_manager->show_cursor();
		}
	};
	
	// Setup gamepad activation callbacks
	ctx.gamepad_axis_moved_subscription = ctx.input_manager->get_event_queue().subscribe<input::gamepad_axis_moved_event>
	(
		[&](const auto& event)
		{
			if (!ctx.gamepad_active && std::abs(event.position) > 0.5f)
			{
				ctx.gamepad_active = true;
				ctx.input_manager->hide_cursor();
			}
		}
	);
	ctx.gamepad_button_pressed_subscription = ctx.input_manager->get_event_queue().subscribe<input::gamepad_button_pressed_event>
	(
		[&ctx = this->ctx](const auto& event)
		{
			if (!ctx.gamepad_active)
			{
				ctx.gamepad_active = true;
				ctx.input_manager->hide_cursor();
			}
		}
	);
	
	// Setup gamepad deactivation callbacks
	ctx.mouse_button_pressed_subscription = ctx.input_manager->get_event_queue().subscribe<input::mouse_button_pressed_event>
	(
		deactivate_gamepad
	);
	ctx.mouse_moved_subscription = ctx.input_manager->get_event_queue().subscribe<input::mouse_moved_event>
	(
		deactivate_gamepad
	);
	ctx.mouse_scrolled_subscription = ctx.input_manager->get_event_queue().subscribe<input::mouse_scrolled_event>
	(
		deactivate_gamepad
	);
	
	// Activate gamepad if one is connected
	if (!ctx.input_manager->get_gamepads().empty())
	{
		ctx.gamepad_active = true;
		ctx.input_manager->hide_cursor();
	}
	else
	{
		ctx.gamepad_active = false;
	}
}

void boot::load_strings()
{
	debug::log::trace("Loading strings...");
	
	// Default strings settings
	ctx.language_tag = "en";
	
	// Read strings settings
	read_or_write_setting(ctx, "language_tag"_fnv1a32, ctx.language_tag);
	
	// Slugify language tag
	std::string language_slug = ctx.language_tag;
	std::transform
	(
		language_slug.begin(),
		language_slug.end(),
		language_slug.begin(),
		[](unsigned char c)
		{
			return std::tolower(c);
		}
	);
	
	// Load string map
	ctx.string_map = ctx.resource_manager->load<i18n::string_map>(language_slug + ".str");
	
	// Log language info
	debug::log::info("Language tag: {}", ctx.language_tag);
	
	// Change window title
	const std::string window_title = get_string(ctx, "window_title"_fnv1a32);
	ctx.window->set_title(window_title);
	
	// Update window title setting
	(*ctx.settings)["window_title"_fnv1a32] = window_title;
	
	debug::log::trace("Loaded strings");
}

void boot::setup_rendering()
{
	debug::log::trace("Setting up rendering...");
	
	// Default rendering settings
	ctx.render_scale = 1.0f;
	ctx.anti_aliasing_method = render::anti_aliasing_method::fxaa;
	ctx.shadow_map_resolution = 4096;
	
	// Read rendering settings
	read_or_write_setting(ctx, "render_scale"_fnv1a32, ctx.render_scale);
	read_or_write_setting(ctx, "anti_aliasing_method"_fnv1a32, *reinterpret_cast<std::underlying_type_t<render::anti_aliasing_method>*>(&ctx.anti_aliasing_method));
	read_or_write_setting(ctx, "shadow_map_resolution"_fnv1a32, ctx.shadow_map_resolution);
	
	// Create framebuffers
	game::graphics::create_framebuffers(ctx);
	
	// Load blue noise texture
	gl::texture_2d* blue_noise_map = ctx.resource_manager->load<gl::texture_2d>("blue-noise.tex");
	
	// Load fallback material
	ctx.fallback_material = ctx.resource_manager->load<render::material>("fallback.mtl");
	
	// Setup common render passes
	{
		// Construct bloom pass
		ctx.bloom_pass = new render::bloom_pass(ctx.window->get_rasterizer(), ctx.resource_manager);
		ctx.bloom_pass->set_source_texture(ctx.hdr_color_texture);
		ctx.bloom_pass->set_mip_chain_length(0);
		ctx.bloom_pass->set_filter_radius(0.005f);
		
		ctx.common_final_pass = new render::final_pass(ctx.window->get_rasterizer(), ctx.ldr_framebuffer_a, ctx.resource_manager);
		ctx.common_final_pass->set_color_texture(ctx.hdr_color_texture);
		ctx.common_final_pass->set_bloom_texture(ctx.bloom_pass->get_bloom_texture());
		ctx.common_final_pass->set_bloom_weight(0.04f);
		ctx.common_final_pass->set_blue_noise_texture(blue_noise_map);
		
		ctx.fxaa_pass = new render::fxaa_pass(ctx.window->get_rasterizer(), &ctx.window->get_rasterizer()->get_default_framebuffer(), ctx.resource_manager);
		ctx.fxaa_pass->set_source_texture(ctx.ldr_color_texture_a);
		
		ctx.resample_pass = new render::resample_pass(ctx.window->get_rasterizer(), &ctx.window->get_rasterizer()->get_default_framebuffer(), ctx.resource_manager);
		ctx.resample_pass->set_source_texture(ctx.ldr_color_texture_b);
		ctx.resample_pass->set_enabled(false);
		
		// Configure anti-aliasing according to settings
		graphics::select_anti_aliasing_method(ctx, ctx.anti_aliasing_method);
		
		// Configure render scaling according to settings
		graphics::change_render_resolution(ctx, ctx.render_scale);
	}
	
	// Setup UI compositor
	{
		ctx.ui_clear_pass = new render::clear_pass(ctx.window->get_rasterizer(), &ctx.window->get_rasterizer()->get_default_framebuffer());
		ctx.ui_clear_pass->set_cleared_buffers(false, true, false);
		ctx.ui_clear_pass->set_clear_depth(-1.0f);
		
		ctx.ui_material_pass = new render::material_pass(ctx.window->get_rasterizer(), &ctx.window->get_rasterizer()->get_default_framebuffer(), ctx.resource_manager);
		ctx.ui_material_pass->set_fallback_material(ctx.fallback_material);
		
		ctx.ui_compositor = new render::compositor();
		ctx.ui_compositor->add_pass(ctx.ui_clear_pass);
		ctx.ui_compositor->add_pass(ctx.ui_material_pass);
	}
	
	// Setup underground compositor
	{
		ctx.underground_clear_pass = new render::clear_pass(ctx.window->get_rasterizer(), ctx.hdr_framebuffer);
		ctx.underground_clear_pass->set_cleared_buffers(true, true, false);
		ctx.underground_clear_pass->set_clear_color({1, 0, 1, 0});
		ctx.underground_clear_pass->set_clear_depth(-1.0f);
		
		ctx.underground_material_pass = new render::material_pass(ctx.window->get_rasterizer(), ctx.hdr_framebuffer, ctx.resource_manager);
		ctx.underground_material_pass->set_fallback_material(ctx.fallback_material);
		
		ctx.underground_compositor = new render::compositor();
		ctx.underground_compositor->add_pass(ctx.underground_clear_pass);
		ctx.underground_compositor->add_pass(ctx.underground_material_pass);
		ctx.underground_compositor->add_pass(ctx.bloom_pass);
		ctx.underground_compositor->add_pass(ctx.common_final_pass);
		ctx.underground_compositor->add_pass(ctx.fxaa_pass);
		ctx.underground_compositor->add_pass(ctx.resample_pass);
	}
	
	// Setup surface compositor
	{
		ctx.surface_shadow_map_clear_pass = new render::clear_pass(ctx.window->get_rasterizer(), ctx.shadow_map_framebuffer);
		ctx.surface_shadow_map_clear_pass->set_cleared_buffers(false, true, false);
		ctx.surface_shadow_map_clear_pass->set_clear_depth(1.0f);
		
		ctx.surface_shadow_map_pass = new render::shadow_map_pass(ctx.window->get_rasterizer(), ctx.resource_manager);
		
		ctx.surface_clear_pass = new render::clear_pass(ctx.window->get_rasterizer(), ctx.hdr_framebuffer);
		ctx.surface_clear_pass->set_cleared_buffers(false, true, true);
		ctx.surface_clear_pass->set_clear_depth(-1.0f);
		
		ctx.sky_pass = new render::sky_pass(ctx.window->get_rasterizer(), ctx.hdr_framebuffer, ctx.resource_manager);
		ctx.sky_pass->set_enabled(false);
		ctx.sky_pass->set_magnification(3.0f);
		
		ctx.ground_pass = new render::ground_pass(ctx.window->get_rasterizer(), ctx.hdr_framebuffer, ctx.resource_manager);
		ctx.ground_pass->set_enabled(false);
		
		ctx.surface_material_pass = new render::material_pass(ctx.window->get_rasterizer(), ctx.hdr_framebuffer, ctx.resource_manager);
		ctx.surface_material_pass->set_fallback_material(ctx.fallback_material);
		
		ctx.surface_outline_pass = new render::outline_pass(ctx.window->get_rasterizer(), ctx.hdr_framebuffer, ctx.resource_manager);
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
		ctx.surface_compositor->add_pass(ctx.bloom_pass);
		ctx.surface_compositor->add_pass(ctx.common_final_pass);
		ctx.surface_compositor->add_pass(ctx.fxaa_pass);
		ctx.surface_compositor->add_pass(ctx.resample_pass);
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
	
	debug::log::trace("Set up rendering");
}

void boot::setup_audio()
{
	debug::log::trace("Setting up audio...");
	
	// Default audio settings
	ctx.master_volume = 1.0f;
	ctx.ambience_volume = 1.0f;
	ctx.effects_volume = 1.0f;
	ctx.mono_audio = false;
	ctx.captions = false;
	ctx.captions_size = 1.0f;
	
	// Read audio settings
	read_or_write_setting(ctx, "master_volume"_fnv1a32, ctx.master_volume);
	read_or_write_setting(ctx, "ambience_volume"_fnv1a32, ctx.ambience_volume);
	read_or_write_setting(ctx, "effects_volume"_fnv1a32, ctx.effects_volume);
	read_or_write_setting(ctx, "mono_audio"_fnv1a32, ctx.mono_audio);
	read_or_write_setting(ctx, "captions"_fnv1a32, ctx.captions);
	read_or_write_setting(ctx, "captions_size"_fnv1a32, ctx.captions_size);
	
	// Open audio device
	debug::log::trace("Opening audio device...");
	ctx.alc_device = alcOpenDevice(nullptr);
	if (!ctx.alc_device)
	{
		debug::log::error("Failed to open audio device: AL error code {}", alGetError());
		return;
	}
	else
	{
		// Get audio device name
		const ALCchar* alc_device_name = nullptr;
		if (alcIsExtensionPresent(ctx.alc_device, "ALC_ENUMERATE_ALL_EXT"))
		{
			alc_device_name = alcGetString(ctx.alc_device, ALC_ALL_DEVICES_SPECIFIER);
		}
		if (alcGetError(ctx.alc_device) != AL_NO_ERROR || !alc_device_name)
		{
			alc_device_name = alcGetString(ctx.alc_device, ALC_DEVICE_SPECIFIER);
		}
		
		// Log audio device name
		debug::log::info("Opened audio device \"{}\"", alc_device_name);
	}
	
	// Create audio context
	debug::log::trace("Creating audio context...");
	ctx.alc_context = alcCreateContext(ctx.alc_device, nullptr);
	if (!ctx.alc_context)
	{
		debug::log::error("Failed to create audio context: ALC error code {}", alcGetError(ctx.alc_device));
		alcCloseDevice(ctx.alc_device);
		return;
	}
	else
	{
		debug::log::trace("Created audio context");
	}
	
	// Make audio context current
	debug::log::trace("Making audio context current...");
	if (alcMakeContextCurrent(ctx.alc_context) == ALC_FALSE)
	{
		debug::log::error("Failed to make audio context current: ALC error code {}", alcGetError(ctx.alc_device));
		if (ctx.alc_context != nullptr)
		{
			alcDestroyContext(ctx.alc_context);
		}
		alcCloseDevice(ctx.alc_device);
		return;
	}
	else
	{
		debug::log::trace("Made audio context current");
	}
	
	debug::log::trace("Set up audio");
}

void boot::setup_scenes()
{
	debug::log::trace("Setting up scenes...");
	
	// Get default framebuffer
	const auto& viewport_size = ctx.window->get_viewport_size();
	const float viewport_aspect_ratio = static_cast<float>(viewport_size[0]) / static_cast<float>(viewport_size[1]);
	
	// Setup UI camera
	ctx.ui_camera = new scene::camera();
	ctx.ui_camera->set_compositor(ctx.ui_compositor);
	float clip_left = 0.0f;
	float clip_right = viewport_size[0];
	float clip_top = 0.0f;
	float clip_bottom = viewport_size[1];
	float clip_near = -100.0f;
	float clip_far = 100.0f;
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
	ctx.surface_camera->set_perspective(math::radians<float>(45.0f), viewport_aspect_ratio, 0.1f, 5000.0f);
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
		menu_bg_material->set_blend_mode(render::blend_mode::translucent);
		menu_bg_material->update_tweens();
		ctx.menu_bg_billboard = new scene::billboard();
		ctx.menu_bg_billboard->set_active(false);
		ctx.menu_bg_billboard->set_material(menu_bg_material);
		ctx.menu_bg_billboard->set_scale({(float)viewport_size[0] * 0.5f, (float)viewport_size[1] * 0.5f, 1.0f});
		ctx.menu_bg_billboard->set_translation({0.0f, 0.0f, -100.0f});
		ctx.menu_bg_billboard->update_tweens();
		
		// Create camera flash billboard
		render::material* flash_material = new render::material();
		flash_material->set_shader_program(ctx.resource_manager->load<gl::shader_program>("ui-element-untextured.glsl"));
		auto flash_tint = flash_material->add_property<float4>("tint");
		flash_tint->set_value(float4{1, 1, 1, 1});
		//flash_tint->set_tween_interpolator(ease<float4>::out_quad);
		
		flash_material->set_blend_mode(render::blend_mode::translucent);
		flash_material->update_tweens();
		
		ctx.camera_flash_billboard = new scene::billboard();
		ctx.camera_flash_billboard->set_material(flash_material);
		ctx.camera_flash_billboard->set_scale({(float)viewport_size[0] * 0.5f, (float)viewport_size[1] * 0.5f, 1.0f});
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
	
	debug::log::trace("Set up scenes");
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
	ctx.fade_transition->get_billboard()->set_translation({0, 0, 98});
	ctx.fade_transition->get_billboard()->update_tweens();
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
	const auto& viewport_size = ctx.window->get_viewport_size();
	float4 viewport = {0.0f, 0.0f, static_cast<float>(viewport_size[0]), static_cast<float>(viewport_size[1])};
	
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
	
	// Setup render system
	ctx.render_system = new game::system::render(*ctx.entity_registry);
	//ctx.render_system->add_layer(ctx.underground_scene);
	ctx.render_system->add_layer(ctx.surface_scene);
	ctx.render_system->add_layer(ctx.ui_scene);
	ctx.render_system->set_renderer(ctx.renderer);
}

void boot::setup_controls()
{
	debug::log::trace("Setting up controls...");
	
	// Load SDL game controller mappings database
	// debug::log::trace("Loading SDL game controller mappings...");
	// file_buffer* game_controller_db = ctx.resource_manager->load<file_buffer>("gamecontrollerdb.txt");
	// if (!game_controller_db)
	// {
		// debug::log::error("Failed to load SDL game controller mappings");
	// }
	// else
	// {
		// ctx.app->add_game_controller_mappings(game_controller_db->data(), game_controller_db->size());
		// debug::log::trace("Loaded SDL game controller mappings");
		
		// ctx.resource_manager->unload("gamecontrollerdb.txt");
	// }
	
	// Default control profile settings
	ctx.control_profile_filename = "controls.cfg";
	ctx.control_profile = nullptr;
	
	// Read control profile settings
	if (read_or_write_setting(ctx, "control_profile"_fnv1a32, ctx.control_profile_filename))
	{
		// Load control profile
		//ctx.control_profile = ctx.resource_manager->load<game::control_profile>(ctx.controls_path / ctx.control_profile_filename);
		ctx.control_profile = ctx.resource_manager->load<game::control_profile>(ctx.control_profile_filename);
	}
	
	if (!ctx.control_profile)
	{
		// Allocate control profile
		ctx.control_profile = new game::control_profile();
		
		// Reset control profile to default settings.
		game::reset_control_profile(*ctx.control_profile);
		
		// Save control profile
		ctx.resource_manager->set_write_dir(ctx.controls_path);
		ctx.resource_manager->save(ctx.control_profile, ctx.control_profile_filename);
	}
	
	// Apply control profile
	game::apply_control_profile(ctx, *ctx.control_profile);
	
	// Setup action callbacks
	setup_window_controls(ctx);
	setup_menu_controls(ctx);
	setup_game_controls(ctx);
	
	// Enable window controls
	enable_window_controls(ctx);
	
	debug::log::trace("Set up controls");
}

void boot::setup_ui()
{
	// Default UI settings
	ctx.font_scale = 1.0f;
	ctx.debug_font_size_pt = 10.0f;
	ctx.menu_font_size_pt = 22.0f;
	ctx.title_font_size_pt = 80.0f;
	ctx.dyslexia_font = false;
	
	// Read UI settings
	read_or_write_setting(ctx, "font_scale"_fnv1a32, ctx.font_scale);
	read_or_write_setting(ctx, "debug_font_size_pt"_fnv1a32, ctx.debug_font_size_pt);
	read_or_write_setting(ctx, "menu_font_size_pt"_fnv1a32, ctx.menu_font_size_pt);
	read_or_write_setting(ctx, "title_font_size_pt"_fnv1a32, ctx.title_font_size_pt);
	read_or_write_setting(ctx, "dyslexia_font"_fnv1a32, ctx.dyslexia_font);
	
	// Load fonts
	debug::log::trace("Loading fonts...");
	try
	{
		game::load_fonts(ctx);
		debug::log::trace("Loaded fonts");
	}
	catch (...)
	{
		debug::log::error("Failed to load fonts");
	}
	
	// Setup window resized callback
	ctx.window_resized_subscription = ctx.window->get_resized_channel().subscribe
	(
		[&](const auto& event)
		{
			const auto& viewport_size = event.window->get_viewport_size();
			const float viewport_aspect_ratio = static_cast<float>(viewport_size.x()) / static_cast<float>(viewport_size.y());
			
			// Resize framebuffers
			game::graphics::change_render_resolution(ctx, ctx.render_scale);
			
			// Update camera projection matrix
			ctx.surface_camera->set_perspective
			(
				ctx.surface_camera->get_fov(),
				viewport_aspect_ratio,
				ctx.surface_camera->get_clip_near(),
				ctx.surface_camera->get_clip_far()
			);
			
			// Update UI camera projection matrix
			ctx.ui_camera->set_orthographic
			(
				0.0f,
				viewport_size.x(),
				0.0f,
				viewport_size.y(),
				ctx.ui_camera->get_clip_near(),
				ctx.ui_camera->get_clip_far()
			);
			
			// Re-align debug text
			ctx.frame_time_text->set_translation({std::round(0.0f), std::round(viewport_size.y() - ctx.debug_font.get_font_metrics().size), 99.0f});
			ctx.frame_time_text->update_tweens();
			
			// Re-align menu text
			game::menu::align_text(ctx);
		}
	);
}

void boot::setup_debugging()
{
	ctx.cli = new debug::cli();
	//debug::log::info(ctx.cli->interpret("echo hi 123"));
	
	const auto& viewport_size = ctx.window->get_viewport_size();
	
	ctx.frame_time_text = new scene::text();
	ctx.frame_time_text->set_material(&ctx.debug_font_material);
	ctx.frame_time_text->set_color({1.0f, 1.0f, 0.0f, 1.0f});
	ctx.frame_time_text->set_font(&ctx.debug_font);
	ctx.frame_time_text->set_translation({std::round(0.0f), std::round(viewport_size.y() - ctx.debug_font.get_font_metrics().size), 99.0f});
	ctx.frame_time_text->update_tweens();
	
	ctx.ui_scene->add_object(ctx.frame_time_text);
}

void boot::setup_loop()
{
	// Default loop settings
	double update_frequency = 60.0;
	
	// Read loop settings
	read_or_write_setting(ctx, "update_frequency"_fnv1a32, update_frequency);
	
	// Set update frequency
	ctx.loop.set_update_frequency(update_frequency);
	
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
			ctx.window_manager->update();
			ctx.input_manager->update();
			
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
			ctx.window->swap_buffers();
		}
	);
}

void boot::loop()
{
	ctx.closed = false;
	math::moving_average<float, 30> average_frame_time;
	
	while (!ctx.closed)
	{
		// Execute main loop
		ctx.loop.tick();
		
		// Sample frame duration
		average_frame_time(static_cast<float>(ctx.loop.get_frame_duration() * 1000.0));
		
		ctx.frame_time_text->set_content(std::format("◷{:5.02f}", average_frame_time.average()));
	}
	
	// Exit all active game states
	while (!ctx.state_machine.empty())
	{
		ctx.state_machine.pop();
	}
}

void boot::shutdown_audio()
{
	debug::log::trace("Shutting down audio...");
	
	if (ctx.alc_context)
	{
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(ctx.alc_context);
	}
	
	if (ctx.alc_device)
	{
		alcCloseDevice(ctx.alc_device);
	}
	
	debug::log::trace("Shut down audio");
}

} // namespace state
} // namespace game
