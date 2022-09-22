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
#include <vector>
#include <queue>
#include "resources/json.hpp"
#include "type/typeface.hpp"
#include "type/bitmap-font.hpp"
#include "render/material.hpp"
#include "render/material-property.hpp"
#include "ui/mouse-tracker.hpp"
#include "application.hpp"
#include "game/state/base.hpp"
#include "game/loop.hpp"
#include "state-machine.hpp"
#include "debug/performance-sampler.hpp"
#include <filesystem>

// Forward declarations
class animator;
class application;

class resource_manager;
class screen_transition;
class timeline;

template <typename T> class animation;

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
		class steering;
	}
}

namespace render
{
	class bloom_pass;
	class clear_pass;
	class compositor;
	class final_pass;
	class material_pass;
	class renderer;
	class outline_pass;
	class shadow_map_pass;
	class simple_render_pass;
	class sky_pass;
	class ground_pass;
}

namespace game {

/// Container for data shared between game states.
struct context
{
	/// Hierarchichal state machine
	hsm::state_machine<game::state::base> state_machine;
	std::function<void()> resume_callback;
	
	/// Debugging
	debug::logger* logger;
	std::ofstream log_filestream;
	debug::performance_sampler performance_sampler;
	debug::cli* cli;
	
	/// Queue for scheduling "next frame" function calls
	std::queue<std::function<void()>> function_queue;
	
	// Parallel processes
	std::unordered_map<std::string, std::function<void(double, double)>> processes;
	
	/// Interface for window management and input events
	application* app;
	
	// Controls
	input::event_router* input_event_router;
	input::mapper* input_mapper;
	input::listener* input_listener;
	std::unordered_map<std::string, input::control*> controls;
	bool mouse_look;
	
	/// Game loop
	game::loop loop;
	
	// Paths
	std::filesystem::path data_path;
	std::filesystem::path config_path;
	std::filesystem::path mods_path;
	std::filesystem::path saves_path;
	std::filesystem::path screenshots_path;
	std::filesystem::path controls_path;
	std::filesystem::path data_package_path;
	
	// Resources
	resource_manager* resource_manager;
	
	// Configuration
	json* config;
	
	// Localization
	std::string language_code;
	int language_count;
	int language_index;
	string_table* string_table;
	string_table_map string_table_map;
	std::unordered_map<std::string, std::string>* strings;
	std::unordered_map<std::string, type::typeface*> typefaces;
	type::bitmap_font debug_font;
	type::bitmap_font menu_font;
	type::bitmap_font title_font;
	render::material debug_font_material;
	render::material menu_font_material;
	render::material title_font_material;
	
	// Framebuffers
	gl::texture_2d* hdr_color_texture;
	gl::texture_2d* hdr_depth_texture;
	gl::framebuffer* hdr_framebuffer;
	gl::texture_2d* bloom_color_texture;
	gl::framebuffer* bloom_framebuffer;
	gl::texture_2d* shadow_map_depth_texture;
	gl::framebuffer* shadow_map_framebuffer;
	
	// Rendering
	gl::rasterizer* rasterizer;
	render::renderer* renderer;
	int2 render_resolution;
	float render_resolution_scale;
	gl::vertex_buffer* billboard_vbo;
	gl::vertex_array* billboard_vao;
	render::material* fallback_material;
	
	// Compositing
	render::clear_pass* ui_clear_pass;
	render::material_pass* ui_material_pass;
	render::compositor* ui_compositor;
	render::bloom_pass* common_bloom_pass;
	render::final_pass* common_final_pass;
	render::clear_pass* underground_clear_pass;
	render::material_pass* underground_material_pass;
	render::compositor* underground_compositor;
	render::clear_pass* surface_shadow_map_clear_pass;
	render::shadow_map_pass* surface_shadow_map_pass;
	render::clear_pass* surface_clear_pass;
	render::sky_pass* sky_pass;
	render::material_pass* surface_material_pass;
	render::outline_pass* surface_outline_pass;
	render::compositor* surface_compositor;
	render::ground_pass* ground_pass;
	
	
	// Scene utilities
	scene::collection* active_scene;
	
	// UI
	scene::collection* ui_scene;
	scene::camera* ui_camera;
	scene::billboard* camera_flash_billboard;
	float font_size;
	bool dyslexia_font;
	ui::mouse_tracker* menu_mouse_tracker;
	std::vector<std::function<void()>> menu_select_callbacks;
	std::vector<std::function<void()>> menu_left_callbacks;
	std::vector<std::function<void()>> menu_right_callbacks;
	std::function<void()> menu_back_callback;
	std::vector<std::tuple<scene::text*, scene::text*>> menu_item_texts;
	std::unordered_map<std::string, int> menu_item_indices;
	int* menu_item_index;
	scene::billboard* menu_bg_billboard;
	animation<float>* menu_fade_animation;
	animation<float>* menu_bg_fade_in_animation;
	animation<float>* menu_bg_fade_out_animation;
	
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
	animation<float>* radial_transition_in;
	animation<float>* radial_transition_out;
	screen_transition* fade_transition;
	render::material_property<float3>* fade_transition_color;
	screen_transition* radial_transition_inner;
	screen_transition* radial_transition_outer;
	animation<float>* equip_tool_animation;
	animation<float>* unequip_tool_animation;
	animation<float>* camera_flash_animation;
	
	// Sound
	float master_volume;
	float ambience_volume;
	float effects_volume;
	bool mono_audio;
	bool captions;
	float captions_size;

	// Entities
	entity::registry* entity_registry;
	std::unordered_map<std::string, entity::id> entities;

	// Systems
	entity::system::behavior* behavior_system;
	entity::system::camera* camera_system;
	entity::system::collision* collision_system;
	entity::system::constraint* constraint_system;
	entity::system::locomotion* locomotion_system;
	entity::system::steering* steering_system;
	entity::system::snapping* snapping_system;
	entity::system::render* render_system;
	entity::system::samara* samara_system;
	entity::system::subterrain* subterrain_system;
	entity::system::terrain* terrain_system;
	entity::system::vegetation* vegetation_system;
	entity::system::spatial* spatial_system;
	entity::system::painting* painting_system;
	entity::system::blackbody* blackbody_system;
	entity::system::atmosphere* atmosphere_system;
	entity::system::astronomy* astronomy_system;
	entity::system::orbit* orbit_system;
	entity::system::proteome* proteome_system;
};

} // namespace game

#endif // ANTKEEPER_GAME_CONTEXT_HPP
