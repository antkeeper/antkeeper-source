// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/game.hpp"
#include "game/debug/shell.hpp"
#include "game/debug/shell-buffer.hpp"
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
#include "game/world.hpp"
#include "game/systems/astronomy-system.hpp"
#include "game/systems/atmosphere-system.hpp"
#include "game/systems/behavior-system.hpp"
#include "game/systems/blackbody-system.hpp"
#include "game/systems/camera-system.hpp"
#include "game/systems/collision-system.hpp"
#include "game/systems/constraint-system.hpp"
#include "game/systems/locomotion-system.hpp"
#include "game/systems/ik-system.hpp"
#include "game/systems/animation-system.hpp"
#include "game/systems/orbit-system.hpp"
#include "game/systems/render-system.hpp"
#include "game/systems/spatial-system.hpp"
#include "game/systems/steering-system.hpp"
#include "game/systems/physics-system.hpp"
#include "game/systems/subterrain-system.hpp"
#include "game/systems/terrain-system.hpp"
#include "game/systems/reproductive-system.hpp"
#include "game/systems/metabolic-system.hpp"
#include "game/systems/metamorphosis-system.hpp"
#include "game/components/animation-component.hpp"
#include <algorithm>
#include <cctype>
#include <engine/animation/ease.hpp>
#include <engine/animation/animation-sequence.hpp>
#include <engine/color/color.hpp>
#include <engine/config.hpp>
#include <engine/debug/log.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/pixel-format.hpp>
#include <engine/gl/pixel-type.hpp>
#include <engine/gl/texture.hpp>
#include <engine/gl/vertex-array.hpp>
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
#include <engine/render/passes/composite-pass.hpp>
#include <engine/render/passes/material-pass.hpp>
#include <engine/render/passes/sky-pass.hpp>
#include <engine/render/passes/clear-pass.hpp>
#include <engine/render/renderer.hpp>
#include <engine/render/vertex-attribute-location.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/scene/scene.hpp>
#include <engine/utility/dict.hpp>
#include <engine/hash/fnv.hpp>
#include <engine/utility/paths.hpp>
#include <engine/ui/label.hpp>
#include <entt/entt.hpp>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>
#include <chrono>

#include <engine/script/script-math-module.hpp>
#include <engine/script/script-io-module.hpp>
#include <engine/script/script-event-module.hpp>
#include <engine/script/script-global-module.hpp>

// Prevent cxxopts from using RTTI
#define CXXOPTS_NO_RTTI
#include <cxxopts.hpp>

using namespace hash::literals;

game::game(int argc, const char* const* argv)
{
	// Boot process
	debug::log_debug("Booting up...");
	
	parse_options(argc, argv);
	setup_resources();
	load_settings();
	setup_window();
	setup_audio();
	setup_input();
	load_language();
	setup_rendering();
	setup_entities();
	setup_scenes();
	setup_animation();
	setup_ui();
	setup_rng();
	setup_systems();
	setup_controls();
	setup_scripting();
	setup_debugging();
	setup_timing();
	
	debug::log_debug("Booting up... OK");
}

game::~game()
{
	debug::log_debug("Booting down...");
	
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
	resource_manager->save(*settings, "settings.json");
	
	// Destruct input and window managers
	input_manager.reset();
	window_manager.reset();
	
	debug::log_debug("Booting down... OK");
}

void game::parse_options(int argc, const char* const* argv)
{
	if (argc <= 1)
	{
		// No command-line options specified
		return;
	}
	
	debug::log_debug("Parsing command-line options...");
	
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
		
		debug::log_debug("Parsing command-line options... OK");
		debug::log_info("Parsed {} command-line options", argc);
	}
	catch (const std::exception& e)
	{
		debug::log_error("Failed to parse one or more command-line options: {}", e.what());
		debug::log_debug("Parsing command-line options... FAILED");
	}
}

void game::setup_resources()
{
	debug::log_debug("Setting up resources...");

	// Allocate resource manager
	resource_manager = std::make_unique<::resource_manager>();
	
	// Get executable data path
	const auto data_path = executable_data_directory_path();
	
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
	local_config_path = ::local_config_directory_path() / config::application_name;
	shared_config_path = ::shared_config_directory_path() / config::application_name;
	saves_path = shared_config_path / "saves";
	screenshots_path = shared_config_path / "gallery";
	controls_path = shared_config_path / "controls";
	
	// Log paths
	debug::log_info("Data package path: \"{}\"", data_package_path.string());
	debug::log_info("Local config path: \"{}\"", local_config_path.string());
	debug::log_info("Shared config path: \"{}\"", shared_config_path.string());
	debug::log_info("Mods path: \"{}\"", mods_path.string());
	
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
				debug::log_info("Created directory \"{}\"", path.string());
			}
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			debug::log_error("Failed to create directory \"{}\": {}", path.string(), e.what());
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
				debug::log_info("Found mod \"{}\"", entry.path().filename().string());
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
	resource_manager->mount(data_path / "data");
	
	// Mount controls path
	resource_manager->mount(shared_config_path / "controls");

	debug::log_debug("Setting up resources... OK");
}

