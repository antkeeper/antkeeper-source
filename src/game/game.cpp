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

#include "game/game.hpp"
#include "game/commands/commands.hpp"
#include "game/control-profile.hpp"
#include "game/controls.hpp"
#include "game/fonts.hpp"
#include "game/graphics.hpp"
#include "game/menu.hpp"
#include "game/settings.hpp"
#include "game/states/main-menu-state.hpp"
#include "game/states/splash-state.hpp"
#include "game/strings.hpp"
#include "game/systems/astronomy-system.hpp"
#include "game/systems/atmosphere-system.hpp"
#include "game/systems/behavior-system.hpp"
#include "game/systems/blackbody-system.hpp"
#include "game/systems/camera-system.hpp"
#include "game/systems/collision-system.hpp"
#include "game/systems/constraint-system.hpp"
#include "game/systems/locomotion-system.hpp"
#include "game/systems/orbit-system.hpp"
#include "game/systems/render-system.hpp"
#include "game/systems/spatial-system.hpp"
#include "game/systems/spring-system.hpp"
#include "game/systems/steering-system.hpp"
#include "game/systems/physics-system.hpp"
#include "game/systems/subterrain-system.hpp"
#include "game/systems/terrain-system.hpp"
#include <algorithm>
#include <cctype>
#include <engine/animation/animation.hpp>
#include <engine/animation/animator.hpp>
#include <engine/animation/ease.hpp>
#include <engine/animation/screen-transition.hpp>
#include <engine/animation/timeline.hpp>
#include <engine/color/color.hpp>
#include <engine/config.hpp>
#include <engine/debug/cli.hpp>
#include <engine/debug/log.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/pixel-format.hpp>
#include <engine/gl/pixel-type.hpp>
#include <engine/gl/rasterizer.hpp>
#include <engine/gl/texture-2d.hpp>
#include <engine/gl/texture-filter.hpp>
#include <engine/gl/texture-wrapping.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-attribute.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/input/application-events.hpp>
#include <engine/input/gamepad.hpp>
#include <engine/input/keyboard.hpp>
#include <engine/input/mapper.hpp>
#include <engine/input/mouse.hpp>
#include <engine/input/scancode.hpp>
#include <engine/render/compositor.hpp>
#include <engine/render/material-flags.hpp>
#include <engine/render/material-variable.hpp>
#include <engine/render/passes/bloom-pass.hpp>
#include <engine/render/passes/clear-pass.hpp>
#include <engine/render/passes/final-pass.hpp>
#include <engine/render/passes/fxaa-pass.hpp>
#include <engine/render/passes/ground-pass.hpp>
#include <engine/render/passes/material-pass.hpp>
#include <engine/render/passes/outline-pass.hpp>
#include <engine/render/passes/resample-pass.hpp>
#include <engine/render/passes/shadow-map-pass.hpp>
#include <engine/render/passes/sky-pass.hpp>
#include <engine/render/renderer.hpp>
#include <engine/render/vertex-attribute.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/scene/scene.hpp>
#include <engine/utility/dict.hpp>
#include <engine/utility/hash/fnv1a.hpp>
#include <engine/utility/paths.hpp>
#include <entt/entt.hpp>
#include <execution>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>
#include <chrono>

// Prevent cxxopts from using RTTI
#define CXXOPTS_NO_RTTI
#include <cxxopts.hpp>

using namespace hash::literals;

game::game(int argc, const char* const* argv)
{
	// Boot process
	debug::log::trace("Booting up...");
	
	// Profile boot duration
	#if !defined(NDEBUG)
		auto boot_t0 = std::chrono::high_resolution_clock::now();
	#endif
	
	parse_options(argc, argv);
	setup_resources();
	load_settings();
	setup_window();
	setup_audio();
	setup_input();
	load_strings();
	setup_rendering();
	setup_scenes();
	setup_animation();
	setup_ui();
	setup_rng();
	setup_entities();
	setup_systems();
	setup_controls();
	setup_debugging();
	setup_timing();
	
	active_ecoregion = nullptr;
	closed = false;
	
	// Profile boot duration
	#if !defined(NDEBUG)
		auto boot_t1 = std::chrono::high_resolution_clock::now();
	#endif
	
	debug::log::trace("Boot up complete");
	
	// Print boot duration
	#if !defined(NDEBUG)
		debug::log::info("Boot duration: {}", std::chrono::duration_cast<std::chrono::duration<double>>(boot_t1 - boot_t0));
	#endif
}

game::~game()
{
	debug::log::trace("Booting down...");
	
	// Exit all active game states
	while (!state_machine.empty())
	{
		state_machine.pop();
	}
	
	// Update window settings
	const auto& windowed_position = window->get_windowed_position();
	const auto& windowed_size = window->get_windowed_size();
	const bool maximized = window->is_maximized();
	const bool fullscreen = window->is_fullscreen();
	(*settings)["window_x"] = windowed_position.x();
	(*settings)["window_y"] = windowed_position.y();
	(*settings)["window_w"] = windowed_size.x();
	(*settings)["window_h"] = windowed_size.y();
	(*settings)["maximized"] = maximized;
	(*settings)["fullscreen"] = fullscreen;
	
	// Destruct window
	window.reset();
	
	// Save settings
	resource_manager->set_write_path(shared_config_path);
	resource_manager->save(*settings, "settings.cfg");
	
	// Destruct input and window managers
	input_manager.reset();
	window_manager.reset();
	
	// Shut down audio
	shutdown_audio();
	
	debug::log::trace("Boot down complete");
}

