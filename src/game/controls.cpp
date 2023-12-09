// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/controls.hpp"
#include "game/graphics.hpp"
#include "game/menu.hpp"
#include "game/control-profile.hpp"
#include "game/states/pause-menu-state.hpp"
#include <engine/resources/resource-manager.hpp>
#include <engine/utility/json.hpp>
#include <engine/input/modifier-key.hpp>
#include <engine/hash/fnv1a.hpp>


void reset_control_profile(::control_profile& profile)
{
	auto& mappings = profile.mappings;
	auto& settings = profile.settings;
	
	mappings.clear();
	settings.clear();
	
	// Fullscreen
	mappings.emplace("fullscreen", std::make_unique<input::key_mapping>(nullptr, input::scancode::f11, 0, false));
	mappings.emplace("fullscreen", std::make_unique<input::key_mapping>(nullptr, input::scancode::enter, input::modifier_key::alt, false));
	
	// Screenshot
	mappings.emplace("screenshot", std::make_unique<input::key_mapping>(nullptr, input::scancode::f12, 0, false));
	mappings.emplace("screenshot", std::make_unique<input::key_mapping>(nullptr, input::scancode::print_screen, 0, false));
	
	// Menu up
	mappings.emplace("menu_up", std::make_unique<input::key_mapping>(nullptr, input::scancode::up, 0, true));
	mappings.emplace("menu_up", std::make_unique<input::key_mapping>(nullptr, input::scancode::w, 0, true));
	mappings.emplace("menu_up", std::make_unique<input::key_mapping>(nullptr, input::scancode::i, 0, true));
	mappings.emplace("menu_up", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_stick_y, true));
	mappings.emplace("menu_up", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_stick_y, true));
	mappings.emplace("menu_up", std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::dpad_up));
	
	// Menu down
	mappings.emplace("menu_down", std::make_unique<input::key_mapping>(nullptr, input::scancode::down, 0, true));
	mappings.emplace("menu_down", std::make_unique<input::key_mapping>(nullptr, input::scancode::s, 0, true));
	mappings.emplace("menu_down", std::make_unique<input::key_mapping>(nullptr, input::scancode::k, 0, true));
	mappings.emplace("menu_down", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_stick_y, false));
	mappings.emplace("menu_down", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_stick_y, false));
	mappings.emplace("menu_down", std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::dpad_down));
	
	// Menu left
	mappings.emplace("menu_left", std::make_unique<input::key_mapping>(nullptr, input::scancode::left, 0, true));
	mappings.emplace("menu_left", std::make_unique<input::key_mapping>(nullptr, input::scancode::a, 0, true));
	mappings.emplace("menu_left", std::make_unique<input::key_mapping>(nullptr, input::scancode::j, 0, true));
	mappings.emplace("menu_left", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_stick_x, true));
	mappings.emplace("menu_left", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_stick_x, true));
	mappings.emplace("menu_left", std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::dpad_left));
	mappings.emplace("menu_left", std::make_unique<input::mouse_scroll_mapping>(nullptr, input::mouse_scroll_axis::x, true));
	mappings.emplace("menu_left", std::make_unique<input::mouse_scroll_mapping>(nullptr, input::mouse_scroll_axis::y, true));
	
	// Menu right
	mappings.emplace("menu_right", std::make_unique<input::key_mapping>(nullptr, input::scancode::right, 0, true));
	mappings.emplace("menu_right", std::make_unique<input::key_mapping>(nullptr, input::scancode::d, 0, true));
	mappings.emplace("menu_right", std::make_unique<input::key_mapping>(nullptr, input::scancode::l, 0, true));
	mappings.emplace("menu_right", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_stick_x, false));
	mappings.emplace("menu_right", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_stick_x, false));
	mappings.emplace("menu_right", std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::dpad_right));
	mappings.emplace("menu_right", std::make_unique<input::mouse_scroll_mapping>(nullptr, input::mouse_scroll_axis::x, false));
	mappings.emplace("menu_right", std::make_unique<input::mouse_scroll_mapping>(nullptr, input::mouse_scroll_axis::y, false));
	
	// Menu select
	mappings.emplace("menu_select", std::make_unique<input::key_mapping>(nullptr, input::scancode::enter, 0, false));
	mappings.emplace("menu_select", std::make_unique<input::key_mapping>(nullptr, input::scancode::space, 0, false));
	mappings.emplace("menu_select", std::make_unique<input::key_mapping>(nullptr, input::scancode::e, 0, false));
	mappings.emplace("menu_select", std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::a));
	
	// Menu back
	mappings.emplace("menu_back", std::make_unique<input::key_mapping>(nullptr, input::scancode::escape, 0, false));
	mappings.emplace("menu_back", std::make_unique<input::key_mapping>(nullptr, input::scancode::backspace, 0, false));
	mappings.emplace("menu_back", std::make_unique<input::key_mapping>(nullptr, input::scancode::q, 0, false));
	mappings.emplace("menu_back", std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::b));
	mappings.emplace("menu_back", std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::back));
	
	// Menu modifier
	mappings.emplace("menu_modifier", std::make_unique<input::key_mapping>(nullptr, input::scancode::left_shift, 0, false));
	mappings.emplace("menu_modifier", std::make_unique<input::key_mapping>(nullptr, input::scancode::right_shift, 0, false));
	
	// Move forward
	mappings.emplace("move_forward", std::make_unique<input::key_mapping>(nullptr, input::scancode::w, 0, false));
	mappings.emplace("move_forward", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_stick_y, true));
	
	// Move back
	mappings.emplace("move_back", std::make_unique<input::key_mapping>(nullptr, input::scancode::s, 0, false));
	mappings.emplace("move_back", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_stick_y, false));
	
	// Move left
	mappings.emplace("move_left", std::make_unique<input::key_mapping>(nullptr, input::scancode::a, 0, false));
	mappings.emplace("move_left", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_stick_x, true));
	
	// Move right
	mappings.emplace("move_right", std::make_unique<input::key_mapping>(nullptr, input::scancode::d, 0, false));
	mappings.emplace("move_right", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_stick_x, false));
	
	// Move up
	mappings.emplace("move_up", std::make_unique<input::mouse_scroll_mapping>(nullptr, input::mouse_scroll_axis::y, false));
	mappings.emplace("move_up", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_trigger, false));
	
	// Move down
	mappings.emplace("move_down", std::make_unique<input::mouse_scroll_mapping>(nullptr, input::mouse_scroll_axis::y, true));
	mappings.emplace("move_down", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_trigger, false));
	
	// Interact
	mappings.emplace("interact", std::make_unique<input::key_mapping>(nullptr, input::scancode::e, 0, false));
	mappings.emplace("interact", std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::a));
	
	// Move fast
	// mappings.emplace("move_fast", std::make_unique<input::key_mapping>(nullptr, input::scancode::left_shift, 0, false));
	
	// Move slow
	mappings.emplace("move_slow", std::make_unique<input::key_mapping>(nullptr, input::scancode::left_ctrl, 0, false));
	
	// Camera mouse pick
	mappings.emplace("camera_mouse_pick", std::make_unique<input::mouse_button_mapping>(nullptr, input::mouse_button::left));
	
	// Camera mouse look
	mappings.emplace("camera_mouse_look", std::make_unique<input::mouse_button_mapping>(nullptr, input::mouse_button::right));
	
	// Camera mouse drag
	mappings.emplace("camera_mouse_drag", std::make_unique<input::mouse_button_mapping>(nullptr, input::mouse_button::middle));
	
	// Camera zoom
	mappings.emplace("camera_mouse_zoom", std::make_unique<input::key_mapping>(nullptr, input::scancode::z, 0, false));
	mappings.emplace("camera_zoom_in", std::make_unique<input::mouse_scroll_mapping>(nullptr, input::mouse_scroll_axis::y, false));
	mappings.emplace("camera_zoom_out", std::make_unique<input::mouse_scroll_mapping>(nullptr, input::mouse_scroll_axis::y, true));
	
	// Camera orbit
	mappings.emplace("camera_orbit_left", std::make_unique<input::key_mapping>(nullptr, input::scancode::left, 0, false));
	mappings.emplace("camera_orbit_left", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_stick_x, false));
	mappings.emplace("camera_orbit_right", std::make_unique<input::key_mapping>(nullptr, input::scancode::right, 0, false));
	mappings.emplace("camera_orbit_right", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_stick_x, true));
	mappings.emplace("camera_orbit_up", std::make_unique<input::key_mapping>(nullptr, input::scancode::up, 0, false));
	mappings.emplace("camera_orbit_up", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_stick_y, true));
	mappings.emplace("camera_orbit_down", std::make_unique<input::key_mapping>(nullptr, input::scancode::up, 0, false));
	mappings.emplace("camera_orbit_down", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::right_stick_y, false));
	
	// Camera look ahead
	mappings.emplace("camera_look_ahead", std::make_unique<input::key_mapping>(nullptr, input::scancode::left_shift, 0, false));
	mappings.emplace("camera_look_ahead", std::make_unique<input::gamepad_axis_mapping>(nullptr, input::gamepad_axis::left_trigger, false));
	
	// Camera presets 
	mappings.emplace("camera_preset_1", std::make_unique<input::key_mapping>(nullptr, input::scancode::digit_1, 0, false));
	mappings.emplace("camera_preset_2", std::make_unique<input::key_mapping>(nullptr, input::scancode::digit_2, 0, false));
	mappings.emplace("camera_preset_3", std::make_unique<input::key_mapping>(nullptr, input::scancode::digit_3, 0, false));
	mappings.emplace("camera_preset_4", std::make_unique<input::key_mapping>(nullptr, input::scancode::digit_4, 0, false));
	mappings.emplace("camera_preset_5", std::make_unique<input::key_mapping>(nullptr, input::scancode::digit_5, 0, false));
	mappings.emplace("camera_preset_6", std::make_unique<input::key_mapping>(nullptr, input::scancode::digit_6, 0, false));
	mappings.emplace("camera_preset_7", std::make_unique<input::key_mapping>(nullptr, input::scancode::digit_7, 0, false));
	mappings.emplace("camera_preset_8", std::make_unique<input::key_mapping>(nullptr, input::scancode::digit_8, 0, false));
	mappings.emplace("camera_preset_9", std::make_unique<input::key_mapping>(nullptr, input::scancode::digit_9, 0, false));
	mappings.emplace("camera_preset_10", std::make_unique<input::key_mapping>(nullptr, input::scancode::digit_0, 0, false));
	mappings.emplace("camera_save_preset", std::make_unique<input::key_mapping>(nullptr, input::scancode::left_ctrl, 0, false));
	mappings.emplace("camera_save_preset", std::make_unique<input::key_mapping>(nullptr, input::scancode::right_ctrl, 0, false));
	
	// Oviposit
	mappings.emplace("oviposit", std::make_unique<input::key_mapping>(nullptr, input::scancode::left_alt, 0, false));
	
	// Pause
	mappings.emplace("pause", std::make_unique<input::key_mapping>(nullptr, input::scancode::escape, 0, false));
	mappings.emplace("pause", std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::start));
	
	// Toggle debug UI
	mappings.emplace("toggle_debug", std::make_unique<input::key_mapping>(nullptr, input::scancode::grave, 0, false));
	
	// Adjust exposure
	mappings.emplace("adjust_exposure", std::make_unique<input::key_mapping>(nullptr, input::scancode::b, 0, false));
	
	// Adjust time
	mappings.emplace("adjust_time", std::make_unique<input::key_mapping>(nullptr, input::scancode::t, 0, false));
}