void game::load_settings()
{
	debug::log_debug("Loading settings...");

	if (option_reset)
	{
		// Command-line reset option found, reset settings
		settings = std::make_shared<json>();
		resource_manager->set_write_path(shared_config_path);
		resource_manager->save(*settings, "settings.json");
		debug::log_info("Settings reset");
	}
	else
	{
		settings = resource_manager->load<json>("settings.json");
		if (!settings)
		{
			debug::log_info("Settings not found");
			settings = std::make_shared<json>();
		}
	}

	debug::log_debug("Loading settings... OK");
}

void game::setup_window()
{
	debug::log_debug("Setting up window...");

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
		const auto primary_display = window_manager->get_primary_display();
		if (!primary_display)
		{
			std::string error_message = "Failed to get primary display.";
			debug::log_fatal("{}", error_message);
			throw std::runtime_error(std::move(error_message));
		}

		const auto& usable_bounds = primary_display->get_usable_bounds();
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
		v_sync,
		{0.0f, 0.0f, 0.0f}
	);
	
	// Restrict window size
	window->set_minimum_size({160, 144});
	
	// Setup window closed callback
	window_closed_subscription = window->get_closed_channel().subscribe
	(
		[&]([[maybe_unused]] const auto& event)
		{
			closed = true;
		}
	);

	debug::log_debug("Setting up window... OK");
}

void game::setup_audio()
{
	debug::log_debug("Setting up audio...");
	
	// Default audio settings
	master_volume = 1.0f;
	ambience_volume = 1.0f;
	effects_volume = 1.0f;
	captions = false;
	captions_size = 1.0f;
	
	// Read audio settings
	read_or_write_setting(*this, "master_volume", master_volume);
	read_or_write_setting(*this, "ambience_volume", ambience_volume);
	read_or_write_setting(*this, "effects_volume", effects_volume);
	read_or_write_setting(*this, "captions", captions);
	read_or_write_setting(*this, "captions_size", captions_size);

	// Limit audio settings
	master_volume = math::clamp(master_volume, 0.0f, 1.0f);
	ambience_volume = math::clamp(ambience_volume, 0.0f, 1.0f);
	effects_volume = math::clamp(effects_volume, 0.0f, 1.0f);
	
	// Init sound system
	debug::log_debug("Constructing sound system...");
	sound_system = std::make_unique<audio::sound_system>();
	debug::log_debug("Constructing sound system... OK");
	
	// Print sound system info
	debug::log_info("Audio playback device: {}", sound_system->get_playback_device_name());

	// Update sound system settings
	sound_system->get_listener().set_gain(master_volume);

	// Load UI sounds
	menu_up_sound = std::make_shared<audio::sound_que>(resource_manager->load<audio::sound_wave>("sounds/menu-up.wav"));
	menu_down_sound = std::make_shared<audio::sound_que>(resource_manager->load<audio::sound_wave>("sounds/menu-down.wav"));
	
	debug::log_debug("Setting up audio... OK");
}

void game::setup_input()
{
	debug::log_debug("Setting up input...");

	// Construct input manager
	input_manager = app::input_manager::instance();
	
	// Process initial input events, such as connecting gamepads
	input_manager->update();
	
	// Setup application quit callback
	application_quit_subscription = input_manager->get_event_dispatcher().subscribe<input::application_quit_event>
	(
		[&]([[maybe_unused]] const auto& event)
		{
			closed = true;
		}
	);
	
	/*
	// Gamepad deactivation function
	auto deactivate_gamepad = [&](const auto& event)
	{
		if (gamepad_active)
		{
			gamepad_active = false;
			
			// WARNING: huge source of lag
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
	*/

	debug::log_debug("Setting up input... OK");
}

void game::load_language()
{
	debug::log_debug("Loading language...");
	
	// Default language tag setting
	language_tag = "en";
	
	// Read language tag setting
	read_or_write_setting(*this, "language_tag", language_tag);
	
	// Log language tag
	debug::log_info("Language tag: {}", language_tag);
	
	// Load languages
	languages = resource_manager->load<json>("localization/languages.json");
	
	// Load language string map
	string_map = resource_manager->load<json>(std::format("localization/strings.{}.json", language_tag));
	
	// Change window title
	const std::string window_title = get_string(*this, "window_title");
	window->set_title(window_title);
	
	// Update window title setting
	(*settings)["window_title"] = window_title;
	
	debug::log_debug("Loading language... OK");
}