void game::parse_options(int argc, const char* const* argv)
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
			option_continue = true;
		}
		
		// --data
		if (result.count("data"))
		{
			option_data = result["data"].as<std::string>();
		}
		
		// --fullscreen
		if (result.count("fullscreen"))
		{
			option_fullscreen = true;
		}
		
		// --new-game
		if (result.count("new-game"))
		{
			option_new_game = true;
		}
		
		// --quick-start
		if (result.count("quick-start"))
		{
			option_quick_start = true;
		}
		
		// --reset
		if (result.count("reset"))
		{
			option_reset = true;
		}
		
		// --v-sync
		if (result.count("v-sync"))
		{
			option_v_sync = result["v-sync"].as<int>();
		}
		
		// --windowed
		if (result.count("windowed"))
		{
			option_windowed = true;
		}
		
		debug::log::info("Parsed {} command-line options", argc);
	}
	catch (const std::exception& e)
	{
		debug::log::error("An error occurred while parsing command-line options: {}", e.what());
	}
}

void game::setup_resources()
{
	// Allocate resource manager
	resource_manager = std::make_unique<::resource_manager>();
	
	// Get executable data path
	const auto data_path = get_executable_data_path();
	
	// Determine data package path
	if (option_data)
	{
		// Handle command-line data path option
		data_package_path = option_data.value();
		if (data_package_path.is_relative())
		{
			data_package_path = data_path / data_package_path;
		}
	}
	else
	{
		data_package_path = data_path / (config::application_slug + std::string("-data.zip"));
	}
	
	// Determine mods path
	mods_path = data_path / "mods";
	
	// Determine config paths
	local_config_path = get_local_config_path() / config::application_name;
	shared_config_path = get_shared_config_path() / config::application_name;
	saves_path = shared_config_path / "saves";
	screenshots_path = shared_config_path / "gallery";
	controls_path = shared_config_path / "controls";
	
	// Log paths
	debug::log::info("Data package path: \"{}\"", data_package_path.string());
	debug::log::info("Local config path: \"{}\"", local_config_path.string());
	debug::log::info("Shared config path: \"{}\"", shared_config_path.string());
	debug::log::info("Mods path: \"{}\"", mods_path.string());
	
	// Create nonexistent config directories
	std::vector<std::filesystem::path> config_paths;
	config_paths.push_back(local_config_path);
	config_paths.push_back(shared_config_path);
	config_paths.push_back(saves_path);
	config_paths.push_back(screenshots_path);
	config_paths.push_back(controls_path);
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
	if (std::filesystem::is_directory(mods_path))
	{
		for (const auto& entry: std::filesystem::directory_iterator{mods_path})
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
		resource_manager->mount(mods_path / mod_path);
	}
	
	// Mount config path
	resource_manager->mount(local_config_path);
	resource_manager->mount(shared_config_path);
	
	// Mount data package path
	resource_manager->mount(data_package_path);
	
	// Mount controls path
	resource_manager->mount(shared_config_path / "controls");
}

void game::load_settings()
{
	if (option_reset)
	{
		// Command-line reset option found, reset settings
		settings = std::make_shared<dict<hash::fnv1a32_t>>();
		resource_manager->set_write_path(shared_config_path);
		resource_manager->save(*settings, "settings.cfg");
		debug::log::info("Settings reset");
	}
	else
	{
		settings = resource_manager->load<dict<hash::fnv1a32_t>>("settings.cfg");
		if (!settings)
		{
			debug::log::info("Settings not found");
			settings = std::make_shared<dict<hash::fnv1a32_t>>();
		}
	}
}

void game::setup_window()
{
	// Construct window manager
	window_manager = app::window_manager::instance();
	
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
	read_or_write_setting(*this, "window_title", window_title);
	read_or_write_setting(*this, "window_x", window_x);
	read_or_write_setting(*this, "window_y", window_y);
	if (!read_or_write_setting(*this, "window_w", window_w) ||
		!read_or_write_setting(*this, "window_h", window_h))
	{
		resize = true;
	}
	read_or_write_setting(*this, "maximized", maximized);
	read_or_write_setting(*this, "fullscreen", fullscreen);
	read_or_write_setting(*this, "v_sync", v_sync);
	
	// If window size not set, resize and reposition relative to default display
	if (resize)
	{
		const app::display& display = window_manager->get_display(0);
		const auto& usable_bounds = display.get_usable_bounds();
		const auto usable_bounds_center = usable_bounds.center();
		
		const float default_windowed_scale = 1.0f / 1.2f;
		
		window_w = static_cast<int>((usable_bounds.max.x() - usable_bounds.min.x()) * default_windowed_scale);
		window_h = static_cast<int>((usable_bounds.max.y() - usable_bounds.min.y()) * default_windowed_scale);
		window_x = usable_bounds_center.x() - window_w / 2;
		window_y = usable_bounds_center.y() - window_h / 2;
	}
	
	// Handle window-related command-line options
	if (option_windowed)
	{
		// Start in windowed mode
		maximized = false;
		fullscreen = false;
	}
	if (option_fullscreen)
	{
		// Start in fullscreen mode
		fullscreen = true;
	}
	if (option_v_sync)
	{
		v_sync = option_v_sync.value();
	}
	
	// Construct window
	window = window_manager->create_window
	(
		window_title,
		{window_x, window_y},
		{window_w, window_h},
		maximized,
		fullscreen,
		v_sync
	);
	
	// Restrict window size
	window->set_minimum_size({160, 144});
	
	// Setup window closed callback
	window_closed_subscription = window->get_closed_channel().subscribe
	(
		[&](const auto& event)
		{
			closed = true;
		}
	);
}

