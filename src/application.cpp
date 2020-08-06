/*
 * Copyright (C) 2020  Christopher J. Howard
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
#include "configuration.hpp"
#include "state/application-states.hpp"
#include "filesystem.hpp"
#include "math.hpp"
#include "timestamp.hpp"

// STL
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <string>
#include <iomanip>

// External
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include "stb/stb_image_write.h"

// Debug
#include "debug/ansi-codes.hpp"
#include "debug/console-commands.hpp"

// Resources
#include "resources/resource-manager.hpp"

// Input
#include "input/sdl-scancode-table.hpp"
#include "input/sdl-game-controller-tables.hpp"
#include "input/scancode.hpp"
#include "input/input-mapping.hpp"

// Rasterizer
#include "rasterizer/rasterizer.hpp"
#include "rasterizer/framebuffer.hpp"
#include "rasterizer/texture-2d.hpp"
#include "rasterizer/pixel-type.hpp"
#include "rasterizer/pixel-format.hpp"
#include "rasterizer/vertex-buffer.hpp"
#include "rasterizer/vertex-array.hpp"
#include "rasterizer/vertex-attribute-type.hpp"
#include "rasterizer/texture-wrapping.hpp"
#include "rasterizer/texture-filter.hpp"

// Renderer
#include "renderer/simple-render-pass.hpp"
#include "renderer/passes/shadow-map-pass.hpp"
#include "renderer/passes/sky-pass.hpp"
#include "renderer/passes/clear-pass.hpp"
#include "renderer/passes/material-pass.hpp"
#include "renderer/passes/bloom-pass.hpp"
#include "renderer/passes/final-pass.hpp"
#include "renderer/vertex-attributes.hpp"
#include "renderer/material-flags.hpp"
#include "renderer/material-property.hpp"

// Animation
#include "animation/animation.hpp"
#include "animation/animator.hpp"

// Scene
#include "scene/billboard.hpp"
#include "scene/model-instance.hpp"

// Systems
#include "systems/behavior-system.hpp"
#include "systems/camera-system.hpp"
#include "systems/collision-system.hpp"
#include "systems/locomotion-system.hpp"
#include "systems/model-system.hpp"
#include "systems/nest-system.hpp"
#include "systems/placement-system.hpp"
#include "systems/samara-system.hpp"
#include "systems/subterrain-system.hpp"
#include "systems/terrain-system.hpp"
#include "systems/vegetation-system.hpp"
#include "systems/tool-system.hpp"
#include "systems/control-system.hpp"
#include "systems/ui-system.hpp"

// Entity components
#include "entity/components/cavity-component.hpp"

template <typename T>
inline T ease_linear(const T& x, const T& y, double a)
{
	return (y - x) * a + x;
}

template <typename T>
T ease_in_quad(const T& x, const T& y, double a)
{
	return (y - x) * a * a + x;
}

template <typename T>
T ease_out_quad(const T& x, const T& y, double a)
{
	return -(y - x) * a * (a - 2.0f) + x;
}

using namespace vmq::operators;

application::application(int argc, char** argv):
	closed(false),
	exit_status(EXIT_SUCCESS)
{
	// Format log messages
	logger.set_warning_prefix("Warning: ");
	logger.set_error_prefix(std::string());
	logger.set_success_prefix(std::string());
	
	// Redirect logger output
	#if defined(DEBUG)
		logger.redirect(&std::cout);
	#else
		std::string log_filename = "log.txt";
		log_filestream.open(log_filename.c_str());
		logger.redirect(&log_filestream);
	#endif

	// Determine application name
	std::string application_name;
	#if defined(_WIN32)
		application_name = "Antkeeper";
	#else
		application_name = "antkeeper";
	#endif
	
	// Form resource paths
	data_path = get_data_path(application_name) + "data/";
	config_path = get_config_path(application_name);
	logger.log("Detected data path as \"" + data_path + "\"\n");
	logger.log("Detected config path as \"" + config_path + "\"\n");

	screenshots_path = config_path + "screenshots/";

	// Create nonexistent config directories
	std::vector<std::string> config_paths;
	config_paths.push_back(config_path);
	config_paths.push_back(screenshots_path);
	for (const std::string& path: config_paths)
	{
		if (!path_exists(path))
		{
			logger.push_task("Creating directory \"" + path + "\"");
			if (create_directory(path))
			{
				logger.pop_task(EXIT_SUCCESS);
			}
			else
			{
				logger.pop_task(EXIT_FAILURE);
			}
		}
	}
	
	// Register CLI commands
	cli.register_command("echo", cc::echo);
	cli.register_command("exit", std::function<std::string()>(std::bind(&cc::exit, this)));
	cli.register_command("scrot", std::function<std::string()>(std::bind(&cc::scrot, this)));
	cli.register_command("cue", std::function<std::string(float, std::string)>(std::bind(&cc::cue, this, std::placeholders::_1, std::placeholders::_2)));
	//std::string cmd = "cue 20 exit";
	//logger.log(cmd + "\n");
	//logger.log(cli.interpret(cmd) + "\n");
	
	// Setup resource manager
	resource_manager = new ::resource_manager();
	resource_manager->set_logger(&logger);

	// Include resource search paths in order of priority
	resource_manager->include(config_path);
	resource_manager->include(data_path);
	resource_manager->include(data_path + "/shaders/include/");
	resource_manager->include(data_path + "/shaders/src/");
	resource_manager->include(data_path + "/models/");
	resource_manager->include(data_path + "/textures/");
	resource_manager->include(data_path + "/materials/");
	resource_manager->include(data_path + "/entities/");
	resource_manager->include(data_path + "/behaviors/");
	resource_manager->include(data_path + "/controls/");

	// Get SDL compiled version
	SDL_version sdl_compiled_version;
	SDL_VERSION(&sdl_compiled_version);
	std::string sdl_compiled_version_string = std::to_string(sdl_compiled_version.major) + "." + std::to_string(sdl_compiled_version.minor) + "." + std::to_string(sdl_compiled_version.patch);
	logger.log("Compiled against SDL " + sdl_compiled_version_string + "\n");

	// Get SDL linked version
	SDL_version sdl_linked_version;
	SDL_GetVersion(&sdl_linked_version);
	std::string sdl_linked_version_string = std::to_string(sdl_linked_version.major) + "." + std::to_string(sdl_linked_version.minor) + "." + std::to_string(sdl_linked_version.patch);
	logger.log("Linking against SDL " + sdl_linked_version_string + "\n");

	// Init SDL
	logger.push_task("Initializing SDL");
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		logger.pop_task(EXIT_FAILURE);
		throw std::runtime_error("Failed to initialize SDL");
	}
	else
	{
		logger.pop_task(EXIT_SUCCESS);
	}

	// Load default OpenGL library
	logger.push_task("Loading OpenGL library");
	if (SDL_GL_LoadLibrary(nullptr) != 0)
	{
		logger.pop_task(EXIT_FAILURE);
	}
	else
	{
		logger.pop_task(EXIT_SUCCESS);
	}

	// Set window creation hints
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_DisplayMode sdl_display_mode;
	if (SDL_GetDesktopDisplayMode(0, &sdl_display_mode) != 0)
	{
		logger.error("Failed to get desktop display mode: \"" + std::string(SDL_GetError()) + "\"\n");
	}
	else
	{
		logger.log("Detected " + std::to_string(sdl_display_mode.w) + "x" + std::to_string(sdl_display_mode.h) + " display\n");
		display_dimensions = {sdl_display_mode.w, sdl_display_mode.h};
	}

	int window_width = 1920;
	int window_height = 1080;
	fullscreen = true;
	
	window_width = 1280;
	window_height = 720;
	fullscreen = false;
	
	viewport = {0.0f, 0.0f, static_cast<float>(window_width), static_cast<float>(window_height)};
	
	int window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
	if (fullscreen)
	{
		window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}

	// Create window
	logger.push_task("Creating " + std::to_string(window_width) + "x" + std::to_string(window_height) + " window");
	window = SDL_CreateWindow
	(
		"Antkeeper", 
    	SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
	    window_width, window_height,
		window_flags
	);
	if (!window)
	{
		logger.pop_task(EXIT_FAILURE);
		throw std::runtime_error("Failed to create SDL window");
	}
	else
	{
		logger.pop_task(EXIT_SUCCESS);
	}

	// Create OpenGL context
	logger.push_task("Creating OpenGL 3.3 context");
	context = SDL_GL_CreateContext(window);
	if (!context)
	{
		logger.pop_task(EXIT_FAILURE);
		throw std::runtime_error("Failed to create OpenGL context");
	}
	else
	{
		logger.pop_task(EXIT_SUCCESS);
	}

	// Load OpenGL functions via GLAD
	logger.push_task("Loading OpenGL functions");
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		logger.pop_task(EXIT_FAILURE);
	}
	else
	{
		logger.pop_task(EXIT_SUCCESS);
	}

	// Set v-sync mode
	int swap_interval = 1;
	logger.push_task((swap_interval) ? "Enabling v-sync" : "Disabling v-sync");
	if (SDL_GL_SetSwapInterval(swap_interval) != 0)
	{
		logger.pop_task(EXIT_FAILURE);
	}
	else
	{
		logger.pop_task(EXIT_SUCCESS);
	}

	// Setup rasterizer
	rasterizer = new ::rasterizer();
	
	// Show window
	SDL_ShowWindow(window);

	// Clear window to black
	rasterizer->set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
	rasterizer->clear_framebuffer(true, false, false);
	SDL_GL_SwapWindow(window);

	// Hide cursor
	SDL_ShowCursor(SDL_DISABLE);

	// Init SDL joystick and game controller subsystems
	logger.push_task("Initializing SDL Joystick and Game Controller subsystems");
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0)
	{
		logger.pop_task(EXIT_FAILURE);
		throw std::runtime_error("Failed to initialize SDL Joystick or Game Controller subsystems");
	}
	else
	{
		logger.pop_task(EXIT_SUCCESS);
	}

	// Load SDL game controller mappings
	logger.push_task("Loading SDL game controller mappings from database");
	std::string gamecontrollerdb_path = data_path + "controls/gamecontrollerdb.txt";
	if (SDL_GameControllerAddMappingsFromFile(gamecontrollerdb_path.c_str()) == -1)
	{
		logger.pop_task(EXIT_FAILURE);
	}
	else
	{
		logger.pop_task(EXIT_SUCCESS);
	}
	
	// Setup billboard VAO
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

		billboard_vbo = new vertex_buffer(sizeof(float) * billboard_vertex_size * billboard_vertex_count, billboard_vertex_data);
		billboard_vao = new vertex_array();
		billboard_vao->bind_attribute(VERTEX_POSITION_LOCATION, *billboard_vbo, 3, vertex_attribute_type::float_32, billboard_vertex_stride, 0);
		billboard_vao->bind_attribute(VERTEX_TEXCOORD_LOCATION, *billboard_vbo, 2, vertex_attribute_type::float_32, billboard_vertex_stride, sizeof(float) * 3);
		billboard_vao->bind_attribute(VERTEX_BARYCENTRIC_LOCATION, *billboard_vbo, 3, vertex_attribute_type::float_32, billboard_vertex_stride, sizeof(float) * 5);
	}
	
	// Setup renderer
	renderer.set_billboard_vao(billboard_vao);
	
	// Load fallback material
	fallback_material = resource_manager->load<material>("fallback.mtl");
	
	// Create shadow map depth texture and framebuffer
	shadow_map_resolution = 4096;
	shadow_map_depth_texture = new texture_2d(shadow_map_resolution, shadow_map_resolution, pixel_type::float_32, pixel_format::d);
	shadow_map_depth_texture->set_wrapping(texture_wrapping::clamp, texture_wrapping::clamp);
	shadow_map_depth_texture->set_filters(texture_min_filter::linear, texture_mag_filter::linear);
	shadow_map_depth_texture->set_max_anisotropy(0.0f);
	shadow_map_framebuffer = new framebuffer(shadow_map_resolution, shadow_map_resolution);
	shadow_map_framebuffer->attach(framebuffer_attachment_type::depth, shadow_map_depth_texture);
	
	// Create HDR framebuffer (32F color, 32F depth)
	framebuffer_hdr_color = new texture_2d(window_width, window_height, pixel_type::float_32, pixel_format::rgb);
	framebuffer_hdr_color->set_wrapping(texture_wrapping::clamp, texture_wrapping::clamp);
	framebuffer_hdr_color->set_filters(texture_min_filter::linear, texture_mag_filter::linear);
	framebuffer_hdr_color->set_max_anisotropy(0.0f);
	framebuffer_hdr_depth = new texture_2d(window_width, window_height, pixel_type::float_32, pixel_format::d);
	framebuffer_hdr_depth->set_wrapping(texture_wrapping::clamp, texture_wrapping::clamp);
	framebuffer_hdr_depth->set_filters(texture_min_filter::linear, texture_mag_filter::linear);
	framebuffer_hdr_depth->set_max_anisotropy(0.0f);
	framebuffer_hdr = new framebuffer(window_width, window_height);
	framebuffer_hdr->attach(framebuffer_attachment_type::color, framebuffer_hdr_color);
	framebuffer_hdr->attach(framebuffer_attachment_type::depth, framebuffer_hdr_depth);
	
	// Create pingpong framebuffers (32F color, no depth)
	int bloom_width = window_width / 2;
	int bloom_height = window_height / 2;
	bloom_texture = new texture_2d(bloom_width, bloom_height, pixel_type::float_32, pixel_format::rgb);
	bloom_texture->set_wrapping(texture_wrapping::clamp, texture_wrapping::clamp);
	bloom_texture->set_filters(texture_min_filter::linear, texture_mag_filter::linear);
	bloom_texture->set_max_anisotropy(0.0f);
	framebuffer_bloom = new framebuffer(bloom_width, bloom_height);
	framebuffer_bloom->attach(framebuffer_attachment_type::color, bloom_texture);
	
	// Setup overworld passes
	shadow_map_clear_pass = new ::clear_pass(rasterizer, shadow_map_framebuffer);
	shadow_map_clear_pass->set_cleared_buffers(false, true, false);
	shadow_map_pass = new ::shadow_map_pass(rasterizer, shadow_map_framebuffer, resource_manager);
	shadow_map_pass->set_split_scheme_weight(0.75f);
	clear_pass = new ::clear_pass(rasterizer, framebuffer_hdr);
	clear_pass->set_cleared_buffers(true, true, false);
	sky_pass = new ::sky_pass(rasterizer, framebuffer_hdr, resource_manager);
	material_pass = new ::material_pass(rasterizer, framebuffer_hdr, resource_manager);
	material_pass->set_fallback_material(fallback_material);
	material_pass->set_time_tween(&time);
	material_pass->set_focal_point_tween(&focal_point_tween);
	material_pass->shadow_map_pass = shadow_map_pass;
	material_pass->shadow_map = shadow_map_depth_texture;
	bloom_pass = new ::bloom_pass(rasterizer, framebuffer_bloom, resource_manager);
	bloom_pass->set_source_texture(framebuffer_hdr_color);
	bloom_pass->set_brightness_threshold(1.0f);
	bloom_pass->set_blur_iterations(4);
	bloom_pass->set_enabled(false);
	final_pass = new ::final_pass(rasterizer, &rasterizer->get_default_framebuffer(), resource_manager);
	final_pass->set_color_texture(framebuffer_hdr_color);
	final_pass->set_bloom_texture(bloom_texture);
	
	// Setup overworld compositor
	overworld_compositor.add_pass(shadow_map_clear_pass);
	overworld_compositor.add_pass(shadow_map_pass);
	overworld_compositor.add_pass(clear_pass);
	overworld_compositor.add_pass(sky_pass);
	overworld_compositor.add_pass(material_pass);
	overworld_compositor.add_pass(bloom_pass);
	overworld_compositor.add_pass(final_pass);
	
	// Setup overworld camera
	overworld_camera.set_perspective(45.0f * vmq::pi<float> / 180.0f, (float)window_width / (float)window_height, 0.1f, 1000.0f);
	overworld_camera.set_compositor(&overworld_compositor);
	overworld_camera.set_composite_index(0);
	overworld_camera.set_active(true);
	
	// Setup underworld passes
	underworld_clear_pass = new ::clear_pass(rasterizer, framebuffer_hdr);
	underworld_clear_pass->set_cleared_buffers(true, true, false);
	
	
	underworld_material_pass = new ::material_pass(rasterizer, framebuffer_hdr, resource_manager);
	underworld_material_pass->set_fallback_material(fallback_material);
	underworld_material_pass->set_time_tween(&time);
	underworld_material_pass->set_focal_point_tween(&focal_point_tween);
	
	shader_program* underworld_final_shader = resource_manager->load<shader_program>("underground-final.glsl");
	underworld_final_pass = new simple_render_pass(rasterizer, &rasterizer->get_default_framebuffer(), underworld_final_shader);
	underworld_final_pass->set_time_tween(&time);
	underground_transition_property = underworld_final_pass->get_material()->add_property<float>("transition");
	underground_transition_property->set_value(0.0f);
	underground_color_texture_property = underworld_final_pass->get_material()->add_property<const texture_2d*>("color_texture");
	underground_color_texture_property->set_value(framebuffer_hdr_color);
	underworld_final_pass->get_material()->update_tweens();
	

	
	// Setup underworld compositor
	underworld_compositor.add_pass(underworld_clear_pass);
	underworld_compositor.add_pass(underworld_material_pass);
	underworld_compositor.add_pass(underworld_final_pass);
	
	// Setup underworld camera
	underworld_camera.set_perspective(45.0f * vmq::pi<float> / 180.0f, (float)window_width / (float)window_height, 0.1f, 1000.0f);
	underworld_camera.look_at({0, 50, 0}, {0, 0, 0}, {0, 0, -1});
	underworld_camera.set_compositor(&underworld_compositor);
	underworld_camera.set_composite_index(0);
	underworld_camera.set_active(false);
	
	// Setup timeline system
	timeline.set_autoremove(true);

	// Setup animation system
	// ...
	animator = new ::animator();
	
	float radial_transition_time = 0.5f;
	radial_transition_in = new animation<float>();
	radial_transition_in->set_frame_callback([this](int channel, float value){this->underground_transition_property->set_value(value);});	
	radial_transition_in->set_interpolator(ease_in_quad<float>);
	animation<float>::channel* channel = radial_transition_in->add_channel(0);
	channel->insert_keyframe({0.0f, 0.0f});
	channel->insert_keyframe({radial_transition_time, 1.0f});
	animator->add_animation(radial_transition_in);
	
	radial_transition_out = new animation<float>();	
	radial_transition_out->set_frame_callback([this](int channel, float value){this->underground_transition_property->set_value(value);});
	radial_transition_out->set_interpolator(ease_out_quad<float>);
	channel = radial_transition_out->add_channel(0);
	channel->insert_keyframe({0.0f, 1.0f});
	channel->insert_keyframe({radial_transition_time, 0.0f});
	animator->add_animation(radial_transition_out);
	
	// ECS
	terrain_system = new ::terrain_system(ecs_registry, resource_manager);
	terrain_system->set_patch_size(TERRAIN_PATCH_SIZE);
	vegetation_system = new ::vegetation_system(ecs_registry);
	vegetation_system->set_terrain_patch_size(TERRAIN_PATCH_SIZE);
	vegetation_system->set_vegetation_patch_resolution(VEGETATION_PATCH_RESOLUTION);
	vegetation_system->set_vegetation_density(1.0f);
	vegetation_system->set_vegetation_model(resource_manager->load<model>("grass-tuft.obj"));
	vegetation_system->set_scene(&overworld_scene);

	
	tool_system = new ::tool_system(ecs_registry);
	tool_system->set_camera(&overworld_camera);
	tool_system->set_orbit_cam(&orbit_cam);
	tool_system->set_viewport(viewport);
	camera_system = new ::camera_system(ecs_registry);
	camera_system->set_orbit_cam(&orbit_cam);
	camera_system->set_viewport(viewport);
	subterrain_system = new ::subterrain_system(ecs_registry, resource_manager);
	subterrain_system->set_scene(&underworld_scene);
	nest_system = new ::nest_system(ecs_registry, resource_manager);
	collision_system = new ::collision_system(ecs_registry);
	samara_system = new ::samara_system(ecs_registry);
	placement_system = new ::placement_system(ecs_registry);
	behavior_system = new ::behavior_system(ecs_registry);
	locomotion_system = new ::locomotion_system(ecs_registry);
	model_system = new ::model_system(ecs_registry, overworld_scene);

	// Setup systems
	systems.push_back([this](double t, double dt){ this->overworld_scene.update_tweens(); this->underworld_scene.update_tweens(); this->ui_system->get_scene()->update_tweens(); focal_point_tween.update(); this->underworld_final_pass->get_material()->update_tweens(); });
	systems.push_back([this](double t, double dt){ this->translate_sdl_events(); });
	systems.push_back([this](double t, double dt){ this->event_dispatcher.update(t); });
	systems.push_back([this](double t, double dt){ this->timeline.advance(dt); });
	systems.push_back(std::bind(&terrain_system::update, terrain_system, std::placeholders::_1, std::placeholders::_2));
	systems.push_back(std::bind(&vegetation_system::update, vegetation_system, std::placeholders::_1, std::placeholders::_2));
	systems.push_back(std::bind(&placement_system::update, placement_system, std::placeholders::_1, std::placeholders::_2));
	systems.push_back(std::bind(&nest_system::update, nest_system, std::placeholders::_1, std::placeholders::_2));
	systems.push_back(std::bind(&subterrain_system::update, subterrain_system, std::placeholders::_1, std::placeholders::_2));
	systems.push_back(std::bind(&collision_system::update, collision_system, std::placeholders::_1, std::placeholders::_2));
	systems.push_back(std::bind(&samara_system::update, samara_system, std::placeholders::_1, std::placeholders::_2));
	systems.push_back(std::bind(&camera_system::update, camera_system, std::placeholders::_1, std::placeholders::_2));
	systems.push_back(std::bind(&behavior_system::update, behavior_system, std::placeholders::_1, std::placeholders::_2));
	systems.push_back(std::bind(&locomotion_system::update, locomotion_system, std::placeholders::_1, std::placeholders::_2));
	systems.push_back([this](double t, double dt){ this->control_system->update(dt); });
	systems.push_back([this](double t, double dt){
		this->subterrain_light.set_translation(orbit_cam.get_focal_point());
		this->lantern.set_translation(orbit_cam.get_focal_point());
		this->spotlight.set_transform(overworld_camera.get_transform());
		this->focal_point_tween[1] = orbit_cam.get_focal_point();
	});

	systems.push_back([this](double t, double dt){ this->ui_system->update(dt); });
	systems.push_back(std::bind(&tool_system::update, tool_system, std::placeholders::_1, std::placeholders::_2));
	systems.push_back(std::bind(&model_system::update, model_system, std::placeholders::_1, std::placeholders::_2));
	systems.push_back([this](double t, double dt){ this->animator->animate(dt); });
	systems.push_back([this](double t, double dt){ this->application_controls.update(); this->menu_controls.update(); this->camera_controls->update(); });

	// Setup FSM states
	loading_state =
	{
		std::function<void()>(std::bind(enter_loading_state, this)),
		std::function<void()>(std::bind(exit_loading_state, this))
	};
	language_select_state =
	{
		std::function<void()>(std::bind(enter_language_select_state, this)),
		std::function<void()>(std::bind(exit_language_select_state, this))
	};
	splash_state =
	{
		std::function<void()>(std::bind(enter_splash_state, this)),
		std::function<void()>(std::bind(exit_splash_state, this))
	};
	title_state =
	{
		std::function<void()>(std::bind(enter_title_state, this)),
		std::function<void()>(std::bind(exit_title_state, this))
	};
	play_state =
	{
		std::function<void()>(std::bind(enter_play_state, this)),
		std::function<void()>(std::bind(exit_play_state, this))
	};
	pause_state =
	{
		std::function<void()>(std::bind(enter_pause_state, this)),
		std::function<void()>(std::bind(exit_pause_state, this))
	};

	// Setup frame timing
	frame_scheduler.set_update_callback(std::bind(&application::update, this, std::placeholders::_1, std::placeholders::_2));
	frame_scheduler.set_render_callback(std::bind(&application::render, this, std::placeholders::_1));
	frame_scheduler.set_update_rate(60.0);
	frame_scheduler.set_max_frame_duration(0.25);

	// Setup performance sampling
	performance_sampler.set_sample_size(15);

	// Setup input event routing
	input_event_router.set_event_dispatcher(&event_dispatcher);
	input_mapper.set_event_dispatcher(&event_dispatcher);
	
	// Setup input devices
	keyboard.set_event_dispatcher(&event_dispatcher);
	mouse.set_event_dispatcher(&event_dispatcher);
	game_controller.set_event_dispatcher(&event_dispatcher);

	// Setup controls
	application_controls.add_control(&toggle_fullscreen_control);
	application_controls.add_control(&dig_control);
	application_controls.add_control(&screenshot_control);
	toggle_fullscreen_control.set_activated_callback(std::bind(&application::toggle_fullscreen, this));
	screenshot_control.set_activated_callback([this]()
		{
			take_screenshot();
		});

	menu_back_control.set_activated_callback(std::bind(&application::close, this, 0));
	menu_controls.add_control(&menu_back_control);
	menu_controls.add_control(&menu_select_control);

	orbit_cam.attach(&overworld_camera);
	control_system = new ::control_system();
	control_system->set_orbit_cam(&orbit_cam);
	control_system->set_viewport(viewport);
	event_dispatcher.subscribe<mouse_moved_event>(control_system);
	event_dispatcher.subscribe<mouse_moved_event>(camera_system);
	event_dispatcher.subscribe<mouse_moved_event>(tool_system);

	camera_controls = control_system->get_control_set();

	// Application control mappings
	input_event_router.add_mapping(key_mapping(&toggle_fullscreen_control, nullptr, scancode::f11));
	input_event_router.add_mapping(key_mapping(&screenshot_control, nullptr, scancode::f12));
	
	// Add menu control mappings
	input_event_router.add_mapping(key_mapping(&menu_back_control, nullptr, scancode::escape));
	input_event_router.add_mapping(key_mapping(&menu_back_control, nullptr, scancode::backspace));
	input_event_router.add_mapping(game_controller_button_mapping(&menu_back_control, nullptr, game_controller_button::b));

	input_event_router.add_mapping(key_mapping(control_system->get_tool_menu_control(), nullptr, scancode::left_shift));
	input_event_router.add_mapping(game_controller_button_mapping(control_system->get_tool_menu_control(), nullptr, game_controller_button::x));
	
	

	input_event_router.add_mapping(key_mapping(&menu_select_control, nullptr, scancode::enter));
	input_event_router.add_mapping(key_mapping(&menu_select_control, nullptr, scancode::space));
	input_event_router.add_mapping(key_mapping(control_system->get_move_forward_control(), nullptr, scancode::w));
	
	input_event_router.add_mapping(key_mapping(control_system->get_toggle_view_control(), nullptr, scancode::tab));
	control_system->get_toggle_view_control()->set_activated_callback(
		[this]()
		{
			if (this->active_scene == &this->overworld_scene)
			{
				// Switch to underworld
				//this->overworld_camera.set_active(false);
				this->underworld_camera.set_active(true);
				this->active_scene = &this->underworld_scene;
				this->radial_transition_out->stop();
				this->radial_transition_in->play();
			}
			else
			{
				// Switch to overworld
				//this->underworld_camera.set_active(false);
				this->overworld_camera.set_active(true);
				this->active_scene = &this->overworld_scene;
				this->radial_transition_in->stop();
				this->radial_transition_out->play();
			}
		});
	
	input_event_router.add_mapping(game_controller_axis_mapping(control_system->get_move_forward_control(), nullptr, game_controller_axis::left_y, true));
	input_event_router.add_mapping(key_mapping(control_system->get_move_back_control(), nullptr, scancode::s));
	input_event_router.add_mapping(game_controller_axis_mapping(control_system->get_move_back_control(), nullptr, game_controller_axis::left_y, false));
	input_event_router.add_mapping(key_mapping(control_system->get_move_left_control(), nullptr, scancode::a));
	input_event_router.add_mapping(game_controller_axis_mapping(control_system->get_move_left_control(), nullptr, game_controller_axis::left_x, true));
	input_event_router.add_mapping(key_mapping(control_system->get_move_right_control(), nullptr, scancode::d));
	input_event_router.add_mapping(game_controller_axis_mapping(control_system->get_move_right_control(), nullptr, game_controller_axis::left_x, false));
	input_event_router.add_mapping(game_controller_axis_mapping(control_system->get_rotate_ccw_control(), nullptr, game_controller_axis::right_x, false));
	input_event_router.add_mapping(game_controller_axis_mapping(control_system->get_rotate_cw_control(), nullptr, game_controller_axis::right_x, true));
	input_event_router.add_mapping(game_controller_axis_mapping(control_system->get_tilt_up_control(), nullptr, game_controller_axis::right_y, false));
	input_event_router.add_mapping(game_controller_axis_mapping(control_system->get_tilt_down_control(), nullptr, game_controller_axis::right_y, true));
	input_event_router.add_mapping(mouse_wheel_mapping(control_system->get_zoom_in_control(), nullptr, mouse_wheel_axis::positive_y));
	input_event_router.add_mapping(mouse_wheel_mapping(control_system->get_zoom_out_control(), nullptr, mouse_wheel_axis::negative_y));
	input_event_router.add_mapping(mouse_button_mapping(control_system->get_adjust_camera_control(), nullptr, 3));
	input_event_router.add_mapping(game_controller_button_mapping(control_system->get_ascend_control(), nullptr, game_controller_button::y));
	input_event_router.add_mapping(game_controller_button_mapping(control_system->get_descend_control(), nullptr, game_controller_button::a));
	input_event_router.add_mapping(game_controller_axis_mapping(control_system->get_zoom_out_control(), nullptr, game_controller_axis::trigger_left, false));
	input_event_router.add_mapping(game_controller_axis_mapping(control_system->get_zoom_in_control(), nullptr, game_controller_axis::trigger_right, false));

	control_system->get_adjust_camera_control()->set_activated_callback([this](){ this->set_relative_mouse_mode(true); this->tool_system->set_pick(false); });
	control_system->get_adjust_camera_control()->set_deactivated_callback([this](){ this->set_relative_mouse_mode(false); this->tool_system->set_pick(true); });



	input_event_router.add_mapping(key_mapping(&dig_control, nullptr, scancode::one));
	dig_control.set_activated_callback(
		[this]()
		{
			const float r = 25.0f;

			ecs::cavity_component cavity;
			cavity.position =
			{
				frand(-r, r),
				frand(-r * 2, r),
				frand(-r, r)
			};
			cavity.radius = frand(0.75f, 1.25f);

			ecs_registry.assign<ecs::cavity_component>(ecs_registry.create(), cavity);
		});

	pheromones.rows = 256;
	pheromones.columns = 256;
	pheromones.buffers = new float*[2];
	pheromones.buffers[0] = new float[pheromones.rows * pheromones.columns];
	pheromones.buffers[1] = new float[pheromones.rows * pheromones.columns];
	pheromones.current = 0;
	//diffuse(&pheromones);
	

	control_system->set_tool(nullptr);
	
	// Setup UI
	ui_system = new ::ui_system(resource_manager);
	ui_system->set_viewport(viewport);
	ui_system->set_tool_menu_control(control_system->get_tool_menu_control());
	event_dispatcher.subscribe<mouse_moved_event>(ui_system);
	
	// Setup UI camera compositor
	ui_clear_pass = new ::clear_pass(rasterizer, &rasterizer->get_default_framebuffer());
	ui_clear_pass->set_cleared_buffers(false, true, false);
	ui_material_pass = new ::material_pass(rasterizer, &rasterizer->get_default_framebuffer(), resource_manager);
	ui_material_pass->set_fallback_material(fallback_material);
	ui_material_pass->set_time_tween(&time);
	ui_compositor.add_pass(ui_clear_pass);
	ui_compositor.add_pass(ui_material_pass);
	ui_system->get_camera()->set_compositor(&ui_compositor);

	// Setup lights
	sun_indirect.set_intensity(0.25f);
	sun_indirect.update_tweens();
	sun_direct.look_at({-1.0f, 5.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
	sun_direct.set_intensity(1.0f);
	sun_direct.update_tweens();
	subterrain_light.set_color({1, 1, 1});
	subterrain_light.set_intensity(1.0f);
	subterrain_light.set_attenuation({1.0f, 0.09f, 0.032f});
	subterrain_light.update_tweens();
	spotlight.set_color({1, 1, 1});
	spotlight.set_intensity(1.0f);
	spotlight.set_attenuation({1.0f, 0.09f, 0.032f});
	spotlight.set_cutoff({vmq::radians(15.0f), vmq::radians(30.0f)});
	spotlight.update_tweens();
	spotlight.set_active(false);
	
	underworld_ambient_light.set_color({1, 1, 1});
	underworld_ambient_light.set_intensity(0.15f);
	underworld_ambient_light.update_tweens();
	
	// Darkness
	darkness_volume.set_model(resource_manager->load<model>("darkness-volume.obj"));
	lantern.set_model(resource_manager->load<model>("lantern.obj"));
	
	// Cloud
	cloud.set_model(resource_manager->load<model>("cloud.obj"));
	cloud.set_translation({0, 0, 4500});
	cloud.set_scale(float3{1, 1, 1} * 100.0f);
	
	
	// Create depth debug billboard
	/*
	material* depth_debug_material = new material();
	depth_debug_material->set_shader_program(resource_manager->load<shader_program>("ui-element-textured.glsl"));
	depth_debug_material->add_property<const texture_2d*>("background")->set_value(shadow_map_depth_texture);
	depth_debug_material->add_property<float4>("tint")->set_value(float4{1, 1, 1, 1});
	billboard* depth_debug_billboard = new billboard();
	depth_debug_billboard->set_material(depth_debug_material);
	depth_debug_billboard->set_scale({128, 128, 1});
	depth_debug_billboard->set_translation({-960 + 128, 1080 * 0.5f - 128, 0});
	depth_debug_billboard->update_tweens();
	ui_system->get_scene()->add_object(depth_debug_billboard);
	*/
	
	material* billboard_material = new material();
	billboard_material->set_shader_program(resource_manager->load<shader_program>("ui-element-textured.glsl"));
	billboard_material->add_property<const texture_2d*>("background")->set_value(resource_manager->load<texture_2d>("arrow.png"));
	billboard_material->add_property<float4>("tint")->set_value(float4{1, 1, 1, 1});
	billboard_material->set_flags(MATERIAL_FLAG_TRANSLUCENT | MATERIAL_FLAG_NOT_SHADOW_CASTER);
	billboard* arrow_billboard = new billboard();
	arrow_billboard->set_material(billboard_material);
	arrow_billboard->set_scale(float3{1, 1, 1} * 2.0f);
	arrow_billboard->set_translation({0, 10, 0});
	arrow_billboard->set_billboard_type(billboard_type::cylindrical);
	arrow_billboard->set_alignment_axis({0, 1, 0});
	arrow_billboard->update_tweens();
	
	
	billboard_material = new material();
	billboard_material->set_shader_program(resource_manager->load<shader_program>("portal-card.glsl"));
	billboard_material->add_property<float4>("color")->set_value(float4{1, 1, 1, 1});
	billboard_material->add_property<float2>("range")->set_value(float2{50.0f, 500.0f});
	billboard_material->set_flags(MATERIAL_FLAG_TRANSLUCENT | MATERIAL_FLAG_NOT_SHADOW_CASTER);
	billboard* portal_billboard = new billboard();
	portal_billboard->set_material(billboard_material);
	portal_billboard->set_scale(float3{1, 1, 1} * 10.0f);
	portal_billboard->set_translation({0.0f, 0, 0});
	portal_billboard->set_billboard_type(billboard_type::spherical);
	portal_billboard->set_alignment_axis({0, 1, 0});
	portal_billboard->update_tweens();
	

	// Setup overworld scene
	overworld_scene.add_object(&overworld_camera);
	overworld_scene.add_object(&sun_indirect);
	overworld_scene.add_object(&sun_direct);
	overworld_scene.add_object(&spotlight);
	overworld_scene.add_object(&cloud);
	overworld_scene.add_object(arrow_billboard);
	
	// Setup underworld scene
	underworld_scene.add_object(&underworld_camera);
	underworld_scene.add_object(&underworld_ambient_light);
	//underworld_scene.add_object(&darkness_volume);
	underworld_scene.add_object(&lantern);
	underworld_scene.add_object(&subterrain_light);
	underworld_scene.add_object(portal_billboard);
	//model_instance* larva = new model_instance(resource_manager->load<model>("larva.obj"));
	//underworld_scene.add_object(larva);
	model_instance* flashlight = new model_instance(resource_manager->load<model>("flashlight.obj"));
	underworld_scene.add_object(flashlight);

	// Set overworld as active scene
	active_scene = &overworld_scene;
}