void game::setup_rendering()
{
	debug::log_debug("Setting up rendering...");
	
	// Default rendering settings
	render_scale = 1.0f;
	anti_aliasing_method = render::anti_aliasing_method::none;
	shadow_map_resolution = 4096;
	
	// Read rendering settings
	read_or_write_setting(*this, "render_scale", render_scale);
	read_or_write_setting(*this, "anti_aliasing_method", *reinterpret_cast<std::underlying_type_t<render::anti_aliasing_method>*>(&anti_aliasing_method));
	read_or_write_setting(*this, "shadow_map_resolution", shadow_map_resolution);
	
	// Create framebuffers
	::graphics::create_framebuffers(*this);
	
	// Load fallback material
	auto fallback_material = resource_manager->load<render::material>("fallback.mtl");
	
	// Setup UI render passes
	{
		ui_material_pass = std::make_unique<render::material_pass>(&window->get_graphics_pipeline(), ui_framebuffer.get(), resource_manager.get());
		ui_material_pass->set_fallback_material(fallback_material);
		
		ui_material_pass->set_clear_mask(gl::color_clear_bit | gl::depth_clear_bit | gl::stencil_clear_bit);
		ui_material_pass->set_clear_value({{0.0f, 0.0f, 0.0f, 0.0f}, 0.0f, 0});
		
		ui_compositor = std::make_unique<render::compositor>();
		ui_compositor->add_pass(ui_material_pass.get());
	}
	
	// Setup scene render passes
	{
		// Copnstruct clear pass
		clear_pass = std::make_unique<render::clear_pass>(&window->get_graphics_pipeline(), scene_framebuffer.get());
		clear_pass->set_clear_mask(gl::color_clear_bit | gl::depth_clear_bit | gl::stencil_clear_bit);
		clear_pass->set_clear_value({{0.0f, 0.0f, 0.0f, 0.0f}, 0.0f, 0});
		
		// Construct sky pass
		sky_pass = std::make_unique<render::sky_pass>(&window->get_graphics_pipeline(), scene_framebuffer.get(), resource_manager.get());
		// sky_pass->set_clear_mask(gl::color_clear_bit | gl::depth_clear_bit | gl::stencil_clear_bit);
		// sky_pass->set_clear_value({{0.0f, 0.0f, 0.0f, 0.0f}, 0.0f, 0});
		// sky_pass->set_magnification(3.0f);
		
		// Construct material pass
		scene_material_pass = std::make_unique<render::material_pass>(&window->get_graphics_pipeline(), scene_framebuffer.get(), resource_manager.get());
		scene_material_pass->set_fallback_material(fallback_material);

		// Construct bloom pass
		bloom_pass = std::make_unique<render::bloom_pass>(&window->get_graphics_pipeline(), resource_manager.get());
		bloom_pass->set_source_texture(scene_color_texture);
		bloom_pass->set_mip_chain_length(5);
		bloom_pass->set_filter_radius(0.005f);
		
		// Construct composite pass
		composite_pass = std::make_unique<render::composite_pass>(&window->get_graphics_pipeline(), nullptr, resource_manager.get());
		composite_pass->set_luminance_texture(scene_color_texture);
		composite_pass->set_bloom_texture(bloom_pass->get_bloom_texture());
		composite_pass->set_bloom_strength(0.03f);
		composite_pass->set_noise_texture(resource_manager->load<gl::texture_2d>("blue-noise.tex"));
		composite_pass->set_noise_strength(1.0f / 255.0f);
		composite_pass->set_overlay_texture(ui_color_texture);
		
		// Construct compositor and add passes
		scene_compositor = std::make_unique<render::compositor>();
		scene_compositor->add_pass(clear_pass.get());
		scene_compositor->add_pass(sky_pass.get());
		scene_compositor->add_pass(scene_material_pass.get());
		scene_compositor->add_pass(bloom_pass.get());
		scene_compositor->add_pass(composite_pass.get());
	}
	
	// Configure anti-aliasing according to settings
	graphics::select_anti_aliasing_method(*this, anti_aliasing_method);

	// Configure render scaling according to settings
	graphics::change_render_resolution(*this, render_scale);
	
	// Create renderer
	renderer = std::make_unique<render::renderer>(window->get_graphics_pipeline(), *resource_manager);
	
	debug::log_debug("Setting up rendering... OK");
}

