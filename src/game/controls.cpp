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

#include "game/controls.hpp"
#include "game/graphics.hpp"
#include "game/menu.hpp"
#include "game/control-profile.hpp"
#include "game/states/pause-menu-state.hpp"
#include <engine/resources/resource-manager.hpp>
#include <engine/utility/json.hpp>
#include <engine/input/modifier-key.hpp>
#include <engine/utility/hash/fnv1a.hpp>


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
	
	// Pause
	mappings.emplace("pause", std::make_unique<input::key_mapping>(nullptr, input::scancode::escape, 0, false));
	mappings.emplace("pause", std::make_unique<input::gamepad_button_mapping>(nullptr, input::gamepad_button::start));
	
	// Mouse pick
	mappings.emplace("mouse_pick", std::make_unique<input::mouse_button_mapping>(nullptr, input::mouse_button::left));
	
	// Mouse look
	mappings.emplace("mouse_look", std::make_unique<input::mouse_button_mapping>(nullptr, input::mouse_button::right));
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
	add_mappings(ctx.movement_action_map, ctx.pause_action, "pause");
	
	// Keeper controls
	ctx.keeper_action_map.remove_mappings();
	add_mappings(ctx.keeper_action_map, ctx.mouse_pick_action, "mouse_pick");
	add_mappings(ctx.keeper_action_map, ctx.mouse_look_action, "mouse_look");
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
	add_mappings(ctx.movement_action_map, ctx.pause_action, "pause");
	
	// Keeper controls
	add_mappings(ctx.keeper_action_map, ctx.mouse_pick_action, "mouse_pick");
	add_mappings(ctx.keeper_action_map, ctx.mouse_look_action, "mouse_look");
}

void setup_window_controls(::game& ctx)
{
	// Setup fullscreen control
	ctx.window_action_subscriptions.emplace_back
	(
		ctx.fullscreen_action.get_activated_channel().subscribe
		(
			[&ctx](const auto& event)
			{
				ctx.window->set_fullscreen(!ctx.window->is_fullscreen());
			}
		)
	);
	
	// Setup screenshot control
	ctx.window_action_subscriptions.emplace_back
	(
		ctx.screenshot_action.get_activated_channel().subscribe
		(
			[&ctx](const auto& event)
			{
				::graphics::save_screenshot(ctx);
			}
		)
	);
}

void setup_menu_controls(::game& ctx)
{
	// Setup menu controls
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_up_action.get_activated_channel().subscribe
		(
			[&ctx](const auto& event)
			{
				--(*ctx.menu_item_index);
				if (*ctx.menu_item_index < 0)
					*ctx.menu_item_index = static_cast<int>(ctx.menu_item_texts.size()) - 1;
				
				::menu::update_text_color(ctx);
			}
		)
	);
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_down_action.get_activated_channel().subscribe
		(
			[&ctx](const auto& event)
			{
				++(*ctx.menu_item_index);
				if (*ctx.menu_item_index >= ctx.menu_item_texts.size())
					*ctx.menu_item_index = 0;
				
				::menu::update_text_color(ctx);
			}
		)
	);
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_left_action.get_activated_channel().subscribe
		(
			[&ctx](const auto& event)
			{
				auto callback = ctx.menu_left_callbacks[*ctx.menu_item_index];
				if (callback != nullptr)
					callback();
			}
		)
	);
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_right_action.get_activated_channel().subscribe
		(
			[&ctx](const auto& event)
			{
				auto callback = ctx.menu_right_callbacks[*ctx.menu_item_index];
				if (callback != nullptr)
					callback();
			}
		)
	);
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_select_action.get_activated_channel().subscribe
		(
			[&ctx](const auto& event)
			{
				const auto& callback = ctx.menu_select_callbacks[*ctx.menu_item_index];
				if (callback != nullptr)
					callback();
			}
		)
	);
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_back_action.get_activated_channel().subscribe
		(
			[&ctx](const auto& event)
			{
				if (ctx.menu_back_callback != nullptr)
					ctx.menu_back_callback();
			}
		)
	);
	
	// Set activation threshold for menu navigation controls to mitigate drifting gamepad axes
	auto menu_action_threshold = [](float x) -> bool
	{
		return x > 0.5f;
	};
	ctx.menu_up_action.set_threshold_function(menu_action_threshold);
	ctx.menu_down_action.set_threshold_function(menu_action_threshold);
	ctx.menu_left_action.set_threshold_function(menu_action_threshold);
	ctx.menu_right_action.set_threshold_function(menu_action_threshold);
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

