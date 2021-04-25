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

#ifndef ANTKEEPER_GAME_CONTEXT_HPP
#define ANTKEEPER_GAME_CONTEXT_HPP

#include "utility/fundamental-types.hpp"
#include "resources/string-table.hpp"
#include "ecs/entity.hpp"
#include "ecs/registry.hpp"
#include "geom/aabb.hpp"
#include "gl/vertex-array.hpp"
#include "gl/vertex-buffer.hpp"
#include "gl/texture-2d.hpp"
#include "gl/rasterizer.hpp"
#include "gl/framebuffer.hpp"
#include "input/control.hpp"
#include "input/control-set.hpp"
#include "input/listener.hpp"
#include "input/mapper.hpp"
#include "input/event-router.hpp"
#include <optional>
#include <entt/entt.hpp>
#include <fstream>
#include <string>

// Forward declarations
class animator;
class application;
class bloom_pass;
class clear_pass;
class compositor;
class config_file;
class final_pass;
class material;
class material_pass;
class orbit_cam;
class pheromone_matrix;
class resource_manager;
class screen_transition;
class shadow_map_pass;
class simple_render_pass;
class sky_pass;
class timeline;
class renderer;
class outline_pass;

struct biome;
template <typename T> class animation;
template <typename T> class material_property;
template <typename T> class tween;

namespace debug
{
	class cli;
	class logger;
}

namespace ecs
{
	class subterrain_system;
	class terrain_system;
	class vegetation_system;
	class tool_system;
	class ui_system;
	class spatial_system;
	class tracking_system;
	class painting_system;
	class weather_system;
	class astronomy_system;
	class solar_system;
	class behavior_system;
	class collision_system;
	class constraint_system;
	class locomotion_system;
	class control_system;
	class snapping_system;
	class camera_system;
	class nest_system;
	class render_system;
	class samara_system;
}

#include "scene/scene.hpp"

/**
 * 
 */
struct game_context
{
	application* app;
	debug::logger* logger;
	std::ofstream log_filestream;
	
	// Command-line options
	std::optional<std::string> option_biome;
	std::optional<bool> option_continue;
	std::optional<std::string> option_data;
	std::optional<bool> option_fullscreen;
	std::optional<bool> option_new_game;
	std::optional<bool> option_quick_start;
	std::optional<bool> option_reset;
	std::optional<int> option_vsync;
	std::optional<bool> option_windowed;
	
	// Paths
	std::string data_path;
	std::string config_path;
	std::string mods_path;
	std::string saves_path;
	std::string screenshots_path;
	std::string data_package_path;
	
	// Config
	config_file* config;
	
	// Resources
	resource_manager* resource_manager;
	
	// Localization
	std::string language_code;
	int language_index;
	string_table* string_table;
	string_table_map string_table_map;
	std::unordered_map<std::string, std::string>* strings;
	
	// Framebuffers
	gl::framebuffer* shadow_map_framebuffer;
	gl::texture_2d* shadow_map_depth_texture;
	gl::framebuffer* framebuffer_hdr;
	gl::texture_2d* framebuffer_hdr_color;
	gl::texture_2d* framebuffer_hdr_depth;
	gl::framebuffer* framebuffer_bloom; // General purpose framebuffer A
	gl::texture_2d* bloom_texture;
	
	// Rendering
	gl::rasterizer* rasterizer;
	renderer* renderer;
	gl::vertex_buffer* billboard_vbo;
	gl::vertex_array* billboard_vao;
	material* fallback_material;
	material* splash_billboard_material;
	gl::texture_2d** marker_albedo_textures;
	
	// Compositing
	bloom_pass* overworld_bloom_pass;
	clear_pass* overworld_clear_pass;
	clear_pass* overworld_shadow_map_clear_pass;
	clear_pass* ui_clear_pass;
	clear_pass* underworld_clear_pass;
	final_pass* overworld_final_pass;
	material_pass* overworld_material_pass;
	material_pass* ui_material_pass;
	material_pass* underworld_material_pass;
	outline_pass* overworld_outline_pass;
	shadow_map_pass* overworld_shadow_map_pass;
	simple_render_pass* underworld_final_pass;
	sky_pass* overworld_sky_pass;
	material_property<const gl::texture_2d*>* underground_color_texture_property;
	compositor* overworld_compositor;
	compositor* underworld_compositor;
	compositor* ui_compositor;
	
	// Scene
	scene::collection* active_scene;
	scene::collection* overworld_scene;
	scene::collection* underworld_scene;
	scene::collection* ui_scene;
	scene::camera* overworld_camera;
	scene::camera* underworld_camera;
	scene::camera* ui_camera;
	scene::directional_light* moon_light;
	scene::point_light* subterrain_light;
	scene::ambient_light* underworld_ambient_light;
	scene::billboard* splash_billboard;
	scene::spot_light* lens_spot_light;
	scene::spot_light* flashlight_spot_light;
	geom::aabb<float> no_cull;
	
	// Animation
	timeline* timeline;
	animator* animator;
	tween<double>* time_tween;
	tween<float3>* focal_point_tween;
	animation<float>* radial_transition_in;
	animation<float>* radial_transition_out;
	screen_transition* fade_transition;
	screen_transition* radial_transition_inner;
	screen_transition* radial_transition_outer;
	animation<float>* equip_tool_animation;
	animation<float>* unequip_tool_animation;

	// Controls
	input::event_router* input_event_router;
	input::mapper* input_mapper;
	input::listener* input_listener;
	input::control_set* application_controls;
	input::control_set* camera_controls;
	input::control_set* menu_controls;
	input::control* menu_back_control;
	input::control* menu_select_control;
	input::control* screenshot_control;
	input::control* toggle_fullscreen_control;

	// Entities
	ecs::registry* ecs_registry;
	ecs::entity brush_entity;
	ecs::entity flashlight_entity;
	ecs::entity forceps_entity;
	ecs::entity lens_entity;
	ecs::entity marker_entity;
	ecs::entity container_entity;
	ecs::entity twig_entity;
	ecs::entity focal_point_entity;

	// Systems
	ecs::behavior_system* behavior_system;
	ecs::camera_system* camera_system;
	ecs::collision_system* collision_system;
	ecs::constraint_system* constraint_system;
	ecs::control_system* control_system;
	ecs::locomotion_system* locomotion_system;
	ecs::nest_system* nest_system;
	ecs::snapping_system* snapping_system;
	ecs::render_system* render_system;
	ecs::samara_system* samara_system;
	ecs::subterrain_system* subterrain_system;
	ecs::terrain_system* terrain_system;
	ecs::tool_system* tool_system;
	ecs::ui_system* ui_system;
	ecs::vegetation_system* vegetation_system;
	ecs::spatial_system* spatial_system;
	ecs::tracking_system* tracking_system;
	ecs::painting_system* painting_system;
	ecs::weather_system* weather_system;
	ecs::astronomy_system* astronomy_system;
	ecs::solar_system* solar_system;
	
	// Game
	biome* biome;
	
	// Debug
	debug::cli* cli;
	
	// Misc
	pheromone_matrix* pheromones;
};

#endif // ANTKEEPER_GAME_CONTEXT_HPP
