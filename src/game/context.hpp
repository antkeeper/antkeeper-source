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

#ifndef ANTKEEPER_GAME_CONTEXT_HPP
#define ANTKEEPER_GAME_CONTEXT_HPP

#include <engine/animation/tween.hpp>
#include <engine/app/input-manager.hpp>
#include <engine/app/window-manager.hpp>
#include <engine/entity/id.hpp>
#include <engine/entity/registry.hpp>
#include <engine/event/subscription.hpp>
#include "game/ecoregion.hpp"
#include "game/loop.hpp"
#include "game/state/base.hpp"
#include <engine/geom/aabb.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/rasterizer.hpp>
#include <engine/gl/texture-2d.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/i18n/string-map.hpp>
#include <engine/input/action-map.hpp>
#include <engine/input/action.hpp>
#include <engine/input/mapper.hpp>
#include <engine/math/moving-average.hpp>
#include <engine/render/anti-aliasing-method.hpp>
#include <engine/render/material-property.hpp>
#include <engine/render/material.hpp>
#include <engine/resources/json.hpp>
#include <engine/scene/scene.hpp>
#include <engine/type/bitmap-font.hpp>
#include <engine/type/typeface.hpp>
#include <engine/utility/dict.hpp>
#include <engine/utility/fundamental-types.hpp>
#include <engine/utility/state-machine.hpp>
#include <AL/al.h>
#include <AL/alc.h>
#include <entt/entt.hpp>
#include <filesystem>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

// Forward declarations
class animator;
class resource_manager;
class screen_transition;
class timeline;

template <typename T> class animation;

namespace debug
{
	class cli;
}

namespace render
{
	class bloom_pass;
	class clear_pass;
	class compositor;
	class final_pass;
	class fxaa_pass;
	class resample_pass;
	class material_pass;
	class renderer;
	class outline_pass;
	class shadow_map_pass;
	class simple_render_pass;
	class sky_pass;
	class ground_pass;
}


// Forward declaration of system types.
class subterrain_system;
class terrain_system;
class spatial_system;
class astronomy_system;
class blackbody_system;
class atmosphere_system;
class orbit_system;
class behavior_system;
class collision_system;
class constraint_system;
class locomotion_system;
class camera_system;
class nest_system;
class render_system;
class steering_system;
class spring_system;

struct control_profile;

/// Container for data shared between game states.
struct context
{
	context();
	~context();
	
	// Command-line options
	std::optional<bool> option_continue;
	std::optional<std::string> option_data;
	std::optional<bool> option_fullscreen;
	std::optional<bool> option_new_game;
	std::optional<bool> option_quick_start;
	std::optional<bool> option_reset;
	std::optional<bool> option_v_sync;
	std::optional<bool> option_windowed;
	
	// Resource management and paths
	resource_manager* resource_manager;
	std::filesystem::path data_package_path;
	std::filesystem::path mods_path;
	std::filesystem::path local_config_path;
	std::filesystem::path shared_config_path;
	std::filesystem::path saves_path;
	std::filesystem::path screenshots_path;
	std::filesystem::path controls_path;
	
	// Persistent settings
	dict<std::uint32_t>* settings;
	
	// Window management and window event handling
	app::window_manager* window_manager;
	app::window* window;
	bool closed;
	std::shared_ptr<::event::subscription> window_closed_subscription;
	std::shared_ptr<::event::subscription> window_resized_subscription;
	
	// Input management and input event handling
	app::input_manager* input_manager;
	std::shared_ptr<::event::subscription> application_quit_subscription;
	std::shared_ptr<::event::subscription> gamepad_axis_moved_subscription;
	std::shared_ptr<::event::subscription> gamepad_button_pressed_subscription;
	std::shared_ptr<::event::subscription> mouse_button_pressed_subscription;
	std::shared_ptr<::event::subscription> mouse_moved_subscription;
	std::shared_ptr<::event::subscription> mouse_scrolled_subscription;
	bool gamepad_active;
	
	// Localization and internationalization
	std::string language_tag;
	i18n::string_map* string_map;
	
