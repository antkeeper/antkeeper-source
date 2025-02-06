// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/controls.hpp"
#include "game/graphics.hpp"
#include "game/menu.hpp"
#include "game/control-profile.hpp"
#include <engine/resources/resource-manager.hpp>
#include <engine/utility/json.hpp>
#include <engine/input/modifier-key.hpp>
#include <engine/hash/fnv.hpp>

using namespace hash::literals;

void reset_control_profile(::control_profile& profile)
{
	auto& mappings = profile.mappings;
	auto& settings = profile.settings;
	
	mappings.clear();
	settings.clear();
	
	// Fullscreen
	mappings.emplace("fullscreen"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::f11, input::modifier_key::none, false));
	mappings.emplace("fullscreen"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::enter, input::modifier_key::alt, false));
	
	// Screenshot
	mappings.emplace("screenshot"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::f12, input::modifier_key::none, false));
	mappings.emplace("screenshot"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::print_screen, input::modifier_key::none, false));
	
	// Toggle terminal
	mappings.emplace("toggle_terminal"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::grave, input::modifier_key::none, false));
	
	// Menu up
	mappings.emplace("menu_up"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::up, input::modifier_key::none, true));
	mappings.emplace("menu_up"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::w, input::modifier_key::none, true));
	mappings.emplace("menu_up"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::i, input::modifier_key::none, true));
	mappings.emplace("menu_up"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_stick_y, true));
	mappings.emplace("menu_up"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_stick_y, true));
	mappings.emplace("menu_up"_fnv1a32, std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::dpad_up));
	
	// Menu down
	mappings.emplace("menu_down"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::down, input::modifier_key::none, true));
	mappings.emplace("menu_down"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::s, input::modifier_key::none, true));
	mappings.emplace("menu_down"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::k, input::modifier_key::none, true));
	mappings.emplace("menu_down"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_stick_y, false));
	mappings.emplace("menu_down"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_stick_y, false));
	mappings.emplace("menu_down"_fnv1a32, std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::dpad_down));
	
	// Menu left
	mappings.emplace("menu_left"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::left, input::modifier_key::none, true));
	mappings.emplace("menu_left"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::a, input::modifier_key::none, true));
	mappings.emplace("menu_left"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::j, input::modifier_key::none, true));
	mappings.emplace("menu_left"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_stick_x, true));
	mappings.emplace("menu_left"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_stick_x, true));
	mappings.emplace("menu_left"_fnv1a32, std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::dpad_left));
	mappings.emplace("menu_left"_fnv1a32, std::make_unique<input::mouse_scroll_mapping>(nullptr, input::mouse_scroll_axis::x, true));
	mappings.emplace("menu_left"_fnv1a32, std::make_unique<input::mouse_scroll_mapping>(nullptr, input::mouse_scroll_axis::y, true));
	
	// Menu right
	mappings.emplace("menu_right"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::right, input::modifier_key::none, true));
	mappings.emplace("menu_right"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::d, input::modifier_key::none, true));
	mappings.emplace("menu_right"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::l, input::modifier_key::none, true));
	mappings.emplace("menu_right"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_stick_x, false));
	mappings.emplace("menu_right"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_stick_x, false));
	mappings.emplace("menu_right"_fnv1a32, std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::dpad_right));
	mappings.emplace("menu_right"_fnv1a32, std::make_unique<input::mouse_scroll_mapping>(nullptr, input::mouse_scroll_axis::x, false));
	mappings.emplace("menu_right"_fnv1a32, std::make_unique<input::mouse_scroll_mapping>(nullptr, input::mouse_scroll_axis::y, false));
	
	// Menu select
	mappings.emplace("menu_select"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::enter, input::modifier_key::none, false));
	mappings.emplace("menu_select"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::space, input::modifier_key::none, false));
	mappings.emplace("menu_select"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::e, input::modifier_key::none, false));
	mappings.emplace("menu_select"_fnv1a32, std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::a));
	
	// Menu back
	mappings.emplace("menu_back"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::escape, input::modifier_key::none, false));
	mappings.emplace("menu_back"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::backspace, input::modifier_key::none, false));
	mappings.emplace("menu_back"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::q, input::modifier_key::none, false));
	mappings.emplace("menu_back"_fnv1a32, std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::b));
	mappings.emplace("menu_back"_fnv1a32, std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::back));
	
	// Menu modifier
	mappings.emplace("menu_modifier"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::left_shift, input::modifier_key::none, false));
	mappings.emplace("menu_modifier"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::right_shift, input::modifier_key::none, false));
	
	// Move forward
	mappings.emplace("move_forward"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::w, input::modifier_key::none, false));
	mappings.emplace("move_forward"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_stick_y, true));
	
	// Move back
	mappings.emplace("move_back"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::s, input::modifier_key::none, false));
	mappings.emplace("move_back"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_stick_y, false));
	
	// Move left
	mappings.emplace("move_left"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::a, input::modifier_key::none, false));
	mappings.emplace("move_left"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_stick_x, true));
	
	// Move right
	mappings.emplace("move_right"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::d, input::modifier_key::none, false));
	mappings.emplace("move_right"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_stick_x, false));
	
	// Move up
	mappings.emplace("move_up"_fnv1a32, std::make_unique<input::mouse_scroll_mapping>(nullptr, input::mouse_scroll_axis::y, false));
	mappings.emplace("move_up"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_trigger, false));
	
	// Move down
	mappings.emplace("move_down"_fnv1a32, std::make_unique<input::mouse_scroll_mapping>(nullptr, input::mouse_scroll_axis::y, true));
	mappings.emplace("move_down"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_trigger, false));
	
	// Interact
	mappings.emplace("interact"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::e, input::modifier_key::none, false));
	mappings.emplace("interact"_fnv1a32, std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::a));
	
	// Move fast
	// mappings.emplace("move_fast", std::make_unique<input::key_mapping>(nullptr, input::scancode::left_shift, 0, false));
	
	// Move slow
	mappings.emplace("move_slow"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::left_ctrl, input::modifier_key::none, false));
	
	// Camera mouse pick
	mappings.emplace("camera_mouse_pick"_fnv1a32, std::make_unique<input::mouse_button_mapping>(nullptr, input::mouse_button::left));
	
	// Camera mouse look
	mappings.emplace("camera_mouse_look"_fnv1a32, std::make_unique<input::mouse_button_mapping>(nullptr, input::mouse_button::right));
	
	// Camera mouse drag
	mappings.emplace("camera_mouse_drag"_fnv1a32, std::make_unique<input::mouse_button_mapping>(nullptr, input::mouse_button::middle));
	
	// Camera zoom
	mappings.emplace("camera_mouse_zoom"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::z, input::modifier_key::none, false));
	mappings.emplace("camera_zoom_in"_fnv1a32, std::make_unique<input::mouse_scroll_mapping>(nullptr, input::mouse_scroll_axis::y, false));
	mappings.emplace("camera_zoom_out"_fnv1a32, std::make_unique<input::mouse_scroll_mapping>(nullptr, input::mouse_scroll_axis::y, true));
	
	// Camera orbit
	mappings.emplace("camera_orbit_left"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::left, input::modifier_key::none, false));
	mappings.emplace("camera_orbit_left"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_stick_x, false));
	mappings.emplace("camera_orbit_right"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::right, input::modifier_key::none, false));
	mappings.emplace("camera_orbit_right"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_stick_x, true));
	mappings.emplace("camera_orbit_up"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::up, input::modifier_key::none, false));
	mappings.emplace("camera_orbit_up"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_stick_y, true));
	mappings.emplace("camera_orbit_down"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::up, input::modifier_key::none, false));
	mappings.emplace("camera_orbit_down"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_stick_y, false));
	
	// Camera look ahead
	mappings.emplace("camera_look_ahead"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::left_shift, input::modifier_key::none, false));
	mappings.emplace("camera_look_ahead"_fnv1a32, std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_trigger, false));
	
	// Oviposit
	mappings.emplace("oviposit"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::left_alt, input::modifier_key::none, false));

	// Stridulate
	mappings.emplace("stridulate"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::f, input::modifier_key::none, false));
	mappings.emplace("stridulate"_fnv1a32, std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::x));
	
	// Pause
	mappings.emplace("pause"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::escape, input::modifier_key::none, false));
	mappings.emplace("pause"_fnv1a32, std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::start));
	
	// Toggle debug UI
	// mappings.emplace("toggle_debug", std::make_unique<input::key_mapping>(nullptr, input::scancode::grave, 0, false));
	
	// Adjust exposure
	mappings.emplace("adjust_exposure"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::b, input::modifier_key::none, false));
	
	// Adjust time
	mappings.emplace("adjust_time"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::t, input::modifier_key::none, false));
	
	// Terminal keys
	mappings.emplace("terminal_up"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::up, input::modifier_key::none, true));
	mappings.emplace("terminal_down"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::down, input::modifier_key::none, true));
	mappings.emplace("terminal_left"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::left, input::modifier_key::none, true));
	mappings.emplace("terminal_right"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::right, input::modifier_key::none, true));
	mappings.emplace("terminal_enter"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::enter, input::modifier_key::none, true));
	mappings.emplace("terminal_backspace"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::backspace, input::modifier_key::none, true));
	mappings.emplace("terminal_paste"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::v, input::modifier_key::ctrl, true));
	mappings.emplace("terminal_paste"_fnv1a32, std::make_unique<input::mouse_button_mapping>(nullptr, input::mouse_button::right));
	mappings.emplace("terminal_clear_line"_fnv1a32, std::make_unique<input::key_mapping>(nullptr, input::scancode::escape, input::modifier_key::none, true));
}