void game::setup_scenes()
{
	debug::log_debug("Setting up scenes...");
	
	// Ratio of meters to scene units.
	constexpr float scene_scale = 1.0f / 100.0f;
	
	// Get default framebuffer
	const auto& viewport_size = window->get_viewport_size();
	const float viewport_aspect_ratio = static_cast<float>(viewport_size[0]) / static_cast<float>(viewport_size[1]);
	
	// Allocate and init exterior scene
	exterior_scene = std::make_unique<scene::collection>();
	exterior_scene->set_scale(scene_scale);
	
	// Allocate and init exterior camera
	exterior_camera = std::make_shared<scene::camera>();
	exterior_camera->set_perspective(math::radians<float>(45.0f), viewport_aspect_ratio, 0.5f);
	exterior_camera->set_compositor(scene_compositor.get());
	exterior_camera->set_composite_index(0);
	
	// Allocate and init interior scene
	interior_scene = std::make_unique<scene::collection>();
	interior_scene->set_scale(scene_scale);
	
	// Clear active scene
	active_scene = nullptr;
	
	debug::log_debug("Setting up scenes... OK");
}

void game::setup_animation()
{
	debug::log_debug("Setting up animation...");
	debug::log_debug("Setting up animation... OK");
}

void game::setup_ui()
{
	debug::log_debug("Setting up UI...");
	
	// Default UI settings
	font_scale = 1.0f;
	debug_font_size_pt = 11.0f;
	menu_font_size_pt = 22.0f;
	title_font_size_pt = 80.0f;
	dyslexia_font = false;
	
	// Read UI settings
	read_or_write_setting(*this, "font_scale", font_scale);
	read_or_write_setting(*this, "debug_font_size_pt", debug_font_size_pt);
	read_or_write_setting(*this, "menu_font_size_pt", menu_font_size_pt);
	read_or_write_setting(*this, "title_font_size_pt", title_font_size_pt);
	read_or_write_setting(*this, "dyslexia_font", dyslexia_font);
	
	// Build font materials
	debug_font_material = std::make_shared<render::material>();
	menu_font_material = std::make_shared<render::material>();
	title_font_material = std::make_shared<render::material>();
	
	// Load fonts
	debug::log_debug("Loading fonts...");
	try
	{
		::load_fonts(*this);
		debug::log_debug("Loading fonts... OK");
	}
	catch (const std::exception& e)
	{
		debug::log_error("Failed to load fonts: {}", e.what());
		debug::log_debug("Loading fonts... FAILED");
	}
	
	// Get default framebuffer
	const auto& viewport_size = window->get_viewport_size();
	
	// Setup UI canvas
	ui_canvas = std::make_shared<ui::canvas>();
	ui_canvas->set_margins(0.0f, 0.0f, static_cast<float>(viewport_size.x()), static_cast<float>(viewport_size.y()));
	
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
	std::shared_ptr<render::matvar_fvec4> menu_bg_tint = std::make_shared<render::matvar_fvec4>(1, math::fvec4{0.0f, 0.0f, 0.0f, 0.5f});
	menu_bg_material->set_variable("tint"_fnv1a32, menu_bg_tint);
	menu_bg_material->set_blend_mode(render::material_blend_mode::translucent);
	
	// Menu BG billboard
	menu_bg_billboard = std::make_unique<scene::billboard>();
	menu_bg_billboard->set_material(menu_bg_material);
	menu_bg_billboard->set_scale({std::ceil(viewport_size.x() * 0.5f), std::ceil(viewport_size.y() * 0.5f), 1.0f});
	menu_bg_billboard->set_translation({std::floor(viewport_size.x() * 0.5f), std::floor(viewport_size.y() * 0.5f), -100.0f});

	// Menu BG material
	screen_transition_material = std::make_shared<render::material>();
	screen_transition_material->set_shader_template(resource_manager->load<gl::shader_template>("ui-element-untextured.glsl"));
	std::shared_ptr<render::matvar_fvec4> screen_transition_tint = std::make_shared<render::matvar_fvec4>(1, math::fvec4{0.0f, 0.0f, 0.0f, 1.0f});
	screen_transition_material->set_variable("tint"_fnv1a32, screen_transition_tint);
	screen_transition_material->set_blend_mode(render::material_blend_mode::translucent);

	// Screen transition billboard
	screen_transition_billboard = std::make_unique<scene::billboard>();
	screen_transition_billboard->set_material(screen_transition_material);
	screen_transition_billboard->set_scale({std::ceil(viewport_size.x() * 0.5f), std::ceil(viewport_size.y() * 0.5f), 1.0f});
	screen_transition_billboard->set_translation({std::floor(viewport_size.x() * 0.5f), std::floor(viewport_size.y() * 0.5f), 98.0f});
	screen_transition_billboard->set_layer_mask(0);

	// Construct menu bg entity
	menu_bg_entity = entity_registry->create();
	entity_registry->emplace<animation_component>(menu_bg_entity);

	// Construct menu bg fade in sequence
	{
		menu_bg_fade_in_sequence = std::make_shared<animation_sequence>();
		auto& opacity_track = menu_bg_fade_in_sequence->tracks()["opacity"];
		auto& opacity_channel = opacity_track.channels().emplace_back();
		opacity_channel.keyframes().emplace(0.0f, 0.0f);
		opacity_channel.keyframes().emplace(config::menu_fade_in_duration, config::menu_bg_opacity);

		opacity_track.output() = [menu_bg_tint](auto samples, auto&)
		{
			menu_bg_tint->set(math::fvec4{0.0f, 0.0f, 0.0f, samples[0]});
		};

		menu_bg_fade_in_sequence->cues().emplace(0.0f, [&](auto&)
		{
			ui_canvas->get_scene().add_object(*menu_bg_billboard);
		});
	}

	// Construct menu bg fade out sequence
	{
		menu_bg_fade_out_sequence = std::make_shared<animation_sequence>();
		auto& opacity_track = menu_bg_fade_out_sequence->tracks()["opacity"];
		auto& opacity_channel = opacity_track.channels().emplace_back();
		opacity_channel.keyframes().emplace(0.0f, config::menu_bg_opacity);
		opacity_channel.keyframes().emplace(config::menu_fade_out_duration, 0.0f);

		opacity_track.output() = [menu_bg_tint](auto samples, auto&)
		{
			menu_bg_tint->set(math::fvec4{0.0f, 0.0f, 0.0f, samples[0]});
		};

		menu_bg_fade_out_sequence->cues().emplace(1.0f, [&](auto&)
		{
			ui_canvas->get_scene().remove_object(*menu_bg_billboard);
		});
	}

	// Construct screen fade in sequence
	{
		screen_fade_in_sequence = std::make_shared<animation_sequence>();
		auto& opacity_track = screen_fade_in_sequence->tracks()["opacity"];
		auto& opacity_channel = opacity_track.channels().emplace_back();
		opacity_channel.keyframes().emplace(0.0f, 1.0f);
		opacity_channel.keyframes().emplace(1.0f, 0.0f);

		opacity_track.output() = [screen_transition_tint](auto samples, auto&)
		{
			screen_transition_tint->set(math::fvec4{0.0f, 0.0f, 0.0f, samples[0]});
		};
	}

	// Construct screen fade out sequence
	{
		screen_fade_out_sequence = std::make_shared<animation_sequence>();
		auto& opacity_track = screen_fade_out_sequence->tracks()["opacity"];
		auto& opacity_channel = opacity_track.channels().emplace_back();
		opacity_channel.keyframes().emplace(0.0f, 0.0f);
		opacity_channel.keyframes().emplace(1.0f, 1.0f);

		opacity_track.output() = [screen_transition_tint](auto samples, auto&)
		{
			screen_transition_tint->set(math::fvec4{0.0f, 0.0f, 0.0f, samples[0]});
		};
	}

	// Construct screen transition entity
	screen_transition_entity = entity_registry->create();
	entity_registry->emplace<animation_component>(screen_transition_entity);

	// Construct menu entity
	menu_entity = entity_registry->create();
	entity_registry->emplace<animation_component>(menu_entity);

	// Setup menu animations
	menu::setup_animations(*this);
	
	// Add UI scene objects to UI scene
	ui_canvas->get_scene().add_object(*ui_camera);
	ui_canvas->get_scene().add_object(*screen_transition_billboard);
	
	auto version_label = std::make_shared<ui::label>();
	version_label->set_font(debug_font);
	version_label->set_material(debug_font_material);
	version_label->set_text(std::format("v{}", config::application_version));
	version_label->set_anchors(0.0f, 0.0f, 1.0f, 1.0f);
	version_label->set_margin_left(50.0f);
	version_label->set_margin_bottom(50.0f);
	version_label->set_color(math::fvec4{1.0f, 1.0f, 1.0f, 1.0f});
	
	version_label->set_mouse_entered_callback
	(
		[](const auto& event)
		{
			static_cast<ui::label*>(event.element)->set_color(math::fvec4{1.0f, 0.0f, 1.0f, 1.0f});
		}
	);
	version_label->set_mouse_exited_callback
	(
		[](const auto& event)
		{
			static_cast<ui::label*>(event.element)->set_color(math::fvec4{1.0f, 1.0f, 1.0f, 1.0f});
		}
	);
	version_label->set_mouse_button_pressed_callback
	(
		[&]([[maybe_unused]] const auto& event)
		{
			// test_sound->play();
		}
	);
	
	//ui_canvas->add_child(version_label);
	
	
	
	// Setup window resized callback
	window_resized_subscription = window->get_resized_channel().subscribe
	(
		[&](const auto& event)
		{
			const auto& viewport_size = event.window->get_viewport_size();
			const float viewport_aspect_ratio = static_cast<float>(viewport_size.x()) / static_cast<float>(viewport_size.y());
			
			// Resize framebuffers
			::graphics::change_render_resolution(*this, render_scale);
			
			// Resize UI canvas
			ui_canvas->set_margins(0.0f, 0.0f, static_cast<float>(viewport_size.x()), static_cast<float>(viewport_size.y()));
			
			// Update camera projection matrix
			exterior_camera->set_aspect_ratio(viewport_aspect_ratio);
			
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

			// Resize screen transition billboard
			screen_transition_billboard->set_scale({std::ceil(viewport_size.x() * 0.5f), std::ceil(viewport_size.y() * 0.5f), 1.0f});
			screen_transition_billboard->set_translation({std::floor(viewport_size.x() * 0.5f), std::floor(viewport_size.y() * 0.5f), -100.0f});
			
			// Resize menu BG billboard
			menu_bg_billboard->set_scale({std::ceil(viewport_size.x() * 0.5f), std::ceil(viewport_size.y() * 0.5f), 1.0f});
			menu_bg_billboard->set_translation({std::floor(viewport_size.x() * 0.5f), std::floor(viewport_size.y() * 0.5f), -100.0f});
			
			// Re-align debug text
			frame_time_text->set_translation({std::round(0.0f), std::round(viewport_size.y() - debug_font->get_metrics().size), 99.0f});
			
			// Re-align menu text
			::menu::align_text(*this);
		}
	);
	
	// Setup mouse moved subscription
	mouse_moved_subscription = input_manager->get_event_dispatcher().subscribe<input::mouse_moved_event>
	(
		[&](const auto& event)
		{
			// Flip mouse Y-coordinate
			input::mouse_moved_event event_flipped_y = event;
			event_flipped_y.position.y() = window->get_size().y() - event.position.y() - 1;
			event_flipped_y.difference.y() = -event.difference.y();
			
			ui_canvas->handle_mouse_moved(event_flipped_y);
		}
	);
	
	// Setup mouse buttton pressed subscription
	mouse_button_pressed_subscription = input_manager->get_event_dispatcher().subscribe<input::mouse_button_pressed_event>
	(
		[&](const auto& event)
		{
			// Flip mouse Y-coordinate
			input::mouse_button_pressed_event event_flipped_y = event;
			event_flipped_y.position.y() = window->get_size().y() - event.position.y() - 1;
			
			ui_canvas->handle_mouse_button_pressed(event_flipped_y);
		}
	);
	
	// Setup mouse buttton released subscription
	mouse_button_released_subscription = input_manager->get_event_dispatcher().subscribe<input::mouse_button_released_event>
	(
		[&](const auto& event)
		{
			// Flip mouse Y-coordinate
			input::mouse_button_released_event event_flipped_y = event;
			event_flipped_y.position.y() = window->get_size().y() - event.position.y() - 1;
			
			ui_canvas->handle_mouse_button_released(event_flipped_y);
		}
	);
	
	debug::log_debug("Setting up UI... OK");
}