application::~application()
{
	// Destroy the SDL window
	SDL_DestroyWindow(window);

	// Shutdown SDL
	SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
	SDL_Quit();
}

void application::close(int status)
{
	closed = true;
	exit_status = status;
}

int application::execute()
{
	// Enter inital state
	state_machine.change_state(play_state);

	// Perform initial update
	update(0.0, 0.0);

	// Reset frame scheduler
	frame_scheduler.reset();

	// Reset time tween
	time[0] = time[1] = 0.0;

	// Schedule frames until closed
	while (!closed)
	{
		// Tick frame scheduler
		frame_scheduler.tick();

		// Sample frame duration
		performance_sampler.sample(frame_scheduler.get_frame_duration());
	}

	// Exit current state
	state_machine.change_state({nullptr, nullptr});

	return exit_status;
}

void application::update(double t, double dt)
{
	// Update time tween
	time.update();
	time[1] = t;

	// Sequentially process systems
	for (const auto& system: systems)
	{
		system(t, dt);
	}
}

void application::render(double alpha)
{
	/*
	std::cout << std::fixed;
	std::cout << std::setprecision(2);
	std::cout << performance_sampler.mean_frame_duration() * 1000.0 << std::endl;
	*/
	
	renderer.render(alpha, overworld_scene);
	renderer.render(alpha, underworld_scene);
	//renderer.render(alpha, *active_scene);
	renderer.render(alpha, *ui_system->get_scene());

	SDL_GL_SwapWindow(window);
}

