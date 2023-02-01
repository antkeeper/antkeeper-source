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

#include "game/state/keyboard-config-menu.hpp"
#include "game/state/controls-menu.hpp"
#include "application.hpp"
#include "scene/text.hpp"
#include "debug/log.hpp"
#include "resources/resource-manager.hpp"
#include "game/menu.hpp"
#include "game/controls.hpp"

namespace game {
namespace state {

keyboard_config_menu::keyboard_config_menu(game::context& ctx):
	game::state::base(ctx)
{
	debug::log::push_task("Entering keyboard config menu state");
	
	// Add camera control menu items
	add_control_item("move_forward");
	add_control_item("move_back");
	add_control_item("move_left");
	add_control_item("move_right");
	add_control_item("move_up");
	add_control_item("move_down");
	
	// Add application control menu items
	add_control_item("toggle_fullscreen");
	add_control_item("screenshot");
	
	// Construct menu item texts
	scene::text* back_text = new scene::text();
	
	// Build list of menu item texts
	ctx.menu_item_texts.push_back({back_text, nullptr});
	
	// Set content of menu item texts
	back_text->set_content((*ctx.strings)["back"]);
	
	// Init menu item index
	game::menu::init_menu_item_index(ctx, "keyboard_config");
	
	game::menu::update_text_color(ctx);
	game::menu::update_text_font(ctx);
	game::menu::align_text(ctx);
	game::menu::update_text_tweens(ctx);
	game::menu::add_text_to_ui(ctx);
	game::menu::setup_animations(ctx);
	
	// Construct menu item callbacks
	auto select_back_callback = [&ctx]()
	{
		// Disable controls
		game::menu::clear_controls(ctx);
		
		game::menu::fade_out
		(
			ctx,
			[&ctx]()
			{
				// Queue change to controls menu state
				ctx.function_queue.push
				(
					[&ctx]()
					{
						ctx.state_machine.pop();
						ctx.state_machine.emplace(new game::state::controls_menu(ctx));
					}
				);
			}
		);
	};
	
	// Build list of menu select callbacks
	ctx.menu_select_callbacks.push_back(select_back_callback);
	
	// Build list of menu left callbacks
	ctx.menu_left_callbacks.push_back(nullptr);
	
	// Build list of menu right callbacks
	ctx.menu_right_callbacks.push_back(nullptr);
	
	// Set menu back callback
	ctx.menu_back_callback = select_back_callback;
	
	// Queue menu control setup
	ctx.function_queue.push(std::bind(game::menu::setup_controls, std::ref(ctx)));
	
	// Fade in menu
	game::menu::fade_in(ctx, nullptr);
	
	debug::log::pop_task(EXIT_SUCCESS);
}

keyboard_config_menu::~keyboard_config_menu()
{
	debug::log::push_task("Exiting keyboard config menu state");
	
	// Destruct menu
	game::menu::clear_controls(ctx);
	game::menu::clear_callbacks(ctx);
	game::menu::delete_animations(ctx);
	game::menu::remove_text_from_ui(ctx);
	game::menu::delete_text(ctx);
	
	// Save control profile
	game::save_control_profile(ctx);
	
	debug::log::pop_task(EXIT_SUCCESS);
}

std::string keyboard_config_menu::get_binding_string(input::control* control)
{
	std::string binding_string;
	/*
	auto mappings = ctx.input_event_router->get_mappings(control);
	for (input::mapping* mapping: *mappings)
	{
		std::string mapping_string;
		
		switch (mapping->get_type())
		{
			
			case input::mapping_type::key:
			{
				const input::key_mapping* key_mapping = static_cast<const input::key_mapping*>(mapping);
				const char* scancode_name = input::keyboard::get_scancode_name(key_mapping->scancode);
				mapping_string = scancode_name;
				break;
			}
			
			case input::mapping_type::mouse_wheel:
			{
				const input::mouse_wheel_mapping* wheel_mapping = static_cast<const input::mouse_wheel_mapping*>(mapping);
				
				switch (wheel_mapping->axis)
				{
					case input::mouse_wheel_axis::negative_x:
						mapping_string = (*ctx.strings)["mouse_wheel_left"];
						break;
					
					case input::mouse_wheel_axis::positive_x:
						mapping_string = (*ctx.strings)["mouse_wheel_right"];
						break;
					
					case input::mouse_wheel_axis::negative_y:
						mapping_string = (*ctx.strings)["mouse_wheel_down"];
						break;
					
					case input::mouse_wheel_axis::positive_y:
						mapping_string = (*ctx.strings)["mouse_wheel_up"];
						break;
					
					default:
						break;
				}
				break;
			}
			
			case input::mapping_type::mouse_button:
			{
				const input::mouse_button_mapping* button_mapping = static_cast<const input::mouse_button_mapping*>(mapping);
				
				if (button_mapping->button == 1)
				{
					mapping_string = (*ctx.strings)["mouse_button_left"];
				}
				else if (button_mapping->button == 2)
				{
					mapping_string = (*ctx.strings)["mouse_button_middle"];
				}
				else if (button_mapping->button == 3)
				{
					mapping_string = (*ctx.strings)["mouse_button_right"];
				}
				else
				{
					const std::string& format = (*ctx.strings)["mouse_button_n"];
					char buffer[64];
					std::snprintf(buffer, 64, format.c_str(), button_mapping->button);
					mapping_string = buffer;
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
	*/
	return binding_string;
}

void keyboard_config_menu::add_control_item(const std::string& control_name)
{
	// Get pointer to control
	//input::control* control = ctx.controls[control_name];
	
	// Construct texts
	scene::text* name_text = new scene::text();
	scene::text* value_text = new scene::text();
	
	// Add texts to list of menu item texts
	ctx.menu_item_texts.push_back({name_text, value_text});
	
	// Set content of name text
	std::string string_name = "control_" + control_name;
	if (auto it = ctx.strings->find(string_name); it != ctx.strings->end())
		name_text->set_content(it->second);
	else
		name_text->set_content(control_name);
	
	// Set content of value text
	//value_text->set_content(get_binding_string( control));
	
	auto select_callback = [this, &ctx = this->ctx, value_text]()
	{
		// Clear binding string from value text
		value_text->set_content((*ctx.strings)["ellipsis"]);
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
		
		// Disable controls
		game::menu::clear_controls(ctx);
		
		/*
		// Remove keyboard and mouse event mappings from control
		ctx.input_event_router->remove_mappings(control, input::mapping_type::key);
		ctx.input_event_router->remove_mappings(control, input::mapping_type::mouse_motion);
		ctx.input_event_router->remove_mappings(control, input::mapping_type::mouse_wheel);
		ctx.input_event_router->remove_mappings(control, input::mapping_type::mouse_button);
		
		// Setup input binding listener
		ctx.input_listener->set_callback
		(
			[this, &ctx, control, value_text](const event_base& event)
			{
				auto id = event.get_event_type_id();
				if (id == key_pressed_event::event_type_id)
				{
					// Map key pressed event to control
					const key_pressed_event& key_event = static_cast<const key_pressed_event&>(event);
					
					if (key_event.scancode != input::scancode::escape && key_event.scancode != input::scancode::backspace)
						ctx.input_event_router->add_mapping(input::key_mapping(control, key_event.keyboard, key_event.scancode));
				}
				else if (id == mouse_wheel_scrolled_event::event_type_id)
				{
					// Map mouse wheel scrolled event to control
					const mouse_wheel_scrolled_event& wheel_event = static_cast<const mouse_wheel_scrolled_event&>(event);
					input::mouse_wheel_axis axis;
					
					if (wheel_event.x < 0)
						axis = input::mouse_wheel_axis::negative_x;
					else if (wheel_event.x > 0)
						axis = input::mouse_wheel_axis::positive_x;
					else if (wheel_event.y < 0)
						axis = input::mouse_wheel_axis::negative_y;
					else if (wheel_event.y > 0)
						axis = input::mouse_wheel_axis::positive_y;
					else
						return;
					
					ctx.input_event_router->add_mapping(input::mouse_wheel_mapping(control, wheel_event.mouse, axis));
				}
				else if (id == mouse_button_pressed_event::event_type_id)
				{
					// Map mouse button pressed event to control
					const mouse_button_pressed_event& button_event = static_cast<const mouse_button_pressed_event&>(event);
					ctx.input_event_router->add_mapping(input::mouse_button_mapping(control, button_event.mouse, button_event.button));
				}
				else
				{
					return;
				}
				
				// Update menu text
				value_text->set_content(this->get_binding_string(control));
				game::menu::align_text(ctx);
				game::menu::update_text_tweens(ctx);
				
				// Disable input listener
				ctx.input_listener->set_enabled(false);
				ctx.input_listener->set_callback(nullptr);
				
				// Queue menu control setup
				ctx.function_queue.push(std::bind(game::menu::setup_controls, std::ref(ctx)));
			}
		);
		ctx.input_listener->set_enabled(true);
		*/
	};
	
	// Register menu item callbacks
	ctx.menu_select_callbacks.push_back(select_callback);
	ctx.menu_left_callbacks.push_back(nullptr);
	ctx.menu_right_callbacks.push_back(nullptr);
}

} // namespace state
} // namespace game