void game::setup_rng()
{
	debug::log_debug("Setting up RNG...");
	std::random_device rd;
	rng.seed(rd());
	debug::log_debug("Setting up RNG... OK");
}

void game::setup_entities()
{
	debug::log_debug("Setting up entities...");

	// Create entity registry
	entity_registry = std::make_unique<entt::registry>();

	debug::log_debug("Setting up entities... OK");
}

void game::setup_systems()
{
	debug::log_debug("Setting up systems...");

	const auto& viewport_size = window->get_viewport_size();
	math::fvec4 viewport = {0.0f, 0.0f, static_cast<float>(viewport_size[0]), static_cast<float>(viewport_size[1])};
	
	// Setup terrain system
	terrain_system = std::make_unique<::terrain_system>(*entity_registry);
	
	// Setup camera system
	camera_system = std::make_unique<::camera_system>(*entity_registry);
	camera_system->set_viewport(viewport);
	
	// Setup subterrain system
	subterrain_system = std::make_unique<::subterrain_system>(*entity_registry, resource_manager.get());
	
	// Setup collision system
	collision_system = std::make_unique<::collision_system>(*entity_registry);
	
	// Setup behavior system
	behavior_system = std::make_unique<::behavior_system>(*entity_registry);
	
	// Setup steering system
	steering_system = std::make_unique<::steering_system>(*entity_registry);
	
	// Setup locomotion system
	locomotion_system = std::make_unique<::locomotion_system>(*entity_registry);
	
	// Setup IK system
	ik_system = std::make_unique<::ik_system>(*entity_registry);
	
	// Setup metabolic system
	metabolic_system = std::make_unique<::metabolic_system>(*entity_registry);
	
	// Setup metamorphosis system
	metamorphosis_system = std::make_unique<::metamorphosis_system>(*entity_registry);
	
	// Setup animation system
	animation_system = std::make_unique<::animation_system>(*entity_registry);
	
	// Setup physics system
	physics_system = std::make_unique<::physics_system>(*entity_registry);
	physics_system->set_gravity({0.0f, -9.80665f * 100.0f, 0.0f});
	
	// Setup reproductive system
	reproductive_system = std::make_unique<::reproductive_system>(*entity_registry);
	reproductive_system->set_physics_system(physics_system.get());
	
	// Setup spatial system
	spatial_system = std::make_unique<::spatial_system>(*entity_registry);
	
	// Setup constraint system
	constraint_system = std::make_unique<::constraint_system>(*entity_registry);
	
	// Setup orbit system
	orbit_system = std::make_unique<::orbit_system>(*entity_registry);
	
	// Setup blackbody system
	blackbody_system = std::make_unique<::blackbody_system>(*entity_registry);
	
	// Setup atmosphere system
	atmosphere_system = std::make_unique<::atmosphere_system>(*entity_registry);
	atmosphere_system->set_sky_pass(sky_pass.get());
	
	// Setup astronomy system
	astronomy_system = std::make_unique<::astronomy_system>(*entity_registry);
	astronomy_system->set_transmittance_samples(16);
	astronomy_system->set_sky_pass(sky_pass.get());
	
	// Setup render system
	render_system = std::make_unique<::render_system>(*entity_registry);
	render_system->set_renderer(renderer.get());
	render_system->add_layer(exterior_scene.get());
	render_system->add_layer(interior_scene.get());
	render_system->add_layer(&ui_canvas->get_scene());


	debug::log_debug("Setting up systems... OK");
}