	// Fonts
	std::unordered_map<std::string, type::typeface*> typefaces;
	type::bitmap_font debug_font;
	type::bitmap_font menu_font;
	type::bitmap_font title_font;
	render::material debug_font_material;
	render::material menu_font_material;
	render::material title_font_material;
	
	// Action maps, actions, and action event handling
	std::string control_profile_filename;
	::control_profile* control_profile;
	
	std::unordered_map<std::uint32_t, input::action> actions;
	input::action_map window_action_map;
	input::action_map menu_action_map;
	input::action_map movement_action_map;
	input::action_map nuptial_flight_action_map;
	input::mapper input_mapper;
	
	input::action fullscreen_action;
	input::action screenshot_action;
	input::action menu_up_action;
	input::action menu_down_action;
	input::action menu_left_action;
	input::action menu_right_action;
	input::action menu_select_action;
	input::action menu_back_action;
	input::action menu_modifier_action;
	input::action move_forward_action;
	input::action move_back_action;
	input::action move_left_action;
	input::action move_right_action;
	input::action move_up_action;
	input::action move_down_action;
	input::action pause_action;
	input::action pick_mate_action;
	
	std::vector<std::shared_ptr<::event::subscription>> window_action_subscriptions;
	std::vector<std::shared_ptr<::event::subscription>> menu_action_subscriptions;
	std::vector<std::shared_ptr<::event::subscription>> menu_mouse_subscriptions;
	std::vector<std::shared_ptr<::event::subscription>> movement_action_subscriptions;
	
	
	// Debugging
	scene::text* frame_time_text;
	debug::cli* cli;
	
	// Hierarchichal state machine
	hsm::state_machine<::state::base> state_machine;
	std::function<void()> resume_callback;
	
	// Queue for scheduling "next frame" function calls
	std::queue<std::function<void()>> function_queue;
	
	// Parallel processes
	std::unordered_map<std::string, std::function<void(double, double)>> processes;
	
	bool mouse_look;
	
	/// Game loop
	::loop loop;
	
	// Framebuffers
	gl::texture_2d* hdr_color_texture;
	gl::texture_2d* hdr_depth_texture;
	gl::framebuffer* hdr_framebuffer;
	gl::texture_2d* ldr_color_texture_a;
	gl::framebuffer* ldr_framebuffer_a;
	gl::texture_2d* ldr_color_texture_b;
	gl::framebuffer* ldr_framebuffer_b;
	gl::texture_2d* shadow_map_depth_texture;
	gl::framebuffer* shadow_map_framebuffer;
	
	// Rendering
	gl::rasterizer* rasterizer;
	render::renderer* renderer;
	int2 render_resolution;
	float render_scale;
	int shadow_map_resolution;
	gl::vertex_buffer* billboard_vbo;
	gl::vertex_array* billboard_vao;
	render::material* fallback_material;
	
	// Compositing
	render::clear_pass* ui_clear_pass;
	render::material_pass* ui_material_pass;
	render::compositor* ui_compositor;
	render::bloom_pass* bloom_pass;
	render::final_pass* common_final_pass;
	render::fxaa_pass* fxaa_pass;
	render::resample_pass* resample_pass;
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
	float font_scale;
	bool dyslexia_font;
	float debug_font_size_pt;
	float menu_font_size_pt;
	float title_font_size_pt;
	
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
	ALCdevice* alc_device;
	ALCcontext* alc_context;
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
	::behavior_system* behavior_system;
	::camera_system* camera_system;
	::collision_system* collision_system;
	::constraint_system* constraint_system;
	::locomotion_system* locomotion_system;
	::steering_system* steering_system;
	::render_system* render_system;
	::subterrain_system* subterrain_system;
	::terrain_system* terrain_system;
	::spring_system* spring_system;
	::spatial_system* spatial_system;
	::blackbody_system* blackbody_system;
	::atmosphere_system* atmosphere_system;
	::astronomy_system* astronomy_system;
	::orbit_system* orbit_system;
	
	std::unique_ptr<::orbit_system> unique_orbit;
	
	double3 rgb_wavelengths;
	
	const ecoregion* active_ecoregion;
	
	render::anti_aliasing_method anti_aliasing_method;
};


#endif // ANTKEEPER_GAME_CONTEXT_HPP
