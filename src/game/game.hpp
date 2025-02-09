// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_HPP
#define ANTKEEPER_GAME_HPP

#include "game/ecoregion.hpp"
#include "game/states/game-state.hpp"
#include "game/systems/component-system.hpp"
#include <engine/app/input-manager.hpp>
#include <engine/app/window-manager.hpp>
#include <engine/entity/id.hpp>
#include <engine/entity/registry.hpp>
#include <engine/event/subscription.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/texture.hpp>
#include <engine/i18n/string-map.hpp>
#include <engine/input/action-map.hpp>
#include <engine/input/action.hpp>
#include <engine/input/mapper.hpp>
#include <engine/math/moving-average.hpp>
#include <engine/render/anti-aliasing-method.hpp>
#include <engine/render/material-variable.hpp>
#include <engine/render/material.hpp>
#include <engine/type/font.hpp>
#include <engine/type/typeface.hpp>
#include <engine/utility/json.hpp>
#include <engine/math/vector.hpp>
#include <engine/utility/state-machine.hpp>
#include <engine/utility/frame-scheduler.hpp>
#include <engine/scene/text.hpp>
#include <engine/scene/directional-light.hpp>
#include <engine/scene/rectangle-light.hpp>
#include <engine/scene/spot-light.hpp>
#include <engine/scene/camera.hpp>
#include <engine/scene/billboard.hpp>
#include <engine/scene/collection.hpp>
#include <engine/scene/text.hpp>
#include <engine/math/functions.hpp>
#include <engine/audio/sound-system.hpp>
#include <engine/audio/sound-que.hpp>
#include <engine/ui/canvas.hpp>
#include <engine/ui/label.hpp>
#include <engine/script/script-context.hpp>
#include <entt/entt.hpp>
#include <filesystem>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>
#include <random>
#include <vector>

// Forward declarations
class resource_manager;
class shell;
class shell_buffer;

namespace render
{
	class bloom_pass;
	class compositor;
	class composite_pass;
	class material_pass;
	class renderer;
	class simple_render_pass;
	class sky_pass;
	class clear_pass;
}


// Forward declarations of system types.
class astronomy_system;
class atmosphere_system;
class blackbody_system;
class constraint_system;
class nest_system;
class orbit_system;
class render_system;
class physics_system;
class animation_sequence;
class fixed_update_system;
class variable_update_system;

struct control_profile;


class game
{
public:
	/// Boots up the game.
	/// @param argc Command line argument count.
	/// @param argv Command line argument vector.
	game(int argc, const char* const* argv);
	
	/// Boots down the game.
	~game();
	
	/// Executes the game.
	void execute();
	
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
	std::unique_ptr<resource_manager> resource_manager;
	std::filesystem::path data_package_path;
	std::filesystem::path mods_path;
	std::filesystem::path local_config_path;
	std::filesystem::path shared_config_path;
	std::filesystem::path saves_path;
	std::filesystem::path screenshots_path;
	std::filesystem::path controls_path;
	
	// Persistent settings
	std::shared_ptr<json> settings;
	
	// Window management and window event handling
	std::unique_ptr<app::window_manager> window_manager;
	std::shared_ptr<app::window> window;
	bool closed{false};
	std::shared_ptr<::event::subscription> window_closed_subscription;
	std::shared_ptr<::event::subscription> window_resized_subscription;
	
	// Input management and input event handling
	std::unique_ptr<app::input_manager> input_manager;
	std::shared_ptr<::event::subscription> application_quit_subscription;
	std::shared_ptr<::event::subscription> gamepad_axis_moved_subscription;
	std::shared_ptr<::event::subscription> gamepad_button_pressed_subscription;
	std::shared_ptr<::event::subscription> mouse_moved_subscription;
	std::shared_ptr<::event::subscription> mouse_button_pressed_subscription;
	std::shared_ptr<::event::subscription> mouse_button_released_subscription;
	std::shared_ptr<::event::subscription> mouse_scrolled_subscription;
	bool gamepad_active;