void apply_control_profile(::game& ctx, const ::control_profile& profile)
{
	auto add_mappings = [&profile](input::action_map& map, input::action& action, hash::fnv1a32_t key)
	{
		auto range = profile.mappings.equal_range(key);
		for (auto i = range.first; i != range.second; ++i)
		{
			map.add_mapping(action, *i->second);
		}
	};
	
	// Window controls
	ctx.window_action_map.remove_mappings();
	add_mappings(ctx.window_action_map, ctx.fullscreen_action, "fullscreen");
	add_mappings(ctx.window_action_map, ctx.screenshot_action, "screenshot");
	
	// Menu controls
	ctx.menu_action_map.remove_mappings();
	add_mappings(ctx.menu_action_map, ctx.menu_up_action, "menu_up");
	add_mappings(ctx.menu_action_map, ctx.menu_down_action, "menu_down");
	add_mappings(ctx.menu_action_map, ctx.menu_left_action, "menu_left");
	add_mappings(ctx.menu_action_map, ctx.menu_right_action, "menu_right");
	add_mappings(ctx.menu_action_map, ctx.menu_select_action, "menu_select");
	add_mappings(ctx.menu_action_map, ctx.menu_back_action, "menu_back");
	add_mappings(ctx.menu_action_map, ctx.menu_modifier_action, "menu_modifier");
	
	// Movement controls
	ctx.movement_action_map.remove_mappings();
	add_mappings(ctx.movement_action_map, ctx.move_forward_action, "move_forward");
	add_mappings(ctx.movement_action_map, ctx.move_back_action, "move_back");
	add_mappings(ctx.movement_action_map, ctx.move_left_action, "move_left");
	add_mappings(ctx.movement_action_map, ctx.move_right_action, "move_right");
	add_mappings(ctx.movement_action_map, ctx.move_up_action, "move_up");
	add_mappings(ctx.movement_action_map, ctx.move_down_action, "move_down");
	add_mappings(ctx.movement_action_map, ctx.move_fast_action, "move_fast");
	add_mappings(ctx.movement_action_map, ctx.move_slow_action, "move_slow");
	add_mappings(ctx.movement_action_map, ctx.pause_action, "pause");
	
	// Camera controls
	ctx.camera_action_map.remove_mappings();
	add_mappings(ctx.camera_action_map, ctx.camera_mouse_pick_action, "camera_mouse_pick");
	add_mappings(ctx.camera_action_map, ctx.camera_mouse_look_action, "camera_mouse_look");
	add_mappings(ctx.camera_action_map, ctx.camera_mouse_drag_action, "camera_mouse_drag");
	add_mappings(ctx.camera_action_map, ctx.camera_mouse_zoom_action, "camera_mouse_zoom");
	add_mappings(ctx.camera_action_map, ctx.camera_zoom_in_action, "camera_zoom_in");
	add_mappings(ctx.camera_action_map, ctx.camera_zoom_out_action, "camera_zoom_out");
	add_mappings(ctx.camera_action_map, ctx.camera_orbit_left_action, "camera_orbit_left");
	add_mappings(ctx.camera_action_map, ctx.camera_orbit_right_action, "camera_orbit_right");
	add_mappings(ctx.camera_action_map, ctx.camera_orbit_up_action, "camera_orbit_up");
	add_mappings(ctx.camera_action_map, ctx.camera_orbit_down_action, "camera_orbit_down");
	add_mappings(ctx.camera_action_map, ctx.camera_look_ahead_action, "camera_look_ahead");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_1_action, "camera_preset_1");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_2_action, "camera_preset_2");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_3_action, "camera_preset_3");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_4_action, "camera_preset_4");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_5_action, "camera_preset_5");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_6_action, "camera_preset_6");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_7_action, "camera_preset_7");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_8_action, "camera_preset_8");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_9_action, "camera_preset_9");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_10_action, "camera_preset_10");
	add_mappings(ctx.camera_action_map, ctx.camera_save_preset_action, "camera_save_preset");
	
	// Ant controls
	ctx.ant_action_map.remove_mappings();
	add_mappings(ctx.ant_action_map, ctx.ant_move_forward_action, "move_forward");
	add_mappings(ctx.ant_action_map, ctx.ant_move_back_action, "move_back");
	add_mappings(ctx.ant_action_map, ctx.ant_move_left_action, "move_left");
	add_mappings(ctx.ant_action_map, ctx.ant_move_right_action, "move_right");
	add_mappings(ctx.ant_action_map, ctx.ant_move_fast_action, "move_fast");
	add_mappings(ctx.ant_action_map, ctx.ant_move_slow_action, "move_slow");
	add_mappings(ctx.ant_action_map, ctx.ant_interact_action, "interact");
	add_mappings(ctx.ant_action_map, ctx.ant_oviposit_action, "oviposit");
	
	// Debug controls
	add_mappings(ctx.debug_action_map, ctx.toggle_debug_ui_action, "toggle_debug");
	add_mappings(ctx.debug_action_map, ctx.adjust_exposure_action, "adjust_exposure");
	add_mappings(ctx.debug_action_map, ctx.adjust_time_action, "adjust_time");
}

