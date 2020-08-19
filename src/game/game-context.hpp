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

#ifndef ANTKEEPER_GAME_CONTEXT_HPP
#define ANTKEEPER_GAME_CONTEXT_HPP

#include "utility/fundamental-types.hpp"
#include "resources/string-table.hpp"
#include <optional>
#include <entt/entt.hpp>
#include <fstream>
#include <string>

// Forward declarations
class ambient_light;
class animator;
class application;
class behavior_system;
class billboard;
class bloom_pass;
class camera;
class camera_system;
class clear_pass;
class collision_system;
class compositor;
class config_file;
class constraint_system;
class control;
class control_set;
class control_system;
class directional_light;
class final_pass;
class framebuffer;
class locomotion_system;
class logger;
class material;
class material_pass;
class nest_system;
class orbit_cam;
class pheromone_matrix;
class snapping_system;
class point_light;
class rasterizer;
class render_system;
class resource_manager;
class samara_system;
class scene;
class screen_transition;
class shadow_map_pass;
class simple_render_pass;
class sky_pass;
class spotlight;
class subterrain_system;
class terrain_system;
class texture_2d;
class timeline;
class tool_system;
class ui_system;
class vegetation_system;
class vertex_array;
class vertex_buffer;
class renderer;
class model_instance;
class input_event_router;
class input_mapper;
class cli;
template <typename T> class animation;
template <typename T> class material_property;
template <typename T> class tween;

/**
 * 
 */
struct game_context
{
	application* app;
	logger* logger;
	std::ofstream log_filestream;
	
	// Command-line options
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
	framebuffer* shadow_map_framebuffer;
	texture_2d* shadow_map_depth_texture;
	framebuffer* framebuffer_hdr;
	texture_2d* framebuffer_hdr_color;
	texture_2d* framebuffer_hdr_depth;
	framebuffer* framebuffer_bloom; // General purpose framebuffer A
	texture_2d* bloom_texture;
	
	// Rendering
	rasterizer* rasterizer;
	renderer* renderer;
	vertex_buffer* billboard_vbo;
	vertex_array* billboard_vao;
	material* fallback_material;
	material* splash_billboard_material;
	
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
	shadow_map_pass* overworld_shadow_map_pass;
	simple_render_pass* underworld_final_pass;
	sky_pass* overworld_sky_pass;
	material_property<const texture_2d*>* underground_color_texture_property;
	compositor* overworld_compositor;
	compositor* underworld_compositor;
	compositor* ui_compositor;
	
	// Scene
	scene* active_scene;
	scene* overworld_scene;
	scene* underworld_scene;
	scene* ui_scene;
	camera* overworld_camera;
	camera* underworld_camera;
	camera* ui_camera;
	ambient_light* sun_indirect;
	directional_light* sun_direct;
	point_light* subterrain_light;
	ambient_light* underworld_ambient_light;
	spotlight* spotlight;
	billboard* splash_billboard;
	
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

	// Controls
	input_event_router* input_event_router;
	input_mapper* input_mapper;
	control_set* application_controls;
	control_set* camera_controls;
	control_set* menu_controls;
	control* menu_back_control;
	control* menu_select_control;
	control* screenshot_control;
	control* rotate_ccw_control;
	control* rotate_cw_control;
	control* toggle_fullscreen_control;

	// Entities
	entt::registry* ecs_registry;
	entt::entity brush_entity;
	entt::entity flashlight_entity;
	entt::entity forceps_entity;
	entt::entity lens_entity;
	entt::entity focal_point_entity;

	// Systems
	behavior_system* behavior_system;
	camera_system* camera_system;
	collision_system* collision_system;
	constraint_system* constraint_system;
	control_system* control_system;
	locomotion_system* locomotion_system;
	nest_system* nest_system;
	snapping_system* snapping_system;
	render_system* render_system;
	samara_system* samara_system;
	subterrain_system* subterrain_system;
	terrain_system* terrain_system;
	tool_system* tool_system;
	ui_system* ui_system;
	vegetation_system* vegetation_system;
	
	// Debug
	cli* cli;
	
	// Misc
	orbit_cam* orbit_cam;
	pheromone_matrix* pheromones;
};

#endif // ANTKEEPER_GAME_CONTEXT_HPP
