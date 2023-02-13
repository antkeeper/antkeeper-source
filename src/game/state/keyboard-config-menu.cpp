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
#include "game/controls.hpp"
#include "scene/text.hpp"
#include "debug/log.hpp"
#include "resources/resource-manager.hpp"
#include "game/menu.hpp"
#include "game/controls.hpp"
#include "game/strings.hpp"
#include "utility/hash/fnv1a.hpp"
#include <format>
#include <utility>

using namespace hash::literals;

namespace game {
namespace state {

keyboard_config_menu::keyboard_config_menu(game::context& ctx):
	game::state::base(ctx)
{
	debug::log::trace("Entering keyboard config menu state...");
	
	// Add control menu items
	add_control_item(ctx.movement_controls, ctx.move_forward_control, "control_move_forward"_fnv1a32);
	add_control_item(ctx.movement_controls, ctx.move_back_control, "control_move_back"_fnv1a32);
	add_control_item(ctx.movement_controls, ctx.move_left_control, "control_move_left"_fnv1a32);
	add_control_item(ctx.movement_controls, ctx.move_right_control, "control_move_right"_fnv1a32);
	add_control_item(ctx.movement_controls, ctx.move_up_control, "control_move_up"_fnv1a32);
	add_control_item(ctx.movement_controls, ctx.move_down_control, "control_move_down"_fnv1a32);
	add_control_item(ctx.movement_controls, ctx.pause_control, "control_pause"_fnv1a32);
	
	// Construct menu item texts
	scene::text* back_text = new scene::text();
	
	// Build list of menu item texts
	ctx.menu_item_texts.push_back({back_text, nullptr});
	
	// Set content of menu item texts
	back_text->set_content(get_string(ctx, "back"_fnv1a32));
	
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
		// Disable menu controls
		ctx.function_queue.push(std::bind(game::disable_menu_controls, std::ref(ctx)));
		
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
	
	// Enable menu controls next frame
	ctx.function_queue.push(std::bind(game::enable_menu_controls, std::ref(ctx)));
	
	// Fade in menu
	game::menu::fade_in(ctx, nullptr);
	
	debug::log::trace("Entered keyboard config menu state");
}

keyboard_config_menu::~keyboard_config_menu()
{
	debug::log::trace("Exiting keyboard config menu state...");
	
	// Destruct menu
	game::disable_menu_controls(ctx);
	game::menu::clear_callbacks(ctx);
	game::menu::delete_animations(ctx);
	game::menu::remove_text_from_ui(ctx);
	game::menu::delete_text(ctx);
	
	// Save control profile
	game::save_control_profile(ctx);
	
	debug::log::trace("Exited keyboard config menu state...");
}

std::string keyboard_config_menu::get_mapping_string(const input::control_map& control_map, const input::control& control)
{
	std::string mapping_string;
	
	if (auto key_mappings = control_map.get_key_mappings(control); !key_mappings.empty())
	{
		const auto& key_mapping = key_mappings.front();
		
		// Get name of scancode string from scancode
		std::string scancode_string_name = std::format("scancode_{:02x}", std::to_underlying(key_mapping.scancode));
		
		// Set mapping string to scancode string
		mapping_string = get_string(ctx, hash::fnv1a32(scancode_string_name.data(), scancode_string_name.length()));
	}
	else if (auto mouse_button_mappings = control_map.get_mouse_button_mappings(control); !mouse_button_mappings.empty())
	{
		const auto& mouse_button_mapping = mouse_button_mappings.front();
		switch (mouse_button_mapping.button)
		{
			case input::mouse_button::left:
				mapping_string = get_string(ctx, "mouse_button_left"_fnv1a32);
				break;
			
			case input::mouse_button::middle:
				mapping_string = get_string(ctx, "mouse_button_middle"_fnv1a32);
				break;
			
			case input::mouse_button::right:
				mapping_string = get_string(ctx, "mouse_button_right"_fnv1a32);
				break;
			
			default:
			{
				std::string format_string = get_string(ctx, "mouse_button_n_format"_fnv1a32);
				mapping_string = std::vformat(format_string, std::make_format_args(std::to_underlying(mouse_button_mapping.button)));
				break;
			}
		}
	}
	else if (auto mouse_scroll_mappings = control_map.get_mouse_scroll_mappings(control); !mouse_scroll_mappings.empty())
	{
		const auto& mouse_scroll_mapping = mouse_scroll_mappings.front();
		
		if (mouse_scroll_mapping.axis == input::mouse_scroll_axis::x)
		{
			if (!mouse_scroll_mapping.direction)
			{
				mapping_string = get_string(ctx, "mouse_scroll_left"_fnv1a32);
			}
			else
			{
				mapping_string = get_string(ctx, "mouse_scroll_right"_fnv1a32);
			}
		}
		else
		{
			if (!mouse_scroll_mapping.direction)
			{
				mapping_string = get_string(ctx, "mouse_scroll_up"_fnv1a32);
			}
			else
			{
				mapping_string = get_string(ctx, "mouse_scroll_down"_fnv1a32);
			}
		}
	}
	else
	{
		mapping_string = get_string(ctx, "control_unmapped"_fnv1a32);
	}
	
	return mapping_string;
}

void keyboard_config_menu::add_control_item(input::control_map& control_map, input::control& control, std::uint32_t control_name_hash)
{
	// Construct texts
	scene::text* name_text = new scene::text();
	scene::text* value_text = new scene::text();
	
	// Add texts to list of menu item texts
	ctx.menu_item_texts.push_back({name_text, value_text});
	
	// Set control name and mapping texts
	name_text->set_content(get_string(ctx, control_name_hash));
	value_text->set_content(get_mapping_string(control_map, control));
	
	// Callback invoked when an input has been mapped to the control
	auto input_mapped_callback = [this, &ctx = this->ctx, control_map = &control_map, control = &control, value_text](const auto& event)
	{
		// Remove key mappings, mouse button mappings, and mouse scroll mappings mapped to the control
		control_map->remove_mappings(*control, input::mapping_type::key);
		control_map->remove_mappings(*control, input::mapping_type::mouse_button);
		control_map->remove_mappings(*control, input::mapping_type::mouse_scroll);
		
		//if (event.mapping.scancode != input::scancode::escape && event.mapping.scancode != input::scancode::backspace)
		{
			// Map generated input mapping to the control
			control_map->add_mapping(*control, event.mapping);
		}
		
		// Update control mapping text
		value_text->set_content(this->get_mapping_string(*control_map, *control));
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
		
		// Queue disabling of input mapper re-enabling of menu controls
		ctx.function_queue.push
		(
			[&ctx]()
			{
				ctx.input_mapper.disconnect();
				game::enable_menu_controls(ctx);
			}
		);
	};
	
	// Callback invoked when the control menu item has been selected
	auto select_callback = [this, &ctx = this->ctx, control_map = &control_map, control = &control, value_text, input_mapped_callback]()
	{
		// Set control mapping text to "..."
		value_text->set_content(get_string(ctx, "control_mapping"_fnv1a32));
		game::menu::align_text(ctx);
		game::menu::update_text_tweens(ctx);
		
		// Setup input mapped callbacks
		key_mapped_subscription = ctx.input_mapper.get_key_mapped_channel().subscribe
		(
			input_mapped_callback
		);
		mouse_button_mapped_subscription = ctx.input_mapper.get_mouse_button_mapped_channel().subscribe
		(
			input_mapped_callback
		);
		mouse_scroll_mapped_subscription = ctx.input_mapper.get_mouse_scroll_mapped_channel().subscribe
		(
			input_mapped_callback
		);
		
		// Queue disabling of menu controls and enabling of input mapper
		ctx.function_queue.push
		(
			[&]()
			{
				game::disable_menu_controls(ctx);
				ctx.input_mapper.connect(ctx.input_manager->get_event_queue());
			}
		);
	};
	
	// Register menu item callbacks
	ctx.menu_select_callbacks.push_back(select_callback);
	ctx.menu_left_callbacks.push_back(nullptr);
	ctx.menu_right_callbacks.push_back(nullptr);
}

} // namespace state
} // namespace game