void update_control_profile(::game& ctx, ::control_profile& profile)
{
	auto add_mappings = [&profile](const input::action_map& map, const input::action& action, hash::fnv1a32_t key)
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
	add_mappings(ctx.window_action_map, ctx.fullscreen_action, "fullscreen");
	add_mappings(ctx.window_action_map, ctx.screenshot_action, "screenshot");
	
	// Menu controls
	add_mappings(ctx.menu_action_map, ctx.menu_up_action, "menu_up");
	add_mappings(ctx.menu_action_map, ctx.menu_down_action, "menu_down");
	add_mappings(ctx.menu_action_map, ctx.menu_left_action, "menu_left");
	add_mappings(ctx.menu_action_map, ctx.menu_right_action, "menu_right");
	add_mappings(ctx.menu_action_map, ctx.menu_select_action, "menu_select");
	add_mappings(ctx.menu_action_map, ctx.menu_back_action, "menu_back");
	add_mappings(ctx.menu_action_map, ctx.menu_modifier_action, "menu_modifier");
	
	// Movement controls
	add_mappings(ctx.movement_action_map, ctx.move_forward_action, "move_forward");
	add_mappings(ctx.movement_action_map, ctx.move_back_action, "move_back");
	add_mappings(ctx.movement_action_map, ctx.move_left_action, "move_left");
	add_mappings(ctx.movement_action_map, ctx.move_right_action, "move_right");
	add_mappings(ctx.movement_action_map, ctx.move_up_action, "move_up");
	add_mappings(ctx.movement_action_map, ctx.move_down_action, "move_down");
	add_mappings(ctx.movement_action_map, ctx.move_fast_action, "move_fast");
	add_mappings(ctx.movement_action_map, ctx.move_slow_action, "move_slow");
	add_mappings(ctx.movement_action_map, ctx.pause_action, "pause");
	
	// Ant controls
	add_mappings(ctx.ant_action_map, ctx.ant_move_forward_action, "move_forward");
	add_mappings(ctx.ant_action_map, ctx.ant_move_back_action, "move_back");
	add_mappings(ctx.ant_action_map, ctx.ant_move_left_action, "move_left");
	add_mappings(ctx.ant_action_map, ctx.ant_move_right_action, "move_right");
	add_mappings(ctx.ant_action_map, ctx.ant_move_fast_action, "move_fast");
	add_mappings(ctx.ant_action_map, ctx.ant_move_slow_action, "move_slow");
	add_mappings(ctx.ant_action_map, ctx.ant_interact_action, "interact");
	add_mappings(ctx.ant_action_map, ctx.ant_oviposit_action, "oviposit");
	
	// Camera controls
	add_mappings(ctx.camera_action_map, ctx.camera_mouse_pick_action, "camera_mouse_pick");
	add_mappings(ctx.camera_action_map, ctx.camera_mouse_look_action, "camera_mouse_look");
	add_mappings(ctx.camera_action_map, ctx.camera_mouse_drag_action, "camera_mouse_drag");
	add_mappings(ctx.camera_action_map, ctx.camera_mouse_zoom_action, "camera_mouse_zoom");
	add_mappings(ctx.camera_action_map, ctx.camera_zoom_in_action, "camera_zoom_in");
	add_mappings(ctx.camera_action_map, ctx.camera_zoom_out_action, "camera_zoom_out");
	add_mappings(ctx.camera_action_map, ctx.camera_orbit_left_action, "camera_orbit_left");
	add_mappings(ctx.camera_action_map, ctx.camera_orbit_right_action, "camera_orbit_right");
	add_mappings(ctx.camera_action_map, ctx.camera_orbit_up_action, "camera_orbit_up");
	add_mappings(ctx.camera_action_map, ctx.camera_orbit_down_action, "camera_orbit_down");
	add_mappings(ctx.camera_action_map, ctx.camera_look_ahead_action, "camera_look_ahead");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_1_action, "camera_preset_1");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_2_action, "camera_preset_2");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_3_action, "camera_preset_3");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_4_action, "camera_preset_4");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_5_action, "camera_preset_5");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_6_action, "camera_preset_6");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_7_action, "camera_preset_7");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_8_action, "camera_preset_8");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_9_action, "camera_preset_9");
	add_mappings(ctx.camera_action_map, ctx.camera_preset_10_action, "camera_preset_10");
	add_mappings(ctx.camera_action_map, ctx.camera_save_preset_action, "camera_save_preset");
	
	// Debug controls
	add_mappings(ctx.debug_action_map, ctx.toggle_debug_ui_action, "toggle_debug");
	add_mappings(ctx.debug_action_map, ctx.adjust_exposure_action, "adjust_exposure");
	add_mappings(ctx.debug_action_map, ctx.adjust_time_action, "adjust_time");
}

void setup_game_controls(::game& ctx)
{
	// Setup pause control
	ctx.movement_action_subscriptions.emplace_back
	(
		ctx.pause_action.get_activated_channel().subscribe
		(
			[&ctx](const auto& event)
			{
				if (!ctx.resume_callback)
				{
					// Queue disable game controls and push pause state
					ctx.function_queue.push
					(
						[&ctx]()
						{
							// Disable game controls
							::disable_game_controls(ctx);
							
							// Push pause menu state
							ctx.state_machine.emplace(std::make_unique<pause_menu_state>(ctx));
						}
					);
					
					// Set resume callback
					ctx.resume_callback = [&ctx]()
					{
						enable_game_controls(ctx);
						ctx.resume_callback = nullptr;
					};
				}
			}
		)
	);
}

void enable_game_controls(::game& ctx)
{
	ctx.movement_action_map.enable();
}

void disable_game_controls(::game& ctx)
{
	ctx.movement_action_map.disable();
	
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