	std::shared_ptr<event::subscription> m_key_mapped_subscription;
	std::shared_ptr<event::subscription> m_mouse_button_mapped_subscription;
	std::shared_ptr<event::subscription> m_mouse_scroll_mapped_subscription;
	std::shared_ptr<event::subscription> m_gamepad_axis_mapped_subscription;
	std::shared_ptr<event::subscription> m_gamepad_button_mapped_subscription;
	
	// Localization and internationalization
	std::shared_ptr<json> languages;
	std::string language_tag;
	std::shared_ptr<json> string_map;
	
	// Fonts
	std::unordered_map<hash::fnv32_t, std::shared_ptr<type::typeface>> typefaces;
	std::shared_ptr<type::font> debug_font;
	std::shared_ptr<type::font> menu_font;
	std::shared_ptr<type::font> title_font;
	std::shared_ptr<render::material> debug_font_material;
	std::shared_ptr<render::material> menu_font_material;
	std::shared_ptr<render::material> title_font_material;
	
	// Action maps, actions, and action event handling
	std::string control_profile_filename;
	std::shared_ptr<::control_profile> control_profile;
	
	input::action_map window_action_map;
	input::action fullscreen_action;
	input::action screenshot_action;
	input::action toggle_terminal_action;
	
	input::action_map menu_action_map;
	input::action menu_up_action;
	input::action menu_down_action;
	input::action menu_left_action;
	input::action menu_right_action;
	input::action menu_select_action;
	input::action menu_back_action;
	input::action menu_modifier_action;
	
	input::action_map movement_action_map;
	input::mapper input_mapper;

	input::action move_forward_action;
	input::action move_back_action;
	input::action move_left_action;
	input::action move_right_action;
	input::action move_up_action;
	input::action move_down_action;
	input::action move_fast_action;
	input::action move_slow_action;
	input::action pause_action;
	
	input::action_map camera_action_map;
	input::action camera_mouse_pick_action;
	input::action camera_mouse_look_action;
	input::action camera_mouse_drag_action;
	input::action camera_mouse_zoom_action;
	input::action camera_zoom_in_action;
	input::action camera_zoom_out_action;
	input::action camera_orbit_left_action;
	input::action camera_orbit_right_action;
	input::action camera_orbit_up_action;
	input::action camera_orbit_down_action;
	input::action camera_look_ahead_action;
	
	input::action_map ant_action_map;
	input::action ant_move_forward_action;
	input::action ant_move_back_action;
	input::action ant_move_left_action;
	input::action ant_move_right_action;
	input::action ant_move_fast_action;
	input::action ant_move_slow_action;
	input::action ant_interact_action;
	input::action ant_oviposit_action;
	input::action ant_stridulate_action;
	math::fvec2 old_mouse_position{};
	
	input::action_map debug_action_map;
	input::action toggle_debug_ui_action;
	input::action adjust_exposure_action;
	input::action adjust_time_action;
	
	input::action_map terminal_action_map;
	input::action terminal_up_action;
	input::action terminal_down_action;
	input::action terminal_left_action;
	input::action terminal_right_action;
	input::action terminal_enter_action;
	input::action terminal_backspace_action;
	input::action terminal_paste_action;
	input::action terminal_clear_line_action;
	
	std::vector<std::shared_ptr<::event::subscription>> event_subscriptions;
	
	std::vector<std::shared_ptr<::event::subscription>> menu_action_subscriptions;
	std::vector<std::shared_ptr<::event::subscription>> menu_mouse_subscriptions;
	std::vector<std::shared_ptr<::event::subscription>> movement_action_subscriptions;

	bool m_ingame{false};
	
	// Mouse settings
	double mouse_radians_per_pixel{math::radians(0.1)};
	double mouse_pan_sensitivity{1.0};
	double mouse_tilt_sensitivity{1.0};
	bool mouse_invert_pan{false};
	bool mouse_invert_tilt{false};
	double mouse_pan_factor{1.0};
	double mouse_tilt_factor{1.0};
	bool toggle_mouse_look{false};
	bool toggle_mouse_grip{false};
	bool toggle_mouse_zoom{false};
	float zoom_steps{6.0};
	