void application::translate_sdl_events()
{
	SDL_Event sdl_event;
	while (SDL_PollEvent(&sdl_event))
	{
		switch (sdl_event.type)
		{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				if (sdl_event.key.repeat == 0)
				{
					scancode scancode = scancode::unknown;
					if (sdl_event.key.keysym.scancode <= SDL_SCANCODE_APP2)
					{
						scancode = sdl_scancode_table[sdl_event.key.keysym.scancode];
					}

					if (sdl_event.type == SDL_KEYDOWN)
						keyboard.press(scancode);
					else
						keyboard.release(scancode);
				}
				break;
			}

			case SDL_MOUSEMOTION:
			{
				mouse.move(sdl_event.motion.x, sdl_event.motion.y, sdl_event.motion.xrel, sdl_event.motion.yrel);
				break;
			}

			case SDL_MOUSEBUTTONDOWN:
			{
				mouse.press(sdl_event.button.button, sdl_event.button.x, sdl_event.button.y);
				break;
			}
			
			case SDL_MOUSEBUTTONUP:
			{
				mouse.release(sdl_event.button.button, sdl_event.button.x, sdl_event.button.y);
				break;
			}
			
			case SDL_MOUSEWHEEL:
			{
				int direction = (sdl_event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) ? -1 : 1;
				mouse.scroll(sdl_event.wheel.x * direction, sdl_event.wheel.y * direction);
				break;
			}

			case SDL_CONTROLLERBUTTONDOWN:
			{
				if (sdl_event.cbutton.button != SDL_CONTROLLER_BUTTON_INVALID)
				{
					game_controller_button button = sdl_button_table[sdl_event.cbutton.button];
					game_controller.press(button);
				}
				break;
			}

			case SDL_CONTROLLERBUTTONUP:
			{
				if (sdl_event.cbutton.button != SDL_CONTROLLER_BUTTON_INVALID)
				{
					game_controller_button button = sdl_button_table[sdl_event.cbutton.button];
					game_controller.release(button);
				}
				break;
			}

			case SDL_CONTROLLERAXISMOTION:
			{
				if (sdl_event.caxis.axis != SDL_CONTROLLER_AXIS_INVALID)
				{
					game_controller_axis axis = sdl_axis_table[sdl_event.caxis.axis];
					float value = sdl_event.caxis.value;
					value /= (value < 0.0f) ? 32768.0f : 32767.0f;
					game_controller.move(axis, value);
				}
				break;
			}

			case SDL_CONTROLLERDEVICEADDED:
			{
				if (SDL_IsGameController(sdl_event.cdevice.which))
				{
					SDL_GameController* sdl_controller = SDL_GameControllerOpen(sdl_event.cdevice.which);

					std::string controller_name = SDL_GameControllerNameForIndex(sdl_event.cdevice.which);
					if (sdl_controller)
					{
						logger.log("Connected game controller \"" + controller_name + "\"\n");
					}
					else
					{
						logger.error("Failed to connected game controller \"" + controller_name + "\"\n");
					}
				}

				break;
			}

			case SDL_CONTROLLERDEVICEREMOVED:
			{
				SDL_GameController* sdl_controller = SDL_GameControllerFromInstanceID(sdl_event.cdevice.which);

				if (sdl_controller)
				{
					SDL_GameControllerClose(sdl_controller);
					logger.log("Disconnected game controller\n");
				}

				break;
			}

			case SDL_WINDOWEVENT:
			{
				if (sdl_event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					window_resized();
				}

				break;
			}

			case SDL_QUIT:
			{
				close(EXIT_SUCCESS);
				break;
			}
		}
	}
}