void enable_window_controls(::game& ctx)
{
	ctx.window_action_map.enable();
}

void enable_menu_controls(::game& ctx)
{
	ctx.menu_action_map.enable();
	
	// Function to select menu item at mouse position
	auto select_menu_item = [&ctx](const math::vector<float, 2>& mouse_position) -> bool
	{
		const float padding = config::menu_mouseover_padding * ctx.menu_font.get_font_metrics().size;
		
		for (std::size_t i = 0; i < ctx.menu_item_texts.size(); ++i)
		{
			auto [name, value] = ctx.menu_item_texts[i];
			
			const auto& name_bounds = static_cast<const geom::aabb<float>&>(name->get_world_bounds());
			float min_x = name_bounds.min_point.x();
			float min_y = name_bounds.min_point.y();
			float max_x = name_bounds.max_point.x();
			float max_y = name_bounds.max_point.y();
			
			if (value)
			{
				const auto& value_bounds = static_cast<const geom::aabb<float>&>(value->get_world_bounds());
				min_x = std::min<float>(min_x, value_bounds.min_point.x());
				min_y = std::min<float>(min_y, value_bounds.min_point.y());
				max_x = std::max<float>(max_x, value_bounds.max_point.x());
				max_y = std::max<float>(max_y, value_bounds.max_point.y());
			}
			
			min_x -= padding;
			min_y -= padding;
			max_x += padding;
			max_y += padding;
			
			const auto& viewport = ctx.window->get_viewport_size();
			const float x = mouse_position.x();
			const float y = static_cast<float>((viewport[1] - mouse_position.y() + 1));
			
			if (x >= min_x && x <= max_x)
			{
				if (y >= min_y && y <= max_y)
				{
					*ctx.menu_item_index = static_cast<int>(i);
					::menu::update_text_color(ctx);
					return true;
				}
			}
		}
		
		return false;
	};
	
	// Enable menu mouse tracking
	ctx.menu_mouse_subscriptions.clear();
	ctx.menu_mouse_subscriptions.emplace_back
	(
		ctx.input_manager->get_event_queue().subscribe<input::mouse_moved_event>
		(
			[&ctx, select_menu_item](const auto& event)
			{
				// Select menu item at mouse position (if any)
				select_menu_item(math::vector<float, 2>(event.position));
			}
		)
	);
	ctx.menu_mouse_subscriptions.emplace_back
	(
		ctx.input_manager->get_event_queue().subscribe<input::mouse_button_pressed_event>
		(
			[&ctx, select_menu_item](const auto& event)
			{
				// Select menu item at mouse position (if any)
				if (select_menu_item(math::vector<float, 2>(event.position)))
				{
					// Determine appropriate menu item callback
					auto callback = ctx.menu_select_callbacks[*ctx.menu_item_index];
					if (event.button == input::mouse_button::left)
					{
						if (ctx.menu_left_callbacks[*ctx.menu_item_index])
						{
							callback = ctx.menu_left_callbacks[*ctx.menu_item_index];
						}
					}
					else if (event.button == input::mouse_button::right)
					{
						if (ctx.menu_right_callbacks[*ctx.menu_item_index])
						{
							callback = ctx.menu_right_callbacks[*ctx.menu_item_index];
						}
					}
					
					// Invoke menu item callback
					if (callback)
					{
						callback();
					}
				}
			}
		)
	);
}

void enable_game_controls(::game& ctx)
{
	ctx.movement_action_map.enable();
}

void enable_keeper_controls(::game& ctx)
{
	ctx.keeper_action_map.enable();
}

void disable_window_controls(::game& ctx)
{
	ctx.window_action_map.disable();
}

void disable_menu_controls(::game& ctx)
{
	ctx.menu_action_map.disable();
	
	ctx.menu_mouse_subscriptions.clear();
	
	ctx.menu_up_action.reset();
	ctx.menu_down_action.reset();
	ctx.menu_left_action.reset();
	ctx.menu_right_action.reset();
	ctx.menu_select_action.reset();
	ctx.menu_back_action.reset();
	ctx.menu_modifier_action.reset();
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
	ctx.pause_action.reset();
}

void disable_keeper_controls(::game& ctx)
{
	ctx.keeper_action_map.disable();
	
	ctx.mouse_pick_action.reset();
	ctx.mouse_look_action.reset();
}