void game::setup_controls()
{
	debug::log_debug("Setting up controls...");
	
	// Load SDL game controller mappings database
	// debug::log_trace("Loading SDL game controller mappings...");
	// file_buffer* game_controller_db = resource_manager->load<file_buffer>("gamecontrollerdb.txt");
	// if (!game_controller_db)
	// {
		// debug::log_error("Failed to load SDL game controller mappings");
	// }
	// else
	// {
		// app->add_game_controller_mappings(game_controller_db->data(), game_controller_db->size());
		// debug::log_trace("Loaded SDL game controller mappings");
		
		// resource_manager->unload("gamecontrollerdb.txt");
	// }
	
	// Pass input event queue to action maps
	event::dispatcher* input_event_dispatcher = &input_manager->get_event_dispatcher();
	window_action_map.set_event_dispatcher(input_event_dispatcher);
	menu_action_map.set_event_dispatcher(input_event_dispatcher);
	movement_action_map.set_event_dispatcher(input_event_dispatcher);
	camera_action_map.set_event_dispatcher(input_event_dispatcher);
	ant_action_map.set_event_dispatcher(input_event_dispatcher);
	debug_action_map.set_event_dispatcher(input_event_dispatcher);
	terminal_action_map.set_event_dispatcher(input_event_dispatcher);
	
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
	mouse_pan_factor = mouse_radians_per_pixel * mouse_pan_sensitivity * (mouse_invert_pan ? -1.0 : 1.0);
	mouse_tilt_factor = mouse_radians_per_pixel * mouse_tilt_sensitivity * (mouse_invert_tilt ? -1.0 : 1.0);
	
	// Setup gamepad sensitivity
	gamepad_pan_factor = gamepad_radians_per_second * gamepad_pan_sensitivity * (gamepad_invert_pan ? -1.0 : 1.0);
	gamepad_tilt_factor = gamepad_radians_per_second * gamepad_tilt_sensitivity * (gamepad_invert_tilt ? -1.0 : 1.0);
	
	// Setup action callbacks
	setup_window_controls(*this);
	setup_menu_controls(*this);
	setup_camera_controls(*this);
	setup_game_controls(*this);
	setup_ant_controls(*this);
	setup_terminal_controls(*this);
	
	// Enable window controls
	enable_window_controls(*this);
	
	#if defined(DEBUG)
		// Setup and enable debug controls
		setup_debug_controls(*this);
		enable_debug_controls(*this);
	#endif
	
	debug::log_debug("Setting up controls... OK");
}