void application::set_relative_mouse_mode(bool enabled)
{
	if (enabled)
	{
		SDL_GetMouseState(&std::get<0>(saved_mouse_position), &std::get<1>(saved_mouse_position));
	}

	SDL_SetRelativeMouseMode((enabled) ? SDL_TRUE : SDL_FALSE);

	if (!enabled)
	{
		SDL_WarpMouseInWindow(window, std::get<0>(saved_mouse_position), std::get<1>(saved_mouse_position));
	}
}

void application::toggle_fullscreen()
{
	fullscreen = !fullscreen;

	if (fullscreen)
	{
		SDL_GetWindowSize(window, &std::get<0>(window_dimensions), &std::get<1>(window_dimensions));
		SDL_GetWindowPosition(window, &std::get<0>(window_position), &std::get<1>(window_position));

		SDL_SetWindowBordered(window, SDL_FALSE);
		SDL_SetWindowResizable(window, SDL_FALSE);
		SDL_SetWindowPosition(window, 0, 0);
		SDL_SetWindowSize(window, std::get<0>(display_dimensions), std::get<1>(display_dimensions));
	}
	else
	{
		SDL_SetWindowBordered(window, SDL_TRUE);
		SDL_SetWindowResizable(window, SDL_TRUE);
		SDL_SetWindowSize(window, std::get<0>(window_dimensions), std::get<1>(window_dimensions));
		SDL_SetWindowPosition(window, std::get<0>(window_position), std::get<1>(window_position));
	}

	window_resized();
}

