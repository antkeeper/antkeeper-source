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

#include "game/states/gamepad-config-menu.hpp"
#include "game/states/controls-menu.hpp"
#include "application.hpp"
#include "scene/text.hpp"
#include "debug/logger.hpp"
#include "resources/resource-manager.hpp"
#include "game/menu.hpp"
#include "game/controls.hpp"
#include "animation/timeline.hpp"

namespace game {
namespace state {
namespace gamepad_config_menu {

static std::string get_binding_string(game::context* ctx, input::control* control)
{
	std::string binding_string;
	
	auto mappings = ctx->input_event_router->get_mappings(control);
	for (input::mapping* mapping: *mappings)
	{
		std::string mapping_string;
		
		switch (mapping->get_type())
		{
			case input::mapping_type::gamepad_axis:
			{
				const input::gamepad_axis_mapping* axis_mapping = static_cast<const input::gamepad_axis_mapping*>(mapping);
				
				switch (axis_mapping->axis)
				{
					case input::gamepad_axis::left_x:
						if (axis_mapping->negative)
							mapping_string = (*ctx->strings)["gamepad_left_stick_left"];
						else
							mapping_string = (*ctx->strings)["gamepad_left_stick_right"];
						break;
					
					case input::gamepad_axis::left_y:
						if (axis_mapping->negative)
							mapping_string = (*ctx->strings)["gamepad_left_stick_up"];
						else
							mapping_string = (*ctx->strings)["gamepad_left_stick_down"];
						break;
					
					case input::gamepad_axis::right_x:
						if (axis_mapping->negative)
							mapping_string = (*ctx->strings)["gamepad_right_stick_left"];
						else
							mapping_string = (*ctx->strings)["gamepad_right_stick_right"];
						break;
					
					case input::gamepad_axis::right_y:
						if (axis_mapping->negative)
							mapping_string = (*ctx->strings)["gamepad_right_stick_up"];
						else
							mapping_string = (*ctx->strings)["gamepad_right_stick_down"];
						break;
					
					case input::gamepad_axis::left_trigger:
						mapping_string = (*ctx->strings)["gamepad_left_trigger"];
						break;
						
					case input::gamepad_axis::right_trigger:
						mapping_string = (*ctx->strings)["gamepad_right_trigger"];
						break;
					
					default:
						break;
				}
				break;
			}
			
			case input::mapping_type::gamepad_button:
			{
				const input::gamepad_button_mapping* button_mapping = static_cast<const input::gamepad_button_mapping*>(mapping);
				
				switch (button_mapping->button)
				{
					case input::gamepad_button::a:
						mapping_string = (*ctx->strings)["gamepad_button_a"];
						break;
					
					case input::gamepad_button::b:
						mapping_string = (*ctx->strings)["gamepad_button_b"];
						break;
					
					case input::gamepad_button::x:
						mapping_string = (*ctx->strings)["gamepad_button_x"];
						break;
					
					case input::gamepad_button::y:
						mapping_string = (*ctx->strings)["gamepad_button_y"];
						break;
					
					case input::gamepad_button::back:
						mapping_string = (*ctx->strings)["gamepad_button_back"];
						break;
					
					case input::gamepad_button::guide:
						mapping_string = (*ctx->strings)["gamepad_button_guide"];
						break;
					
					case input::gamepad_button::start:
						mapping_string = (*ctx->strings)["gamepad_button_start"];
						break;
					
					case input::gamepad_button::left_stick:
						mapping_string = (*ctx->strings)["gamepad_button_left_stick"];
						break;
					
					case input::gamepad_button::right_stick:
						mapping_string = (*ctx->strings)["gamepad_button_right_stick"];
						break;
					
					case input::gamepad_button::left_shoulder:
						mapping_string = (*ctx->strings)["gamepad_button_left_shoulder"];
						break;
					
					case input::gamepad_button::right_shoulder:
						mapping_string = (*ctx->strings)["gamepad_button_right_shoulder"];
						break;
					
					case input::gamepad_button::dpad_up:
						mapping_string = (*ctx->strings)["gamepad_button_dpad_up"];
						break;
					
					case input::gamepad_button::dpad_down:
						mapping_string = (*ctx->strings)["gamepad_button_dpad_down"];
						break;
					
					case input::gamepad_button::dpad_left:
						mapping_string = (*ctx->strings)["gamepad_button_dpad_left"];
						break;
					
					case input::gamepad_button::dpad_right:
						mapping_string = (*ctx->strings)["gamepad_button_dpad_right"];
						break;
					
					default:
						break;
				}
				break;
			}
			
			default:
				break;
		}
		
		if (!mapping_string.empty())
		{
			if (binding_string.empty())
			{
				binding_string = mapping_string;
			}
			else
			{
				binding_string += " " + mapping_string;
			}
		}
	}
	
	return binding_string;
}

static void add_control_item(game::context* ctx, const std::string& control_name)
{
	// Get pointer to control
	input::control* control = ctx->controls[control_name];
	
	// Construct texts
	scene::text* name_text = new scene::text();
	scene::text* value_text = new scene::text();
	
	// Add texts to list of menu item texts
	ctx->menu_item_texts.push_back({name_text, value_text});
	
	// Set content of name text
	std::string string_name = "control_" + control_name;
	if (auto it = ctx->strings->find(string_name); it != ctx->strings->end())
		name_text->set_content(it->second);
	else
		name_text->set_content(control_name);
	
	// Set content of value text
	value_text->set_content(get_binding_string(ctx, control));
	
	auto select_callback = [ctx, control, value_text]()
	{
		// Clear binding string from value text
		value_text->set_content((*ctx->strings)["ellipsis"]);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
		
		// Disable controls
		game::menu::clear_controls(ctx);
		
		// Remove gamepad event mappings from control
		ctx->input_event_router->remove_mappings(control, input::mapping_type::gamepad_axis);
		ctx->input_event_router->remove_mappings(control, input::mapping_type::gamepad_button);
		
		// Setup input binding listener
		ctx->input_listener->set_callback
		(
			[ctx, control, value_text](const event_base& event)
			{
				auto id = event.get_event_type_id();
				if (id == gamepad_axis_moved_event::event_type_id)
				{
					// Map gamepad axis event to control
					const gamepad_axis_moved_event& axis_event = static_cast<const gamepad_axis_moved_event&>(event);	
					if (std::abs(axis_event.value) < 0.5f)
						return;
					
					ctx->input_event_router->add_mapping(input::gamepad_axis_mapping(control, nullptr, axis_event.axis, (axis_event.value < 0)));
				}
				else if (id == gamepad_button_pressed_event::event_type_id)
				{
					// Map gamepad button event to control
					const gamepad_button_pressed_event& button_event = static_cast<const gamepad_button_pressed_event&>(event);
					ctx->input_event_router->add_mapping(input::gamepad_button_mapping(control, nullptr, button_event.button));
				}
				else if (id == key_pressed_event::event_type_id)
				{
					// Map key pressed event to control
					const key_pressed_event& key_event = static_cast<const key_pressed_event&>(event);
					
					if (key_event.scancode != input::scancode::escape && key_event.scancode != input::scancode::backspace)
						return;
				}
				else
				{
					return;
				}
				
				// Update menu text
				value_text->set_content(get_binding_string(ctx, control));
				game::menu::align_text(ctx);
				game::menu::update_text_tweens(ctx);
				
				// Disable input listener
				ctx->input_listener->set_enabled(false);
				ctx->input_listener->set_callback(nullptr);
				
				// Schedule re-enabling of menu controls
				timeline* timeline = ctx->timeline;
				float t = timeline->get_position();
				timeline->add_sequence({{t + game::menu::input_delay, std::bind(game::menu::setup_controls, ctx)}});
			}
		);
		ctx->input_listener->set_enabled(true);
	};
	
	// Register menu item callbacks
	ctx->menu_select_callbacks.push_back(select_callback);
	ctx->menu_left_callbacks.push_back(nullptr);
	ctx->menu_right_callbacks.push_back(nullptr);
}

void enter(game::context* ctx)
{
	// Add camera control menu items
	add_control_item(ctx, "move_forward");
	add_control_item(ctx, "move_back");
	add_control_item(ctx, "move_left");
	add_control_item(ctx, "move_right");
	add_control_item(ctx, "move_up");
	add_control_item(ctx, "move_down");
	
	// Add application control menu items
	add_control_item(ctx, "toggle_fullscreen");
	add_control_item(ctx, "screenshot");
	
	// Construct menu item texts
	scene::text* back_text = new scene::text();
	
	// Build list of menu item texts
	ctx->menu_item_texts.push_back({back_text, nullptr});
	
	// Set content of menu item texts
	back_text->set_content((*ctx->strings)["back"]);
	
	// Init menu item index
	game::menu::init_menu_item_index(ctx, "gamepad_config");
	
	game::menu::update_text_color(ctx);
	game::menu::update_text_font(ctx);
	game::menu::align_text(ctx);
	game::menu::update_text_tweens(ctx);
	game::menu::add_text_to_ui(ctx);
	game::menu::setup_animations(ctx);
	
	// Construct menu item callbacks
	auto select_back_callback = [ctx]()
	{
		// Disable controls
		game::menu::clear_controls(ctx);
		
		game::menu::fade_out
		(
			ctx,
			[ctx]()
			{
				application::state next_state;
				next_state.name = "controls_menu";
				next_state.enter = std::bind(game::state::controls_menu::enter, ctx);
				next_state.exit = std::bind(game::state::controls_menu::exit, ctx);
				ctx->app->queue_state(next_state);
			}
		);
	};
	
	// Build list of menu select callbacks
	ctx->menu_select_callbacks.push_back(select_back_callback);
	
	// Build list of menu left callbacks
	ctx->menu_left_callbacks.push_back(nullptr);
	
	// Build list of menu right callbacks
	ctx->menu_right_callbacks.push_back(nullptr);
	
	// Set menu back callback
	ctx->menu_back_callback = select_back_callback;
	
	// Schedule menu control setup
	timeline* timeline = ctx->timeline;
	float t = timeline->get_position();
	timeline->add_sequence({{t + game::menu::input_delay, std::bind(game::menu::setup_controls, ctx)}});
	
	// Fade in menu
	game::menu::fade_in(ctx, nullptr);
}

void exit(game::context* ctx)
{
	// Destruct menu
	game::menu::clear_controls(ctx);
	game::menu::clear_callbacks(ctx);
	game::menu::delete_animations(ctx);
	game::menu::remove_text_from_ui(ctx);
	game::menu::delete_text(ctx);
	
	// Save control profile
	game::save_control_profile(ctx);
}

} // namespace gamepad_config_menu
} // namespace state
} // namespace game