void game::setup_scripting()
{
	debug::log_debug("Setting up scripting...");

	load_global_module(script);
	load_math_module(script);
	load_event_module(script, input_manager->get_event_dispatcher(), script_event_subscriptions);

	debug::log_debug("Setting up scripting... OK");
}

void game::setup_debugging()
{
	debug::log_debug("Setting up debugging...");

	command_line_text = std::make_shared<scene::text>();
	command_line_text->set_material(debug_font_material);
	command_line_text->set_color({1.0f, 1.0f, 0.0f, 1.0f});
	command_line_text->set_font(debug_font);
	command_line_text->set_translation
	({
		std::round(debug_font->get_metrics().linespace),
		std::round(debug_font->get_metrics().linespace - debug_font->get_metrics().descent),
		99.0f
	});
	
	shell_buffer_text = std::make_shared<scene::text>();
	shell_buffer_text->set_font(debug_font);
	shell_buffer_text->set_material(debug_font_material);
	shell_buffer_text->set_color({1.0f, 1.0f, 0.0f, 1.0f});
	shell_buffer_text->set_translation({0.0f, 0.0f, 99.0f});
	
	shell_buffer = std::make_unique<::shell_buffer>();
	shell_buffer->set_text_object(shell_buffer_text);
	
	shell = std::make_unique<::shell>(this);
	shell->get_output().rdbuf(shell_buffer.get());

	load_io_module(script, shell->get_output());
	
	command_line_text->set_content(shell->prompt());
	
	const auto& viewport_size = window->get_viewport_size();
	
	frame_time_text = std::make_unique<scene::text>();
	frame_time_text->set_material(debug_font_material);
	frame_time_text->set_color({1.0f, 1.0f, 0.0f, 1.0f});
	frame_time_text->set_font(debug_font);
	frame_time_text->set_translation({std::round(0.0f), std::round(viewport_size.y() - debug_font->get_metrics().size), 99.0f});

	debug::log_debug("Setting up debugging... OK");
}