void game::setup_audio()
{
	debug::log::trace("Setting up audio...");
	
	// Default audio settings
	master_volume = 1.0f;
	ambience_volume = 1.0f;
	effects_volume = 1.0f;
	mono_audio = false;
	captions = false;
	captions_size = 1.0f;
	
	// Read audio settings
	read_or_write_setting(*this, "master_volume", master_volume);
	read_or_write_setting(*this, "ambience_volume", ambience_volume);
	read_or_write_setting(*this, "effects_volume", effects_volume);
	read_or_write_setting(*this, "mono_audio", mono_audio);
	read_or_write_setting(*this, "captions", captions);
	read_or_write_setting(*this, "captions_size", captions_size);
	
	// Open audio device
	debug::log::trace("Opening audio device...");
	alc_device = alcOpenDevice(nullptr);
	if (!alc_device)
	{
		debug::log::error("Failed to open audio device: AL error code {}", alGetError());
		return;
	}
	else
	{
		// Get audio device name
		const ALCchar* alc_device_name = nullptr;
		if (alcIsExtensionPresent(alc_device, "ALC_ENUMERATE_ALL_EXT"))
		{
			alc_device_name = alcGetString(alc_device, ALC_ALL_DEVICES_SPECIFIER);
		}
		if (alcGetError(alc_device) != AL_NO_ERROR || !alc_device_name)
		{
			alc_device_name = alcGetString(alc_device, ALC_DEVICE_SPECIFIER);
		}
		
		// Log audio device name
		debug::log::info("Opened audio device \"{}\"", alc_device_name);
	}
	
	// Create audio context
	debug::log::trace("Creating audio context...");
	alc_context = alcCreateContext(alc_device, nullptr);
	if (!alc_context)
	{
		debug::log::error("Failed to create audio context: ALC error code {}", alcGetError(alc_device));
		alcCloseDevice(alc_device);
		return;
	}
	else
	{
		debug::log::trace("Created audio context");
	}
	
	// Make audio context current
	debug::log::trace("Making audio context current...");
	if (alcMakeContextCurrent(alc_context) == ALC_FALSE)
	{
		debug::log::error("Failed to make audio context current: ALC error code {}", alcGetError(alc_device));
		if (alc_context != nullptr)
		{
			alcDestroyContext(alc_context);
		}
		alcCloseDevice(alc_device);
		return;
	}
	else
	{
		debug::log::trace("Made audio context current");
	}
	
	debug::log::trace("Set up audio");
}

void game::setup_input()
{
	// Construct input manager
	input_manager = app::input_manager::instance();
	
	// Process initial input events, such as connecting gamepads
	input_manager->update();
	
	// Setup application quit callback
	application_quit_subscription = input_manager->get_event_dispatcher().subscribe<input::application_quit_event>
	(
		[&](const auto& event)
		{
			closed = true;
		}
	);
	
	// Gamepad deactivation function
	auto deactivate_gamepad = [&](const auto& event)
	{
		if (gamepad_active)
		{
			gamepad_active = false;
			input_manager->set_cursor_visible(true);
		}
	};
	
	// Setup gamepad activation callbacks
	gamepad_axis_moved_subscription = input_manager->get_event_dispatcher().subscribe<input::gamepad_axis_moved_event>
	(
		[&](const auto& event)
		{
			if (!gamepad_active && std::abs(event.position) > 0.5f)
			{
				gamepad_active = true;
				input_manager->set_cursor_visible(false);
			}
		}
	);
	gamepad_button_pressed_subscription = input_manager->get_event_dispatcher().subscribe<input::gamepad_button_pressed_event>
	(
		[&](const auto& event)
		{
			if (!gamepad_active)
			{
				gamepad_active = true;
				input_manager->set_cursor_visible(false);
			}
		}
	);
	
	// Setup gamepad deactivation callbacks
	mouse_button_pressed_subscription = input_manager->get_event_dispatcher().subscribe<input::mouse_button_pressed_event>
	(
		deactivate_gamepad
	);
	mouse_moved_subscription = input_manager->get_event_dispatcher().subscribe<input::mouse_moved_event>
	(
		deactivate_gamepad
	);
	mouse_scrolled_subscription = input_manager->get_event_dispatcher().subscribe<input::mouse_scrolled_event>
	(
		deactivate_gamepad
	);
	
	// Activate gamepad if one is connected
	if (!input_manager->get_gamepads().empty())
	{
		gamepad_active = true;
		input_manager->set_cursor_visible(false);
	}
	else
	{
		gamepad_active = false;
	}
}

