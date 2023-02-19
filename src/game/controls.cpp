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
#include "game/state/pause-menu.hpp"
#include "resources/resource-manager.hpp"
#include "resources/json.hpp"
#include "input/modifier-key.hpp"
#include "utility/hash/fnv1a.hpp"

using namespace hash::literals;

namespace game {

void reset_control_profile(game::control_profile& profile)
{
	auto& mappings = profile.mappings;
	auto& settings = profile.settings;
	
	mappings.clear();
	settings.clear();
	
	// Window controls
	mappings.emplace("fullscreen"_fnv1a32, new input::key_mapping(nullptr, input::scancode::f11, 0, false));
	mappings.emplace("fullscreen"_fnv1a32, new input::key_mapping(nullptr, input::scancode::enter, input::modifier_key::alt, false));
	mappings.emplace("screenshot"_fnv1a32, new input::key_mapping(nullptr, input::scancode::f12, 0, false));
	mappings.emplace("screenshot"_fnv1a32, new input::key_mapping(nullptr, input::scancode::print_screen, 0, false));
	
	// Menu controls
	mappings.emplace("menu_up"_fnv1a32, new input::key_mapping(nullptr, input::scancode::up, 0, true));
	mappings.emplace("menu_up"_fnv1a32, new input::key_mapping(nullptr, input::scancode::w, 0, true));
	mappings.emplace("menu_up"_fnv1a32, new input::key_mapping(nullptr, input::scancode::i, 0, true));
	mappings.emplace("menu_up"_fnv1a32, new input::gamepad_axis_mapping(nullptr, input::gamepad_axis::left_stick_y, true));
	mappings.emplace("menu_up"_fnv1a32, new input::gamepad_axis_mapping(nullptr, input::gamepad_axis::right_stick_y, true));
	mappings.emplace("menu_up"_fnv1a32, new input::gamepad_button_mapping(nullptr, input::gamepad_button::dpad_up));
	mappings.emplace("menu_down"_fnv1a32, new input::key_mapping(nullptr, input::scancode::down, 0, true));
	mappings.emplace("menu_down"_fnv1a32, new input::key_mapping(nullptr, input::scancode::s, 0, true));
	mappings.emplace("menu_down"_fnv1a32, new input::key_mapping(nullptr, input::scancode::k, 0, true));
	mappings.emplace("menu_down"_fnv1a32, new input::gamepad_axis_mapping(nullptr, input::gamepad_axis::left_stick_y, false));
	mappings.emplace("menu_down"_fnv1a32, new input::gamepad_axis_mapping(nullptr, input::gamepad_axis::right_stick_y, false));
	mappings.emplace("menu_down"_fnv1a32, new input::gamepad_button_mapping(nullptr, input::gamepad_button::dpad_down));
	mappings.emplace("menu_left"_fnv1a32, new input::key_mapping(nullptr, input::scancode::left, 0, true));
	mappings.emplace("menu_left"_fnv1a32, new input::key_mapping(nullptr, input::scancode::a, 0, true));
	mappings.emplace("menu_left"_fnv1a32, new input::key_mapping(nullptr, input::scancode::j, 0, true));
	mappings.emplace("menu_left"_fnv1a32, new input::gamepad_axis_mapping(nullptr, input::gamepad_axis::left_stick_x, true));
	mappings.emplace("menu_left"_fnv1a32, new input::gamepad_axis_mapping(nullptr, input::gamepad_axis::right_stick_x, true));
	mappings.emplace("menu_left"_fnv1a32, new input::gamepad_button_mapping(nullptr, input::gamepad_button::dpad_left));
	mappings.emplace("menu_left"_fnv1a32, new input::mouse_scroll_mapping(nullptr, input::mouse_scroll_axis::x, true));
	mappings.emplace("menu_left"_fnv1a32, new input::mouse_scroll_mapping(nullptr, input::mouse_scroll_axis::y, true));
	mappings.emplace("menu_right"_fnv1a32, new input::key_mapping(nullptr, input::scancode::right, 0, true));
	mappings.emplace("menu_right"_fnv1a32, new input::key_mapping(nullptr, input::scancode::d, 0, true));
	mappings.emplace("menu_right"_fnv1a32, new input::key_mapping(nullptr, input::scancode::l, 0, true));
	mappings.emplace("menu_right"_fnv1a32, new input::gamepad_axis_mapping(nullptr, input::gamepad_axis::left_stick_x, false));
	mappings.emplace("menu_right"_fnv1a32, new input::gamepad_axis_mapping(nullptr, input::gamepad_axis::right_stick_x, false));
	mappings.emplace("menu_right"_fnv1a32, new input::gamepad_button_mapping(nullptr, input::gamepad_button::dpad_right));
	mappings.emplace("menu_right"_fnv1a32, new input::mouse_scroll_mapping(nullptr, input::mouse_scroll_axis::x, false));
	mappings.emplace("menu_right"_fnv1a32, new input::mouse_scroll_mapping(nullptr, input::mouse_scroll_axis::y, false));
	mappings.emplace("menu_select"_fnv1a32, new input::key_mapping(nullptr, input::scancode::enter, 0, false));
	mappings.emplace("menu_select"_fnv1a32, new input::key_mapping(nullptr, input::scancode::space, 0, false));
	mappings.emplace("menu_select"_fnv1a32, new input::key_mapping(nullptr, input::scancode::e, 0, false));
	mappings.emplace("menu_select"_fnv1a32, new input::gamepad_button_mapping(nullptr, input::gamepad_button::a));
	mappings.emplace("menu_back"_fnv1a32, new input::key_mapping(nullptr, input::scancode::escape, 0, false));
	mappings.emplace("menu_back"_fnv1a32, new input::key_mapping(nullptr, input::scancode::backspace, 0, false));
	mappings.emplace("menu_back"_fnv1a32, new input::key_mapping(nullptr, input::scancode::q, 0, false));
	mappings.emplace("menu_back"_fnv1a32, new input::gamepad_button_mapping(nullptr, input::gamepad_button::b));
	mappings.emplace("menu_back"_fnv1a32, new input::gamepad_button_mapping(nullptr, input::gamepad_button::back));
	mappings.emplace("menu_modifier"_fnv1a32, new input::key_mapping(nullptr, input::scancode::left_shift, 0, false));
	mappings.emplace("menu_modifier"_fnv1a32, new input::key_mapping(nullptr, input::scancode::right_shift, 0, false));
	
	// Movement controls
	mappings.emplace("move_forward"_fnv1a32, new input::key_mapping(nullptr, input::scancode::w, 0, false));
	mappings.emplace("move_forward"_fnv1a32, new input::gamepad_axis_mapping(nullptr, input::gamepad_axis::left_stick_y, true));
	mappings.emplace("move_back"_fnv1a32, new input::key_mapping(nullptr, input::scancode::s, 0, false));
	mappings.emplace("move_back"_fnv1a32, new input::gamepad_axis_mapping(nullptr, input::gamepad_axis::left_stick_y, false));
	mappings.emplace("move_left"_fnv1a32, new input::key_mapping(nullptr, input::scancode::a, 0, false));
	mappings.emplace("move_left"_fnv1a32, new input::gamepad_axis_mapping(nullptr, input::gamepad_axis::left_stick_x, true));
	mappings.emplace("move_right"_fnv1a32, new input::key_mapping(nullptr, input::scancode::d, 0, false));
	mappings.emplace("move_right"_fnv1a32, new input::gamepad_axis_mapping(nullptr, input::gamepad_axis::left_stick_x, false));
	mappings.emplace("move_up"_fnv1a32, new input::mouse_scroll_mapping(nullptr, input::mouse_scroll_axis::y, false));
	mappings.emplace("move_up"_fnv1a32, new input::gamepad_axis_mapping(nullptr, input::gamepad_axis::right_trigger, false));
	mappings.emplace("move_down"_fnv1a32, new input::mouse_scroll_mapping(nullptr, input::mouse_scroll_axis::y, true));
	mappings.emplace("move_down"_fnv1a32, new input::gamepad_axis_mapping(nullptr, input::gamepad_axis::left_trigger, false));
	mappings.emplace("pause"_fnv1a32, new input::key_mapping(nullptr, input::scancode::escape, 0, false));
	mappings.emplace("pause"_fnv1a32, new input::gamepad_button_mapping(nullptr, input::gamepad_button::start));
}

void apply_control_profile(game::context& ctx, const game::control_profile& profile)
{
	auto add_mappings = [&profile](input::action_map& map, input::action& action, std::uint32_t key)
	{
		auto range = profile.mappings.equal_range(key);
		for (auto i = range.first; i != range.second; ++i)
		{
			map.add_mapping(action, *i->second);
		}
	};
	
	// Window controls
	ctx.window_actions.remove_mappings();
	add_mappings(ctx.window_actions, ctx.fullscreen_action, "fullscreen"_fnv1a32);
	add_mappings(ctx.window_actions, ctx.screenshot_action, "screenshot"_fnv1a32);
	
	// Menu controls
	ctx.menu_actions.remove_mappings();
	add_mappings(ctx.menu_actions, ctx.menu_up_action, "menu_up"_fnv1a32);
	add_mappings(ctx.menu_actions, ctx.menu_down_action, "menu_down"_fnv1a32);
	add_mappings(ctx.menu_actions, ctx.menu_left_action, "menu_left"_fnv1a32);
	add_mappings(ctx.menu_actions, ctx.menu_right_action, "menu_right"_fnv1a32);
	add_mappings(ctx.menu_actions, ctx.menu_select_action, "menu_select"_fnv1a32);
	add_mappings(ctx.menu_actions, ctx.menu_back_action, "menu_back"_fnv1a32);
	add_mappings(ctx.menu_actions, ctx.menu_modifier_action, "menu_modifier"_fnv1a32);
	
	// Movement controls
	ctx.movement_actions.remove_mappings();
	add_mappings(ctx.movement_actions, ctx.move_forward_action, "move_forward"_fnv1a32);
	add_mappings(ctx.movement_actions, ctx.move_back_action, "move_back"_fnv1a32);
	add_mappings(ctx.movement_actions, ctx.move_left_action, "move_left"_fnv1a32);
	add_mappings(ctx.movement_actions, ctx.move_right_action, "move_right"_fnv1a32);
	add_mappings(ctx.movement_actions, ctx.move_up_action, "move_up"_fnv1a32);
	add_mappings(ctx.movement_actions, ctx.move_down_action, "move_down"_fnv1a32);
	add_mappings(ctx.movement_actions, ctx.pause_action, "pause"_fnv1a32);
}

void update_control_profile(game::context& ctx, game::control_profile& profile)
{
	auto add_mappings = [&profile](const input::action_map& map, const input::action& action, std::uint32_t key)
	{
		auto gamepad_axis_mappings = map.get_gamepad_axis_mappings(action);
		auto gamepad_button_mappings = map.get_gamepad_button_mappings(action);
		auto key_mappings = map.get_key_mappings(action);
		auto mouse_button_mappings = map.get_mouse_button_mappings(action);
		auto mouse_motion_mappings = map.get_mouse_motion_mappings(action);
		auto mouse_scroll_mappings = map.get_mouse_scroll_mappings(action);
		
		for (const auto& mapping: gamepad_axis_mappings)
		{
			profile.mappings.emplace(key, new input::gamepad_axis_mapping(mapping));
		}
		for (const auto& mapping: gamepad_button_mappings)
		{
			profile.mappings.emplace(key, new input::gamepad_button_mapping(mapping));
		}
		for (const auto& mapping: key_mappings)
		{
			profile.mappings.emplace(key, new input::key_mapping(mapping));
		}
		for (const auto& mapping: mouse_button_mappings)
		{
			profile.mappings.emplace(key, new input::mouse_button_mapping(mapping));
		}
		for (const auto& mapping: mouse_motion_mappings)
		{
			profile.mappings.emplace(key, new input::mouse_motion_mapping(mapping));
		}
		for (const auto& mapping: mouse_scroll_mappings)
		{
			profile.mappings.emplace(key, new input::mouse_scroll_mapping(mapping));
		}
	};
	
	profile.mappings.clear();
	
	// Window controls
	add_mappings(ctx.window_actions, ctx.fullscreen_action, "fullscreen"_fnv1a32);
	add_mappings(ctx.window_actions, ctx.screenshot_action, "screenshot"_fnv1a32);
	
	// Menu controls
	add_mappings(ctx.menu_actions, ctx.menu_up_action, "menu_up"_fnv1a32);
	add_mappings(ctx.menu_actions, ctx.menu_down_action, "menu_down"_fnv1a32);
	add_mappings(ctx.menu_actions, ctx.menu_left_action, "menu_left"_fnv1a32);
	add_mappings(ctx.menu_actions, ctx.menu_right_action, "menu_right"_fnv1a32);
	add_mappings(ctx.menu_actions, ctx.menu_select_action, "menu_select"_fnv1a32);
	add_mappings(ctx.menu_actions, ctx.menu_back_action, "menu_back"_fnv1a32);
	add_mappings(ctx.menu_actions, ctx.menu_modifier_action, "menu_modifier"_fnv1a32);
	
	// Movement controls
	add_mappings(ctx.movement_actions, ctx.move_forward_action, "move_forward"_fnv1a32);
	add_mappings(ctx.movement_actions, ctx.move_back_action, "move_back"_fnv1a32);
	add_mappings(ctx.movement_actions, ctx.move_left_action, "move_left"_fnv1a32);
	add_mappings(ctx.movement_actions, ctx.move_right_action, "move_right"_fnv1a32);
	add_mappings(ctx.movement_actions, ctx.move_up_action, "move_up"_fnv1a32);
	add_mappings(ctx.movement_actions, ctx.move_down_action, "move_down"_fnv1a32);
	add_mappings(ctx.movement_actions, ctx.pause_action, "pause"_fnv1a32);
}

void setup_window_controls(game::context& ctx)
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
				game::graphics::save_screenshot(ctx);
			}
		)
	);
}