void application::window_resized()
{
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
	viewport = {0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)};

	rasterizer->window_resized(width, height);
	overworld_camera.set_perspective(overworld_camera.get_fov(), aspect_ratio, overworld_camera.get_clip_near(), overworld_camera.get_clip_far());
	underworld_camera.set_perspective(underworld_camera.get_fov(), aspect_ratio, underworld_camera.get_clip_near(), underworld_camera.get_clip_far());
	control_system->set_viewport(viewport);
	camera_system->set_viewport(viewport);
	tool_system->set_viewport(viewport);
	ui_system->set_viewport(viewport);
}

void application::take_screenshot() const
{
	int x = viewport[0];
	int y = viewport[1];
	int w = viewport[2];
	int h = viewport[3];

	// Read pixel data from framebuffer
	unsigned char* pixels = new unsigned char[w * h * 3];
	glReadBuffer(GL_BACK);
	glReadPixels(x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	std::string filename = screenshots_path + "antkeeper-" + timestamp() + ".png";
	std::thread screenshot_thread(application::save_image, filename, w, h, pixels);
	screenshot_thread.detach();
}

void application::save_image(const std::string& filename, int w, int h, const unsigned char* pixels)
{
	stbi_flip_vertically_on_write(1);
	stbi_write_png(filename.c_str(), w, h, 3, pixels, w * 3);
	delete[] pixels;
}