void game::load_strings()
{
	debug::log::trace("Loading strings...");
	
	// Default strings settings
	language_tag = "en";
	
	// Read strings settings
	read_or_write_setting(*this, "language_tag", language_tag);
	
	// Slugify language tag
	std::string language_slug = language_tag;
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
	string_map = resource_manager->load<i18n::string_map>(language_slug + ".str");
	
	// Log language info
	debug::log::info("Language tag: {}", language_tag);
	
	// Change window title
	const std::string window_title = get_string(*this, "window_title");
	window->set_title(window_title);
	
	// Update window title setting
	(*settings)["window_title"] = window_title;
	
	debug::log::trace("Loaded strings");
}

void game::setup_rendering()
{
	debug::log::trace("Setting up rendering...");
	
	// Default rendering settings
	render_scale = 1.0f;
	anti_aliasing_method = render::anti_aliasing_method::fxaa;
	shadow_map_resolution = 4096;
	
	// Read rendering settings
	read_or_write_setting(*this, "render_scale", render_scale);
	read_or_write_setting(*this, "anti_aliasing_method", *reinterpret_cast<std::underlying_type_t<render::anti_aliasing_method>*>(&anti_aliasing_method));
	read_or_write_setting(*this, "shadow_map_resolution", shadow_map_resolution);
	
	// Create framebuffers
	::graphics::create_framebuffers(*this);
	
	// Load fallback material
	auto fallback_material = resource_manager->load<render::material>("fallback.mtl");
	
	// Setup common render passes
	{
		// Construct bloom pass
		bloom_pass = std::make_unique<render::bloom_pass>(window->get_rasterizer(), resource_manager.get());
		bloom_pass->set_source_texture(hdr_color_texture.get());
		bloom_pass->set_mip_chain_length(5);
		//bloom_pass->set_mip_chain_length(0);
		bloom_pass->set_filter_radius(0.005f);
		
		common_final_pass = std::make_unique<render::final_pass>(window->get_rasterizer(), ldr_framebuffer_a.get(), resource_manager.get());
		common_final_pass->set_color_texture(hdr_color_texture.get());
		common_final_pass->set_bloom_texture(bloom_pass->get_bloom_texture());
		common_final_pass->set_bloom_weight(0.04f);
		//common_final_pass->set_bloom_weight(0.0f);
		common_final_pass->set_blue_noise_texture(resource_manager->load<gl::texture_2d>("blue-noise.tex"));
		
		fxaa_pass = std::make_unique<render::fxaa_pass>(window->get_rasterizer(), &window->get_rasterizer()->get_default_framebuffer(), resource_manager.get());
		fxaa_pass->set_source_texture(ldr_color_texture_a.get());
		
		resample_pass = std::make_unique<render::resample_pass>(window->get_rasterizer(), &window->get_rasterizer()->get_default_framebuffer(), resource_manager.get());
		resample_pass->set_source_texture(ldr_color_texture_b.get());
		resample_pass->set_enabled(false);
		
		// Configure anti-aliasing according to settings
		graphics::select_anti_aliasing_method(*this, anti_aliasing_method);
		
		// Configure render scaling according to settings
		graphics::change_render_resolution(*this, render_scale);
	}
	
	// Setup UI compositor
	{
		ui_clear_pass = std::make_unique<render::clear_pass>(window->get_rasterizer(), &window->get_rasterizer()->get_default_framebuffer());
		ui_clear_pass->set_cleared_buffers(false, true, false);
		ui_clear_pass->set_clear_depth(-1.0f);
		
		ui_material_pass = std::make_unique<render::material_pass>(window->get_rasterizer(), &window->get_rasterizer()->get_default_framebuffer(), resource_manager.get());
		ui_material_pass->set_fallback_material(fallback_material);
		
		ui_compositor = std::make_unique<render::compositor>();
		ui_compositor->add_pass(ui_clear_pass.get());
		ui_compositor->add_pass(ui_material_pass.get());
	}
	
	// Setup underground compositor
	{
		underground_clear_pass = std::make_unique<render::clear_pass>(window->get_rasterizer(), hdr_framebuffer.get());
		underground_clear_pass->set_cleared_buffers(true, true, false);
		underground_clear_pass->set_clear_color({1, 0, 1, 0});
		underground_clear_pass->set_clear_depth(-1.0f);
		
		underground_material_pass = std::make_unique<render::material_pass>(window->get_rasterizer(), hdr_framebuffer.get(), resource_manager.get());
		underground_material_pass->set_fallback_material(fallback_material);
		
		underground_compositor = std::make_unique<render::compositor>();
		underground_compositor->add_pass(underground_clear_pass.get());
		underground_compositor->add_pass(underground_material_pass.get());
		underground_compositor->add_pass(bloom_pass.get());
		underground_compositor->add_pass(common_final_pass.get());
		underground_compositor->add_pass(fxaa_pass.get());
		underground_compositor->add_pass(resample_pass.get());
	}
	
	// Setup surface compositor
	{
		surface_shadow_map_clear_pass = std::make_unique<render::clear_pass>(window->get_rasterizer(), shadow_map_framebuffer.get());
		surface_shadow_map_clear_pass->set_cleared_buffers(false, true, false);
		surface_shadow_map_clear_pass->set_clear_depth(1.0f);
		
		surface_shadow_map_pass = std::make_unique<render::shadow_map_pass>(window->get_rasterizer(), resource_manager.get());
		
		surface_clear_pass = std::make_unique<render::clear_pass>(window->get_rasterizer(), hdr_framebuffer.get());
		surface_clear_pass->set_clear_color({0.0f, 0.0f, 0.0f, 1.0f});
		surface_clear_pass->set_cleared_buffers(true, true, false);
		surface_clear_pass->set_clear_depth(-1.0f);
		
		sky_pass = std::make_unique<render::sky_pass>(window->get_rasterizer(), hdr_framebuffer.get(), resource_manager.get());
		sky_pass->set_magnification(3.0f);
		
		ground_pass = std::make_unique<render::ground_pass>(window->get_rasterizer(), hdr_framebuffer.get(), resource_manager.get());
		
		surface_material_pass = std::make_unique<render::material_pass>(window->get_rasterizer(), hdr_framebuffer.get(), resource_manager.get());
		surface_material_pass->set_fallback_material(fallback_material);
		
		surface_outline_pass = std::make_unique<render::outline_pass>(window->get_rasterizer(), hdr_framebuffer.get(), resource_manager.get());
		surface_outline_pass->set_outline_width(0.25f);
		surface_outline_pass->set_outline_color(float4{1.0f, 1.0f, 1.0f, 1.0f});
		
		surface_compositor = std::make_unique<render::compositor>();
		surface_compositor->add_pass(surface_shadow_map_clear_pass.get());
		surface_compositor->add_pass(surface_shadow_map_pass.get());
		surface_compositor->add_pass(surface_clear_pass.get());
		surface_compositor->add_pass(sky_pass.get());
		//surface_compositor->add_pass(ground_pass.get());
		surface_compositor->add_pass(surface_material_pass.get());
		//surface_compositor->add_pass(surface_outline_pass.get());
		surface_compositor->add_pass(bloom_pass.get());
		surface_compositor->add_pass(common_final_pass.get());
		surface_compositor->add_pass(fxaa_pass.get());
		surface_compositor->add_pass(resample_pass.get());
	}
	
	// Create renderer
	renderer = std::make_unique<render::renderer>();
	
	debug::log::trace("Set up rendering");
}