void setup_menu_controls(game::context& ctx)
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
				
				game::menu::update_text_color(ctx);
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
				
				game::menu::update_text_color(ctx);
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

void setup_game_controls(game::context& ctx)
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
							game::disable_game_controls(ctx);
							
							// Push pause menu state
							ctx.state_machine.emplace(new game::state::pause_menu(ctx));
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

void enable_window_controls(game::context& ctx)
{
	ctx.window_actions.connect(ctx.input_manager->get_event_queue());
}

void enable_menu_controls(game::context& ctx)
{
	ctx.menu_actions.connect(ctx.input_manager->get_event_queue());
	
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
					game::menu::update_text_color(ctx);
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

void enable_game_controls(game::context& ctx)
{
	ctx.movement_actions.connect(ctx.input_manager->get_event_queue());
}

void disable_window_controls(game::context& ctx)
{
	ctx.window_actions.disconnect();
}

void disable_menu_controls(game::context& ctx)
{
	// Reset menu action states
	ctx.menu_up_action.reset();
	ctx.menu_down_action.reset();
	ctx.menu_left_action.reset();
	ctx.menu_right_action.reset();
	ctx.menu_select_action.reset();
	ctx.menu_back_action.reset();
	ctx.menu_modifier_action.reset();
	
	ctx.menu_actions.disconnect();
	ctx.menu_mouse_subscriptions.clear();
}

void disable_game_controls(game::context& ctx)
{
	ctx.movement_actions.disconnect();
	
	ctx.move_forward_action.reset();
	ctx.move_back_action.reset();
	ctx.move_left_action.reset();
	ctx.move_right_action.reset();
	ctx.move_up_action.reset();
	ctx.move_down_action.reset();
	ctx.pause_action.reset();
}

} // namespace game