void game::setup_timing()
{
	debug::log_debug("Setting up timing...");

	// Init default settings
	max_frame_rate = window->get_display()->get_refresh_rate() * 2.0f;
	
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

	debug::log_debug("Setting up timing... OK");
}

void game::fixed_update(::frame_scheduler::duration_type fixed_update_time, ::frame_scheduler::duration_type fixed_update_interval)
{
	const float t = std::chrono::duration<float>(fixed_update_time).count();
	const float dt = std::chrono::duration<float>(fixed_update_interval).count();
	
	// Process window events
	window_manager->update();
	
	// Process function queue
	while (!function_queue.empty())
	{
		function_queue.front()();
		function_queue.pop();
	}
	
	// Update entity systems
	animation_system->update(t, dt);
	
	physics_system->update(t, dt);
	
	//terrain_system->update(t, dt);
	//subterrain_system->update(t, dt);
	collision_system->update(t, dt);
	behavior_system->update(t, dt);
	steering_system->update(t, dt);
	locomotion_system->update(t, dt);
	ik_system->update(t, dt);
	reproductive_system->update(t, dt);
	metabolic_system->update(t, dt);
	metamorphosis_system->update(t, dt);
	// physics_system->update(t, dt);
	orbit_system->update(t, dt);
	blackbody_system->update(t, dt);
	atmosphere_system->update(t, dt);
	astronomy_system->update(t, dt);
	spatial_system->update(t, dt);
	constraint_system->update(t, dt);
	camera_system->update(t, dt);
	render_system->update(t, dt);
}

void game::variable_update([[maybe_unused]] ::frame_scheduler::duration_type fixed_update_time, ::frame_scheduler::duration_type fixed_update_interval, ::frame_scheduler::duration_type accumulated_time)
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
	
	// Interpolate animation
	animation_system->interpolate(alpha);
	
	// Render
	camera_system->interpolate(alpha);
	render_system->draw(alpha);
	window->swap_buffers();
}

void game::execute()
{
	// Change to initial state
	state_machine.emplace(std::make_unique<main_menu_state>(*this, true));
	
	debug::log_debug("Entered main loop");
	
	frame_scheduler.refresh();
	
	while (!closed)
	{
		frame_scheduler.tick();
	}
	
	debug::log_debug("Exited main loop");
	
	// Exit all active game states
	while (!state_machine.empty())
	{
		state_machine.pop();
	}
}