void game::setup_scenes()
{
	debug::log::trace("Setting up scenes...");
	
	// Get default framebuffer
	const auto& viewport_size = window->get_viewport_size();
	const float viewport_aspect_ratio = static_cast<float>(viewport_size[0]) / static_cast<float>(viewport_size[1]);
	
	// Setup surface scene
	surface_scene = std::make_unique<scene::collection>();
	
	// Setup surface camera
	surface_camera = std::make_shared<scene::camera>();
	surface_camera->set_perspective(math::radians<float>(45.0f), viewport_aspect_ratio, 0.1f, 5000.0f);
	surface_camera->set_compositor(surface_compositor.get());
	surface_camera->set_composite_index(0);
	
	// Setup underground scene
	underground_scene = std::make_unique<scene::collection>();
	
	// Setup underground camera
	underground_camera = std::make_shared<scene::camera>();
	underground_camera->set_perspective(math::radians<float>(45.0f), viewport_aspect_ratio, 0.1f, 1000.0f);
	underground_camera->set_compositor(underground_compositor.get());
	underground_camera->set_composite_index(0);
	
	// Clear active scene
	active_scene = nullptr;
	
	debug::log::trace("Set up scenes");
}

void game::setup_animation()
{
	// Setup timeline system
	timeline = std::make_unique<::timeline>();
	timeline->set_autoremove(true);

	// Setup animator
	animator = std::make_unique<::animator>();
}

