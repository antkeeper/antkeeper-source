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
#include "entity/id.hpp"
#include "entity/registry.hpp"
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
#include "animation/tween.hpp"
#include "scene/scene.hpp"
#include <optional>
#include <entt/entt.hpp>
#include <fstream>
#include <queue>
#include <string>
#include <unordered_map>
#include "resources/json.hpp"
#include "type/typeface.hpp"

// Forward declarations
class animator;
class application;
class bloom_pass;
class clear_pass;
class compositor;
class final_pass;
class material;
class material_pass;
class resource_manager;
class screen_transition;
class shadow_map_pass;
class simple_render_pass;
class sky_pass;
class timeline;
class renderer;
class outline_pass;

template <typename T> class animation;
template <typename T> class material_property;

namespace debug
{
	class cli;
	class logger;
}

namespace entity
{
	namespace system
	{
		class subterrain;
		class terrain;
		class vegetation;
		class ui;
		class spatial;
		class painting;
		class astronomy;
		class blackbody;
		class atmosphere;
		class orbit;
		class behavior;
		class collision;
		class constraint;
		class locomotion;
		class snapping;
		class camera;
		class nest;
		class render;
		class samara;
		class proteome;
	}
}

namespace game {

/// Structure containing the state of a game.
struct context
{
	application* app;
	debug::logger* logger;
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
	std::string controls_path;
	std::string data_package_path;
	
	// Configuration
	json* config;
	
	// Resources
	resource_manager* resource_manager;
	
	// Localization
	std::string language_code;
	int language_index;
	string_table* string_table;
	string_table_map string_table_map;
	std::unordered_map<std::string, std::string>* strings;
	std::unordered_map<std::string, type::typeface*> typefaces;
	
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
	
	// Compositing
	clear_pass* ui_clear_pass;
	material_pass* ui_material_pass;
	compositor* ui_compositor;
	
	bloom_pass* common_bloom_pass;
	final_pass* common_final_pass;
	
	clear_pass* underground_clear_pass;
	material_pass* underground_material_pass;
	compositor* underground_compositor;
	
	clear_pass* surface_shadow_map_clear_pass;
	shadow_map_pass* surface_shadow_map_pass;
	clear_pass* surface_clear_pass;
	sky_pass* surface_sky_pass;
	material_pass* surface_material_pass;
	outline_pass* surface_outline_pass;
	compositor* surface_compositor;
	
	// Scene utilities
	scene::collection* active_scene;
	geom::aabb<float> no_cull;
	
	// UI scene
	scene::collection* ui_scene;
	scene::camera* ui_camera;
	scene::billboard* splash_billboard;
	scene::billboard* camera_flash_billboard;
	
	// Surface scene
	scene::collection* surface_scene;
	scene::camera* surface_camera;
	
	// Underground scene
	scene::collection* underground_scene;
	scene::camera* underground_camera;
	scene::ambient_light* underground_ambient_light;
	scene::spot_light* flashlight_spot_light;
	
	// Animation
	timeline* timeline;
	animator* animator;
	tween<double>* time_tween;
	animation<float>* radial_transition_in;
	animation<float>* radial_transition_out;
	screen_transition* fade_transition;
	material_property<float3>* fade_transition_color;
	screen_transition* radial_transition_inner;
	screen_transition* radial_transition_outer;
	animation<float>* equip_tool_animation;
	animation<float>* unequip_tool_animation;
	animation<float>* camera_flash_animation;
	
	// Controls
	input::event_router* input_event_router;
	input::mapper* input_mapper;
	input::listener* input_listener;
	std::unordered_map<std::string, input::control*> controls;
	bool mouse_look;
	
	// Parallel processes
	std::unordered_map<std::string, std::function<void(double, double)>> processes;

	// Entities
	entity::registry* entity_registry;
	std::unordered_map<std::string, entity::id> entities;

	// Systems
	entity::system::behavior* behavior_system;
	entity::system::camera* camera_system;
	entity::system::collision* collision_system;
	entity::system::constraint* constraint_system;
	entity::system::locomotion* locomotion_system;
	entity::system::snapping* snapping_system;
	entity::system::render* render_system;
	entity::system::samara* samara_system;
	entity::system::subterrain* subterrain_system;
	entity::system::terrain* terrain_system;
	entity::system::ui* ui_system;
	entity::system::vegetation* vegetation_system;
	entity::system::spatial* spatial_system;
	entity::system::painting* painting_system;
	entity::system::blackbody* blackbody_system;
	entity::system::atmosphere* atmosphere_system;
	entity::system::astronomy* astronomy_system;
	entity::system::orbit* orbit_system;
	entity::system::proteome* proteome_system;
	
	// Debug
	debug::cli* cli;
};

} // namespace game

#endif // ANTKEEPER_GAME_CONTEXT_HPP