void apply_control_profile(::game& ctx, const ::control_profile& profile)
{
	auto add_mappings = [&profile](input::action_map& map, input::action& action, hash::fnv32_t key)
	{
		auto range = profile.mappings.equal_range(key);
		for (auto i = range.first; i != range.second; ++i)
		{
			map.add_mapping(action, *i->second);
		}
	};
	
	// Window controls
	ctx.window_action_map.remove_mappings();
	add_mappings(ctx.window_action_map, ctx.fullscreen_action, "fullscreen"_fnv1a32);
	add_mappings(ctx.window_action_map, ctx.screenshot_action, "screenshot"_fnv1a32);
	add_mappings(ctx.window_action_map, ctx.toggle_terminal_action, "toggle_terminal"_fnv1a32);
	
	// Menu controls
	ctx.menu_action_map.remove_mappings();
	add_mappings(ctx.menu_action_map, ctx.menu_up_action, "menu_up"_fnv1a32);
	add_mappings(ctx.menu_action_map, ctx.menu_down_action, "menu_down"_fnv1a32);
	add_mappings(ctx.menu_action_map, ctx.menu_left_action, "menu_left"_fnv1a32);
	add_mappings(ctx.menu_action_map, ctx.menu_right_action, "menu_right"_fnv1a32);
	add_mappings(ctx.menu_action_map, ctx.menu_select_action, "menu_select"_fnv1a32);
	add_mappings(ctx.menu_action_map, ctx.menu_back_action, "menu_back"_fnv1a32);
	add_mappings(ctx.menu_action_map, ctx.menu_modifier_action, "menu_modifier"_fnv1a32);
	
	// Movement controls
	ctx.movement_action_map.remove_mappings();
	add_mappings(ctx.movement_action_map, ctx.move_forward_action, "move_forward"_fnv1a32);
	add_mappings(ctx.movement_action_map, ctx.move_back_action, "move_back"_fnv1a32);
	add_mappings(ctx.movement_action_map, ctx.move_left_action, "move_left"_fnv1a32);
	add_mappings(ctx.movement_action_map, ctx.move_right_action, "move_right"_fnv1a32);
	add_mappings(ctx.movement_action_map, ctx.move_up_action, "move_up"_fnv1a32);
	add_mappings(ctx.movement_action_map, ctx.move_down_action, "move_down"_fnv1a32);
	add_mappings(ctx.movement_action_map, ctx.move_fast_action, "move_fast"_fnv1a32);
	add_mappings(ctx.movement_action_map, ctx.move_slow_action, "move_slow"_fnv1a32);
	add_mappings(ctx.movement_action_map, ctx.pause_action, "pause"_fnv1a32);
	
	// Camera controls
	ctx.camera_action_map.remove_mappings();
	add_mappings(ctx.camera_action_map, ctx.camera_mouse_pick_action, "camera_mouse_pick"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_mouse_look_action, "camera_mouse_look"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_mouse_drag_action, "camera_mouse_drag"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_mouse_zoom_action, "camera_mouse_zoom"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_zoom_in_action, "camera_zoom_in"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_zoom_out_action, "camera_zoom_out"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_orbit_left_action, "camera_orbit_left"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_orbit_right_action, "camera_orbit_right"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_orbit_up_action, "camera_orbit_up"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_orbit_down_action, "camera_orbit_down"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_look_ahead_action, "camera_look_ahead"_fnv1a32);
	
	// Ant controls
	ctx.ant_action_map.remove_mappings();
	add_mappings(ctx.ant_action_map, ctx.ant_move_forward_action, "move_forward"_fnv1a32);
	add_mappings(ctx.ant_action_map, ctx.ant_move_back_action, "move_back"_fnv1a32);
	add_mappings(ctx.ant_action_map, ctx.ant_move_left_action, "move_left"_fnv1a32);
	add_mappings(ctx.ant_action_map, ctx.ant_move_right_action, "move_right"_fnv1a32);
	add_mappings(ctx.ant_action_map, ctx.ant_move_fast_action, "move_fast"_fnv1a32);
	add_mappings(ctx.ant_action_map, ctx.ant_move_slow_action, "move_slow"_fnv1a32);
	add_mappings(ctx.ant_action_map, ctx.ant_interact_action, "interact"_fnv1a32);
	add_mappings(ctx.ant_action_map, ctx.ant_oviposit_action, "oviposit"_fnv1a32);
	add_mappings(ctx.ant_action_map, ctx.ant_stridulate_action, "stridulate"_fnv1a32);
	
	// Debug controls
	add_mappings(ctx.debug_action_map, ctx.toggle_debug_ui_action, "toggle_debug"_fnv1a32);
	add_mappings(ctx.debug_action_map, ctx.adjust_exposure_action, "adjust_exposure"_fnv1a32);
	add_mappings(ctx.debug_action_map, ctx.adjust_time_action, "adjust_time"_fnv1a32);
	
	// Terminal controls
	add_mappings(ctx.terminal_action_map, ctx.terminal_up_action, "terminal_up"_fnv1a32);
	add_mappings(ctx.terminal_action_map, ctx.terminal_down_action, "terminal_down"_fnv1a32);
	add_mappings(ctx.terminal_action_map, ctx.terminal_left_action, "terminal_left"_fnv1a32);
	add_mappings(ctx.terminal_action_map, ctx.terminal_right_action, "terminal_right"_fnv1a32);
	add_mappings(ctx.terminal_action_map, ctx.terminal_enter_action, "terminal_enter"_fnv1a32);
	add_mappings(ctx.terminal_action_map, ctx.terminal_backspace_action, "terminal_backspace"_fnv1a32);
	add_mappings(ctx.terminal_action_map, ctx.terminal_paste_action, "terminal_paste"_fnv1a32);
	add_mappings(ctx.terminal_action_map, ctx.terminal_clear_line_action, "terminal_clear_line"_fnv1a32);
}