	// Gamepad settings
	double gamepad_radians_per_second{math::radians(180.0)};
	double gamepad_pan_sensitivity{1.0};
	double gamepad_tilt_sensitivity{1.0};
	bool gamepad_invert_pan{false};
	bool gamepad_invert_tilt{false};
	double gamepad_pan_factor{1.0};
	double gamepad_tilt_factor{1.0};

	// Scripting
	script_context script;
	std::unordered_map<std::string, std::pair<int, std::shared_ptr<event::subscription>>> script_event_subscriptions;
	
	// Debugging
	bool debug_ui_visible{false};
	std::unique_ptr<scene::text> frame_time_text;
	bool terminal_enabled{false};
	std::string command_line;
	std::size_t command_line_cursor{};
	std::shared_ptr<scene::text> command_line_text;
	std::shared_ptr<scene::text> shell_buffer_text;
	std::unique_ptr<::shell_buffer> shell_buffer;
	std::unique_ptr<::shell> shell;
	std::vector<std::function<void()>> reenable_controls;
	
	// Hierarchichal state machine
	hsm::state_machine<game_state> state_machine;
	
	// Queue for scheduling "next frame" function calls
	std::queue<std::function<void()>> function_queue;
	
	// Framebuffers
	std::shared_ptr<gl::texture_2d> scene_color_texture;
	std::shared_ptr<gl::texture_2d> scene_depth_stencil_texture;
	std::shared_ptr<gl::framebuffer> scene_framebuffer;

	std::shared_ptr<gl::texture_2d> ui_color_texture;
	std::shared_ptr<gl::texture_2d> ui_depth_stencil_texture;
	std::shared_ptr<gl::framebuffer> ui_framebuffer;

	std::shared_ptr<gl::texture_2d> shadow_map_depth_texture;
	std::shared_ptr<gl::framebuffer> shadow_map_framebuffer;
	
	// Rendering
	math::ivec2 render_resolution;
	float render_scale;
	int shadow_map_resolution;
	render::anti_aliasing_method anti_aliasing_method;
	std::unique_ptr<render::material_pass> ui_material_pass;
	std::unique_ptr<render::compositor> ui_compositor;
	std::unique_ptr<render::bloom_pass> bloom_pass;
	std::unique_ptr<render::composite_pass> composite_pass;
	std::unique_ptr<render::clear_pass> clear_pass;
	std::unique_ptr<render::sky_pass> sky_pass;
	std::unique_ptr<render::material_pass> scene_material_pass;
	std::unique_ptr<render::compositor> scene_compositor;
	std::unique_ptr<render::renderer> renderer;
	
	// UI
	std::shared_ptr<ui::canvas> ui_canvas;
	std::unique_ptr<scene::camera> ui_camera;

	std::unique_ptr<scene::billboard> screen_transition_billboard;
	std::shared_ptr<render::material> screen_transition_material;

	std::shared_ptr<ui::element> m_root_menu_container;
	std::shared_ptr<ui::element> m_main_menu_container;
	std::shared_ptr<ui::element> m_pause_menu_container;
	std::shared_ptr<ui::element> m_options_menu_container;
	std::shared_ptr<ui::element> m_controls_menu_container;
	std::shared_ptr<ui::element> m_keyboard_config_menu_container;
	std::shared_ptr<ui::element> m_gamepad_config_menu_container;
	std::shared_ptr<ui::element> m_graphics_menu_container;
	std::shared_ptr<ui::element> m_sound_menu_container;
	std::shared_ptr<ui::element> m_language_menu_container;
	std::shared_ptr<ui::element> m_extras_menu_container;
	std::shared_ptr<ui::element> m_credits_menu_container;
	std::weak_ptr<ui::element> m_menu_focused_element;
	std::shared_ptr<render::material> m_pause_menu_bg_material;
	std::shared_ptr<render::matvar_fvec4> m_pause_menu_bg_color;
	std::shared_ptr<ui::element> m_pause_menu_bg;