void game::setup_ui()
{
	// Default UI settings
	font_scale = 1.0f;
	debug_font_size_pt = 10.0f;
	menu_font_size_pt = 22.0f;
	title_font_size_pt = 80.0f;
	dyslexia_font = false;
	
	// Read UI settings
	read_or_write_setting(*this, "font_scale", font_scale);
	read_or_write_setting(*this, "debug_font_size_pt", debug_font_size_pt);
	read_or_write_setting(*this, "menu_font_size_pt", menu_font_size_pt);
	read_or_write_setting(*this, "title_font_size_pt", title_font_size_pt);
	read_or_write_setting(*this, "dyslexia_font", dyslexia_font);
	
	// Allocate font materials
	debug_font_material = std::make_shared<render::material>();
	menu_font_material = std::make_shared<render::material>();
	title_font_material = std::make_shared<render::material>();
	
	// Load fonts
	debug::log::trace("Loading fonts...");
	try
	{
		::load_fonts(*this);
		debug::log::trace("Loaded fonts");
	}
	catch (...)
	{
		debug::log::error("Failed to load fonts");
	}
	
	// Get default framebuffer
	const auto& viewport_size = window->get_viewport_size();
	const float viewport_aspect_ratio = static_cast<float>(viewport_size[0]) / static_cast<float>(viewport_size[1]);
	
	// Setup UI scene
	ui_scene = std::make_unique<scene::collection>();
	
	// Setup UI camera
	ui_camera = std::make_unique<scene::camera>();
	ui_camera->set_compositor(ui_compositor.get());
	const float clip_left = 0.0f;
	const float clip_right = static_cast<float>(viewport_size.x());
	const float clip_top = 0.0f;
	const float clip_bottom = static_cast<float>(viewport_size.y());
	const float clip_near = -100.0f;
	const float clip_far = 100.0f;
	ui_camera->set_orthographic(clip_left, clip_right, clip_top, clip_bottom, clip_near, clip_far);
	ui_camera->look_at({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f});
	
	// Menu BG material
	menu_bg_material = std::make_shared<render::material>();
	menu_bg_material->set_shader_template(resource_manager->load<gl::shader_template>("ui-element-untextured.glsl"));
	std::shared_ptr<render::material_float4> menu_bg_tint = std::make_shared<render::material_float4>(1, float4{0.0f, 0.0f, 0.0f, 0.5f});
	menu_bg_material->set_variable("tint", menu_bg_tint);
	menu_bg_material->set_blend_mode(render::material_blend_mode::translucent);
	
	// Menu BG billboard
	menu_bg_billboard = std::make_unique<scene::billboard>();
	menu_bg_billboard->set_material(menu_bg_material);
	menu_bg_billboard->set_scale({std::ceil(viewport_size.x() * 0.5f), std::ceil(viewport_size.y() * 0.5f), 1.0f});
	menu_bg_billboard->set_translation({std::floor(viewport_size.x() * 0.5f), std::floor(viewport_size.y() * 0.5f), -100.0f});
	
	// Create fade transition
	fade_transition = std::make_unique<screen_transition>();
	fade_transition->get_material()->set_shader_template(resource_manager->load<gl::shader_template>("fade-transition.glsl"));
	fade_transition_color = std::make_shared<render::material_float3>(1, float3{0, 0, 0});
	fade_transition->get_material()->set_variable("color", fade_transition_color);
	fade_transition->get_billboard()->set_translation({0, 0, 98});
	
	// Create inner radial transition
	radial_transition_inner = std::make_unique<screen_transition>();
	radial_transition_inner->get_material()->set_shader_template(resource_manager->load<gl::shader_template>("radial-transition-inner.glsl"));
	
	// Create outer radial transition
	radial_transition_outer = std::make_unique<screen_transition>();
	radial_transition_outer->get_material()->set_shader_template(resource_manager->load<gl::shader_template>("radial-transition-outer.glsl"));
	
	// Menu BG animations
	{
		auto menu_bg_frame_callback = [menu_bg_tint](int channel, const float& opacity)
		{
			menu_bg_tint->set(float4{0.0f, 0.0f, 0.0f, opacity});
		};
		
		// Menu BG fade in animation
		menu_bg_fade_in_animation = std::make_unique<animation<float>>();
		{
			menu_bg_fade_in_animation->set_interpolator(ease<float>::out_cubic);
			animation_channel<float>* channel = menu_bg_fade_in_animation->add_channel(0);
			channel->insert_keyframe({0.0f, 0.0f});
			channel->insert_keyframe({config::menu_fade_in_duration, config::menu_bg_opacity});
			menu_bg_fade_in_animation->set_frame_callback(menu_bg_frame_callback);
			menu_bg_fade_in_animation->set_start_callback
			(
				[&, menu_bg_tint]()
				{
					ui_scene->add_object(*menu_bg_billboard);
					
					menu_bg_tint->set(float4{0.0f, 0.0f, 0.0f, 0.0f});
					//menu_bg_billboard->set_active(true);
				}
			);
		}
		
		// Menu BG fade out animation
		menu_bg_fade_out_animation = std::make_unique<animation<float>>();
		{
			menu_bg_fade_out_animation->set_interpolator(ease<float>::out_cubic);
			animation_channel<float>* channel = menu_bg_fade_out_animation->add_channel(0);
			channel->insert_keyframe({0.0f, config::menu_bg_opacity});
			channel->insert_keyframe({config::menu_fade_out_duration, 0.0f});
			menu_bg_fade_out_animation->set_frame_callback(menu_bg_frame_callback);
			menu_bg_fade_out_animation->set_end_callback
			(
				[&]()
				{
					ui_scene->remove_object(*menu_bg_billboard);
					//menu_bg_billboard->set_active(false);
				}
			);
		}
	}
	
	// Add UI scene objects to UI scene
	ui_scene->add_object(*ui_camera);
	ui_scene->add_object(*fade_transition->get_billboard());
	
	// Add UI animations to animator
	animator->add_animation(fade_transition->get_animation());
	animator->add_animation(menu_bg_fade_in_animation.get());
	animator->add_animation(menu_bg_fade_out_animation.get());
	
	// Setup window resized callback
	window_resized_subscription = window->get_resized_channel().subscribe
	(
		[&](const auto& event)
		{
			const auto& viewport_size = event.window->get_viewport_size();
			const float viewport_aspect_ratio = static_cast<float>(viewport_size.x()) / static_cast<float>(viewport_size.y());
			
			// Resize framebuffers
			::graphics::change_render_resolution(*this, render_scale);
			
			// Update camera projection matrix
			surface_camera->set_perspective
			(
				surface_camera->get_fov(),
				viewport_aspect_ratio,
				surface_camera->get_clip_near(),
				surface_camera->get_clip_far()
			);
			
			// Update UI camera projection matrix
			ui_camera->set_orthographic
			(
				0.0f,
				static_cast<float>(viewport_size.x()),
				0.0f,
				static_cast<float>(viewport_size.y()),
				ui_camera->get_clip_near(),
				ui_camera->get_clip_far()
			);
			
			// Resize menu BG billboard
			menu_bg_billboard->set_scale({std::ceil(viewport_size.x() * 0.5f), std::ceil(viewport_size.y() * 0.5f), 1.0f});
			menu_bg_billboard->set_translation({std::floor(viewport_size.x() * 0.5f), std::floor(viewport_size.y() * 0.5f), -100.0f});
			
			// Re-align debug text
			frame_time_text->set_translation({std::round(0.0f), std::round(viewport_size.y() - debug_font.get_font_metrics().size), 99.0f});
			
			// Re-align menu text
			::menu::align_text(*this);
		}
	);
}