void update_control_profile(::game& ctx, ::control_profile& profile)
{
	auto add_mappings = [&profile](const input::action_map& map, const input::action& action, hash::fnv32_t key)
	{
		auto gamepad_axis_mappings = map.get_gamepad_axis_mappings(action);
		auto gamepad_button_mappings = map.get_gamepad_button_mappings(action);
		auto key_mappings = map.get_key_mappings(action);
		auto mouse_button_mappings = map.get_mouse_button_mappings(action);
		auto mouse_motion_mappings = map.get_mouse_motion_mappings(action);
		auto mouse_scroll_mappings = map.get_mouse_scroll_mappings(action);
		
		for (const auto& mapping: gamepad_axis_mappings)
		{
			profile.mappings.emplace(key, std::make_unique<input::gamepad_axis_mapping>(mapping));
		}
		for (const auto& mapping: gamepad_button_mappings)
		{
			profile.mappings.emplace(key, std::make_unique<input::gamepad_button_mapping>(mapping));
		}
		for (const auto& mapping: key_mappings)
		{
			profile.mappings.emplace(key, std::make_unique<input::key_mapping>(mapping));
		}
		for (const auto& mapping: mouse_button_mappings)
		{
			profile.mappings.emplace(key, std::make_unique<input::mouse_button_mapping>(mapping));
		}
		for (const auto& mapping: mouse_motion_mappings)
		{
			profile.mappings.emplace(key, std::make_unique<input::mouse_motion_mapping>(mapping));
		}
		for (const auto& mapping: mouse_scroll_mappings)
		{
			profile.mappings.emplace(key, std::make_unique<input::mouse_scroll_mapping>(mapping));
		}
	};
	
	profile.mappings.clear();
	
	// Window controls
	add_mappings(ctx.window_action_map, ctx.fullscreen_action, "fullscreen"_fnv1a32);
	add_mappings(ctx.window_action_map, ctx.screenshot_action, "screenshot"_fnv1a32);
	add_mappings(ctx.window_action_map, ctx.toggle_terminal_action, "toggle_terminal"_fnv1a32);
	
	// Menu controls
	add_mappings(ctx.menu_action_map, ctx.menu_up_action, "menu_up"_fnv1a32);
	add_mappings(ctx.menu_action_map, ctx.menu_down_action, "menu_down"_fnv1a32);
	add_mappings(ctx.menu_action_map, ctx.menu_left_action, "menu_left"_fnv1a32);
	add_mappings(ctx.menu_action_map, ctx.menu_right_action, "menu_right"_fnv1a32);
	add_mappings(ctx.menu_action_map, ctx.menu_select_action, "menu_select"_fnv1a32);
	add_mappings(ctx.menu_action_map, ctx.menu_back_action, "menu_back"_fnv1a32);
	add_mappings(ctx.menu_action_map, ctx.menu_modifier_action, "menu_modifier"_fnv1a32);
	
	// Movement controls
	add_mappings(ctx.movement_action_map, ctx.move_forward_action, "move_forward"_fnv1a32);
	add_mappings(ctx.movement_action_map, ctx.move_back_action, "move_back"_fnv1a32);
	add_mappings(ctx.movement_action_map, ctx.move_left_action, "move_left"_fnv1a32);
	add_mappings(ctx.movement_action_map, ctx.move_right_action, "move_right"_fnv1a32);
	add_mappings(ctx.movement_action_map, ctx.move_up_action, "move_up"_fnv1a32);
	add_mappings(ctx.movement_action_map, ctx.move_down_action, "move_down"_fnv1a32);
	add_mappings(ctx.movement_action_map, ctx.move_fast_action, "move_fast"_fnv1a32);
	add_mappings(ctx.movement_action_map, ctx.move_slow_action, "move_slow"_fnv1a32);
	add_mappings(ctx.movement_action_map, ctx.pause_action, "pause"_fnv1a32);
	
	// Ant controls
	add_mappings(ctx.ant_action_map, ctx.ant_move_forward_action, "move_forward"_fnv1a32);
	add_mappings(ctx.ant_action_map, ctx.ant_move_back_action, "move_back"_fnv1a32);
	add_mappings(ctx.ant_action_map, ctx.ant_move_left_action, "move_left"_fnv1a32);
	add_mappings(ctx.ant_action_map, ctx.ant_move_right_action, "move_right"_fnv1a32);
	add_mappings(ctx.ant_action_map, ctx.ant_move_fast_action, "move_fast"_fnv1a32);
	add_mappings(ctx.ant_action_map, ctx.ant_move_slow_action, "move_slow"_fnv1a32);
	add_mappings(ctx.ant_action_map, ctx.ant_interact_action, "interact"_fnv1a32);
	add_mappings(ctx.ant_action_map, ctx.ant_oviposit_action, "oviposit"_fnv1a32);
	add_mappings(ctx.ant_action_map, ctx.ant_stridulate_action, "stridulate"_fnv1a32);
	
	// Camera controls
	add_mappings(ctx.camera_action_map, ctx.camera_mouse_pick_action, "camera_mouse_pick"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_mouse_look_action, "camera_mouse_look"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_mouse_drag_action, "camera_mouse_drag"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_mouse_zoom_action, "camera_mouse_zoom"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_zoom_in_action, "camera_zoom_in"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_zoom_out_action, "camera_zoom_out"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_orbit_left_action, "camera_orbit_left"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_orbit_right_action, "camera_orbit_right"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_orbit_up_action, "camera_orbit_up"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_orbit_down_action, "camera_orbit_down"_fnv1a32);
	add_mappings(ctx.camera_action_map, ctx.camera_look_ahead_action, "camera_look_ahead"_fnv1a32);
	
	// Debug controls
	add_mappings(ctx.debug_action_map, ctx.toggle_debug_ui_action, "toggle_debug"_fnv1a32);
	add_mappings(ctx.debug_action_map, ctx.adjust_exposure_action, "adjust_exposure"_fnv1a32);
	add_mappings(ctx.debug_action_map, ctx.adjust_time_action, "adjust_time"_fnv1a32);
	
	// Terminal controls
	add_mappings(ctx.terminal_action_map, ctx.terminal_up_action, "terminal_up"_fnv1a32);
	add_mappings(ctx.terminal_action_map, ctx.terminal_down_action, "terminal_down"_fnv1a32);
	add_mappings(ctx.terminal_action_map, ctx.terminal_left_action, "terminal_left"_fnv1a32);
	add_mappings(ctx.terminal_action_map, ctx.terminal_right_action, "terminal_right"_fnv1a32);
	add_mappings(ctx.terminal_action_map, ctx.terminal_enter_action, "terminal_enter"_fnv1a32);
	add_mappings(ctx.terminal_action_map, ctx.terminal_backspace_action, "terminal_backspace"_fnv1a32);
	add_mappings(ctx.terminal_action_map, ctx.terminal_paste_action, "terminal_paste"_fnv1a32);
	add_mappings(ctx.terminal_action_map, ctx.terminal_clear_line_action, "terminal_clear_line"_fnv1a32);
}

void setup_game_controls(::game& ctx)
{
	// Setup pause control
	ctx.movement_action_subscriptions.emplace_back
	(
		ctx.pause_action.get_activated_channel().subscribe
		(
			[&ctx]([[maybe_unused]] const auto& event)
			{
				ctx.function_queue.emplace
				(
					[&ctx]()
					{
						::disable_game_controls(ctx);
						open_pause_menu(ctx);
					}
				);
			}
		)
	);
}

void enable_game_controls(::game& ctx)
{
	ctx.movement_action_map.enable();
	ctx.ant_action_map.enable();
}

void disable_game_controls(::game& ctx)
{
	ctx.movement_action_map.disable();
	ctx.ant_action_map.disable();
	
	ctx.move_forward_action.reset();
	ctx.move_back_action.reset();
	ctx.move_left_action.reset();
	ctx.move_right_action.reset();
	ctx.move_up_action.reset();
	ctx.move_down_action.reset();
	ctx.move_fast_action.reset();
	ctx.move_slow_action.reset();
	ctx.pause_action.reset();
}