	entity::id m_menu_animation_entity{entt::null};
	std::shared_ptr<animation_sequence> m_menu_fade_in_sequence;
	std::shared_ptr<animation_sequence> m_menu_fade_out_sequence;
	entity::id m_pause_menu_bg_animation_entity{entt::null};
	std::shared_ptr<animation_sequence> m_pause_menu_bg_fade_in_sequence;
	std::shared_ptr<animation_sequence> m_pause_menu_bg_fade_out_sequence;
	
	float font_scale;
	bool dyslexia_font;
	float debug_font_size_pt;
	float menu_font_size_pt;
	float title_font_size_pt;
	
	// Scene
	std::unique_ptr<scene::collection> exterior_scene;
	std::shared_ptr<scene::camera> exterior_camera;
	std::unique_ptr<scene::directional_light> sun_light;
	std::unique_ptr<scene::directional_light> moon_light;
	std::unique_ptr<scene::collection> interior_scene;
	std::unique_ptr<scene::directional_light> interior_directional_light;
	std::unique_ptr<scene::rectangle_light> interior_rectangle_light;
	scene::collection* active_scene;
	
	// Animation
	std::shared_ptr<animation_sequence> menu_fade_in_sequence;
	std::shared_ptr<animation_sequence> menu_fade_out_sequence;
	entity::id screen_transition_entity{entt::null};
	std::shared_ptr<animation_sequence> screen_fade_in_sequence;
	std::shared_ptr<animation_sequence> screen_fade_out_sequence;
	
	// Sound
	std::unique_ptr<audio::sound_system> sound_system;
	float master_volume;
	float ambience_volume;
	float effects_volume;
	bool captions;
	float captions_size;
	std::shared_ptr<audio::sound_que> menu_left_sound;
	std::shared_ptr<audio::sound_que> menu_right_sound;
	std::shared_ptr<audio::sound_que> menu_up_sound;
	std::shared_ptr<audio::sound_que> menu_down_sound;
	std::shared_ptr<audio::sound_que> menu_select_sound;
	std::shared_ptr<audio::sound_que> menu_back_sound;
	
	// Random number generation
	std::mt19937 rng;
	
	// Entities
	std::unique_ptr<entity::registry> entity_registry;
	std::unordered_map<hash::fnv32_t, entity::id> entities;
	entity::id controlled_ant_eid{entt::null};
	entity::id active_camera_eid{entt::null};
	
	// Systems
	std::shared_ptr<constraint_system> m_constraint_system;
	std::shared_ptr<physics_system> m_physics_system;
	std::shared_ptr<render_system> m_render_system;
	std::shared_ptr<blackbody_system> m_blackbody_system;
	std::shared_ptr<atmosphere_system> m_atmosphere_system;
	std::shared_ptr<astronomy_system> m_astronomy_system;
	std::shared_ptr<orbit_system> m_orbit_system;
	std::vector<std::shared_ptr<fixed_update_system>> m_fixed_update_systems;
	std::vector<std::shared_ptr<variable_update_system>> m_variable_update_systems;
	
	// Frame timing
	float fixed_update_rate{60.0};
	float max_frame_rate{120.0};
	bool limit_frame_rate{false};
	::frame_scheduler frame_scheduler;
	math::moving_average<float> average_frame_duration;
	
	std::shared_ptr<ecoregion> active_ecoregion;
	
private:
	void parse_options(int argc, const char* const* argv);
	void setup_resources();
	void load_settings();
	void setup_window();
	void setup_input();
	void load_language();
	void setup_rendering();
	void setup_audio();
	void setup_scenes();
	void setup_animation();
	void setup_ui();
	void setup_rng();
	void setup_entities();
	void setup_systems();
	void setup_controls();
	void setup_scripting();
	void setup_debugging();
	void setup_timing();
	
	void fixed_update(::frame_scheduler::duration_type fixed_update_time, ::frame_scheduler::duration_type fixed_update_interval);
	void variable_update(::frame_scheduler::duration_type fixed_update_time, ::frame_scheduler::duration_type fixed_update_interval, ::frame_scheduler::duration_type accumulated_time);
};

#endif // ANTKEEPER_GAME_HPP