void game::setup_rng()
{
	std::random_device rd;
	rng.seed(rd());
}

void game::setup_entities()
{
	// Create entity registry
	entity_registry = std::make_unique<entt::registry>();
}

void game::setup_systems()
{
	const auto& viewport_size = window->get_viewport_size();
	float4 viewport = {0.0f, 0.0f, static_cast<float>(viewport_size[0]), static_cast<float>(viewport_size[1])};
	
	// Setup terrain system
	terrain_system = std::make_unique<::terrain_system>(*entity_registry);
	terrain_system->set_patch_side_length(31.0f);
	terrain_system->set_patch_subdivisions(31);
	terrain_system->set_scene_collection(surface_scene.get());
	
	// Setup camera system
	camera_system = std::make_unique<::camera_system>(*entity_registry);
	camera_system->set_viewport(viewport);
	
	// Setup subterrain system
	subterrain_system = std::make_unique<::subterrain_system>(*entity_registry, resource_manager.get());
	subterrain_system->set_scene(underground_scene.get());
	
	// Setup collision system
	collision_system = std::make_unique<::collision_system>(*entity_registry);
	
	// Setup behavior system
	behavior_system = std::make_unique<::behavior_system>(*entity_registry);
	
	// Setup steering system
	steering_system = std::make_unique<::steering_system>(*entity_registry);
	
	// Setup locomotion system
	locomotion_system = std::make_unique<::locomotion_system>(*entity_registry);
	
	// Setup physics system
	physics_system = std::make_unique<::physics_system>(*entity_registry);
	physics_system->set_gravity({0.0f, -9.80665f * 100.0f, 0.0f});
	
	// Setup spring system
	spring_system = std::make_unique<::spring_system>(*entity_registry);
	
	// Setup spatial system
	spatial_system = std::make_unique<::spatial_system>(*entity_registry);
	
	// Setup constraint system
	constraint_system = std::make_unique<::constraint_system>(*entity_registry);
	
	// Setup orbit system
	orbit_system = std::make_unique<::orbit_system>(*entity_registry);
	
	// Setup blackbody system
	blackbody_system = std::make_unique<::blackbody_system>(*entity_registry);
	blackbody_system->set_illuminant(color::illuminant::deg2::d55<double>);
	
	// RGB wavelengths for atmospheric scatteering
	rgb_wavelengths = {680, 550, 440};
	
	// Setup atmosphere system
	atmosphere_system = std::make_unique<::atmosphere_system>(*entity_registry);
	atmosphere_system->set_rgb_wavelengths(rgb_wavelengths * 1e-9);
	atmosphere_system->set_sky_pass(sky_pass.get());
	
	// Setup astronomy system
	astronomy_system = std::make_unique<::astronomy_system>(*entity_registry);
	astronomy_system->set_transmittance_samples(16);
	astronomy_system->set_sky_pass(sky_pass.get());
	
	// Setup render system
	render_system = std::make_unique<::render_system>(*entity_registry);
	render_system->set_renderer(renderer.get());
	//render_system->add_layer(underground_scene.get());
	render_system->add_layer(surface_scene.get());
	render_system->add_layer(ui_scene.get());
}

void game::setup_controls()
{
	debug::log::trace("Setting up controls...");
	
	// Load SDL game controller mappings database
	// debug::log::trace("Loading SDL game controller mappings...");
	// file_buffer* game_controller_db = resource_manager->load<file_buffer>("gamecontrollerdb.txt");
	// if (!game_controller_db)
	// {
		// debug::log::error("Failed to load SDL game controller mappings");
	// }
	// else
	// {
		// app->add_game_controller_mappings(game_controller_db->data(), game_controller_db->size());
		// debug::log::trace("Loaded SDL game controller mappings");
		
		// resource_manager->unload("gamecontrollerdb.txt");
	// }
	
	// Pass input event queue to action maps
	event::dispatcher* input_event_dispatcher = &input_manager->get_event_dispatcher();
	window_action_map.set_event_dispatcher(input_event_dispatcher);
	menu_action_map.set_event_dispatcher(input_event_dispatcher);
	movement_action_map.set_event_dispatcher(input_event_dispatcher);
	keeper_action_map.set_event_dispatcher(input_event_dispatcher);
	
	// Default control profile settings
	control_profile_filename = "controls.cfg";
	
	// Read control profile settings
	if (read_or_write_setting(*this, "control_profile", control_profile_filename))
	{
		control_profile = resource_manager->load<::control_profile>(control_profile_filename);
	}
	
	if (!control_profile)
	{
		// Allocate control profile
		control_profile = std::make_shared<::control_profile>();
		
		// Reset control profile to default settings.
		::reset_control_profile(*control_profile);
		
		// Save control profile
		resource_manager->set_write_path(controls_path);
		resource_manager->save(*control_profile, control_profile_filename);
	}
	
	// Apply control profile
	::apply_control_profile(*this, *control_profile);
	
	// Setup mouse sensitivity
	mouse_pan_factor = mouse_radians_per_pixel * mouse_pan_sensitivity * (invert_mouse_pan ? -1.0 : 1.0);
	mouse_tilt_factor = mouse_radians_per_pixel * mouse_tilt_sensitivity * (invert_mouse_tilt ? -1.0 : 1.0);
	
	// Setup action callbacks
	setup_window_controls(*this);
	setup_menu_controls(*this);
	setup_game_controls(*this);
	
	// Enable window controls
	enable_window_controls(*this);
	
	debug::log::trace("Set up controls");
}

void game::setup_debugging()
{
	cli = std::make_unique<debug::cli>();
	
	const auto& viewport_size = window->get_viewport_size();
	
	frame_time_text = std::make_unique<scene::text>();
	frame_time_text->set_material(debug_font_material);
	frame_time_text->set_color({1.0f, 1.0f, 0.0f, 1.0f});
	frame_time_text->set_font(&debug_font);
	frame_time_text->set_translation({std::round(0.0f), std::round(viewport_size.y() - debug_font.get_font_metrics().size), 99.0f});
	
	ui_scene->add_object(*frame_time_text);
}

void game::setup_timing()
{
	// Init default settings
	max_frame_rate = static_cast<float>(window_manager->get_display(0).get_refresh_rate() * 2);
	
	// Read settings
	read_or_write_setting(*this, "fixed_update_rate", fixed_update_rate);
	read_or_write_setting(*this, "max_frame_rate", max_frame_rate);
	read_or_write_setting(*this, "limit_frame_rate", limit_frame_rate);
	
	const auto fixed_update_interval = std::chrono::duration_cast<::frame_scheduler::duration_type>(std::chrono::duration<double>(1.0 / fixed_update_rate));
	const auto min_frame_duration = (limit_frame_rate) ? std::chrono::duration_cast<::frame_scheduler::duration_type>(std::chrono::duration<double>(1.0 / max_frame_rate)) : frame_scheduler::duration_type::zero();
	const auto max_frame_duration = fixed_update_interval * 15;
	
	// Configure frame scheduler
	frame_scheduler.set_fixed_update_interval(fixed_update_interval);
	frame_scheduler.set_min_frame_duration(min_frame_duration);
	frame_scheduler.set_max_frame_duration(max_frame_duration);
	frame_scheduler.set_fixed_update_callback(std::bind_front(&game::fixed_update, this));
	frame_scheduler.set_variable_update_callback(std::bind_front(&game::variable_update, this));
	
	// Init frame duration average
	average_frame_duration.reserve(15);
}

void game::shutdown_audio()
{
	debug::log::trace("Shutting down audio...");
	
	if (alc_context)
	{
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(alc_context);
	}
	
	if (alc_device)
	{
		alcCloseDevice(alc_device);
	}
	
	debug::log::trace("Shut down audio");
}

void game::fixed_update(::frame_scheduler::duration_type fixed_update_time, ::frame_scheduler::duration_type fixed_update_interval)
{
	const float t = std::chrono::duration<float>(fixed_update_time).count();
	const float dt = std::chrono::duration<float>(fixed_update_interval).count();
	
	// Update tweens
	sky_pass->update_tweens();
	
	// Process window events
	window_manager->update();
	
	// Process function queue
	while (!function_queue.empty())
	{
		function_queue.front()();
		function_queue.pop();
	}
	
	// Advance tline
	//timeline->advance(dt);
	
	// Update entity systems
	//terrain_system->update(t, dt);
	//subterrain_system->update(t, dt);
	collision_system->update(t, dt);
	behavior_system->update(t, dt);
	steering_system->update(t, dt);
	locomotion_system->update(t, dt);
	physics_system->update(t, dt);
	camera_system->update(t, dt);
	orbit_system->update(t, dt);
	blackbody_system->update(t, dt);
	atmosphere_system->update(t, dt);
	astronomy_system->update(t, dt);
	spring_system->update(t, dt);
	spatial_system->update(t, dt);
	constraint_system->update(t, dt);
	animator->animate(dt);
	render_system->update(t, dt);
}

void game::variable_update(::frame_scheduler::duration_type fixed_update_time, ::frame_scheduler::duration_type fixed_update_interval, ::frame_scheduler::duration_type accumulated_time)
{
	// Calculate subframe interpolation factor (`alpha`)
	const float alpha = static_cast<float>(std::chrono::duration<double, ::frame_scheduler::duration_type::period>{accumulated_time} / fixed_update_interval);
	
	// Sample average frame duration
	const float average_frame_ms = average_frame_duration(std::chrono::duration<float, std::milli>(frame_scheduler.get_frame_duration()).count());
	const float average_frame_fps = 1000.0f / average_frame_ms;
	
	// Update frame rate display
	frame_time_text->set_content(std::format("{:5.02f}ms / {:5.02f} FPS", average_frame_ms, average_frame_fps));
	
	// Process input events
	input_manager->update();
	
	// Interpolate physics
	physics_system->interpolate(alpha);
	
	// Render
	render_system->draw(alpha);
	window->swap_buffers();
}

void game::execute()
{
	// Change to initial state
	state_machine.emplace(std::make_unique<main_menu_state>(*this, true));
	
	debug::log::trace("Entered main loop");
	
	frame_scheduler.refresh();
	
	while (!closed)
	{
		frame_scheduler.tick();
	}
	
	debug::log::trace("Exited main loop");
	
	// Exit all active game states
	while (!state_machine.empty())
	{
		state_machine.pop();
	}
}
