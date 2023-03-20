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

#include "game/states/gamepad-config-menu-state.hpp"
#include "game/states/controls-menu-state.hpp"
#include "game/controls.hpp"
#include "game/game.hpp"
#include <engine/scene/text.hpp>
#include <engine/debug/log.hpp>
#include <engine/resources/resource-manager.hpp>
#include "game/menu.hpp"
#include "game/controls.hpp"
#include "game/strings.hpp"
#include <engine/utility/hash/fnv1a.hpp>

using namespace hash::literals;


gamepad_config_menu_state::gamepad_config_menu_state(::game& ctx):
	game_state(ctx),
	action_remapped(false)
{
	debug::log::trace("Entering gamepad config menu state...");
	
	// Add control menu items
	add_control_item(ctx.movement_action_map, ctx.move_forward_action, "control_move_forward");
	add_control_item(ctx.movement_action_map, ctx.move_back_action, "control_move_back");
	add_control_item(ctx.movement_action_map, ctx.move_left_action, "control_move_left");
	add_control_item(ctx.movement_action_map, ctx.move_right_action, "control_move_right");
	add_control_item(ctx.movement_action_map, ctx.move_up_action, "control_move_up");
	add_control_item(ctx.movement_action_map, ctx.move_down_action, "control_move_down");
	add_control_item(ctx.movement_action_map, ctx.pause_action, "control_pause");
	
	// Construct menu item texts
	back_text = std::make_unique<scene::text>();
	
	// Build list of menu item texts
	ctx.menu_item_texts.push_back({back_text.get(), nullptr});
	
	// Set content of menu item texts
	back_text->set_content(get_string(ctx, "back"));
	
	// Init menu item index
	::menu::init_menu_item_index(ctx, "gamepad_config");
	
	::menu::update_text_color(ctx);
	::menu::update_text_font(ctx);
	::menu::align_text(ctx);
	::menu::add_text_to_ui(ctx);
	::menu::setup_animations(ctx);
	
	// Construct menu item callbacks
	auto select_back_callback = [&ctx]()
	{
		// Disable menu controls
		ctx.function_queue.push(std::bind(::disable_menu_controls, std::ref(ctx)));
		
		::menu::fade_out
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
						ctx.state_machine.emplace(std::make_unique<controls_menu_state>(ctx));
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
	ctx.function_queue.push(std::bind(::enable_menu_controls, std::ref(ctx)));
	
	// Fade in menu
	::menu::fade_in(ctx, nullptr);
	
	debug::log::trace("Entered gamepad config menu state");
}

gamepad_config_menu_state::~gamepad_config_menu_state()
{
	debug::log::trace("Exiting gamepad config menu state...");
	
	// Destruct menu
	::disable_menu_controls(ctx);
	::menu::clear_callbacks(ctx);
	::menu::delete_animations(ctx);
	::menu::remove_text_from_ui(ctx);
	::menu::delete_text(ctx);
	
	if (action_remapped)
	{
		// Update control profile
		::update_control_profile(ctx, *ctx.control_profile);
		
		// Save control profile
		ctx.resource_manager->set_write_path(ctx.controls_path);
		ctx.resource_manager->save(*ctx.control_profile, ctx.control_profile_filename);
	}
	
	debug::log::trace("Exited gamepad config menu state");
}

std::string gamepad_config_menu_state::get_mapping_string(const input::action_map& action_map, const input::action& control)
{
	std::string mapping_string;
	
	if (auto gamepad_axis_mappings = action_map.get_gamepad_axis_mappings(control); !gamepad_axis_mappings.empty())
	{
		const auto& gamepad_axis_mapping = gamepad_axis_mappings.front();
		
		switch (gamepad_axis_mapping.axis)
		{
			case input::gamepad_axis::left_stick_x:
				if (gamepad_axis_mapping.direction)
				{
					mapping_string = get_string(ctx, "gamepad_left_stick_left");
				}
				else
				{
					mapping_string = get_string(ctx, "gamepad_left_stick_right");
				}
				break;
			
			case input::gamepad_axis::left_stick_y:
				if (gamepad_axis_mapping.direction)
				{
					mapping_string = get_string(ctx, "gamepad_left_stick_up");
				}
				else
				{
					mapping_string = get_string(ctx, "gamepad_left_stick_down");
				}
				break;
			
			case input::gamepad_axis::right_stick_x:
				if (gamepad_axis_mapping.direction)
				{
					mapping_string = get_string(ctx, "gamepad_right_stick_left");
				}
				else
				{
					mapping_string = get_string(ctx, "gamepad_right_stick_right");
				}
				break;
			
			case input::gamepad_axis::right_stick_y:
				if (gamepad_axis_mapping.direction)
				{
					mapping_string = get_string(ctx, "gamepad_right_stick_up");
				}
				else
				{
					mapping_string = get_string(ctx, "gamepad_right_stick_down");
				}
				break;
			
			case input::gamepad_axis::left_trigger:
				mapping_string = get_string(ctx, "gamepad_left_trigger");
				break;
			
			case input::gamepad_axis::right_trigger:
				mapping_string = get_string(ctx, "gamepad_right_trigger");
				break;
			
			default:
			{
				const char sign = (gamepad_axis_mapping.direction) ? '-' : '+';
				const std::string format_string = get_string(ctx, "gamepad_axis_n_format");
				mapping_string = std::vformat(format_string, std::make_format_args(std::to_underlying(gamepad_axis_mapping.axis), sign));
				break;
			}
		}
	}
	else if (auto gamepad_button_mappings = action_map.get_gamepad_button_mappings(control); !gamepad_button_mappings.empty())
	{
		const auto& gamepad_button_mapping = gamepad_button_mappings.front();
		switch (gamepad_button_mapping.button)
		{
			case input::gamepad_button::a:
				mapping_string = get_string(ctx, "gamepad_button_a");
				break;
			
			case input::gamepad_button::b:
				mapping_string = get_string(ctx, "gamepad_button_b");
				break;
			
			case input::gamepad_button::x:
				mapping_string = get_string(ctx, "gamepad_button_x");
				break;
			
			case input::gamepad_button::y:
				mapping_string = get_string(ctx, "gamepad_button_y");
				break;
			
			case input::gamepad_button::back:
				mapping_string = get_string(ctx, "gamepad_button_back");
				break;
			
			case input::gamepad_button::guide:
				mapping_string = get_string(ctx, "gamepad_button_guide");
				break;
			
			case input::gamepad_button::start:
				mapping_string = get_string(ctx, "gamepad_button_start");
				break;
			
			case input::gamepad_button::left_stick:
				mapping_string = get_string(ctx, "gamepad_button_left_stick");
				break;
			
			case input::gamepad_button::right_stick:
				mapping_string = get_string(ctx, "gamepad_button_right_stick");
				break;
			
			case input::gamepad_button::left_shoulder:
				mapping_string = get_string(ctx, "gamepad_button_left_shoulder");
				break;
			
			case input::gamepad_button::right_shoulder:
				mapping_string = get_string(ctx, "gamepad_button_right_shoulder");
				break;
			
			case input::gamepad_button::dpad_up:
				mapping_string = get_string(ctx, "gamepad_button_dpad_up");
				break;
			
			case input::gamepad_button::dpad_down:
				mapping_string = get_string(ctx, "gamepad_button_dpad_down");
				break;
			
			case input::gamepad_button::dpad_left:
				mapping_string = get_string(ctx, "gamepad_button_dpad_left");
				break;
			
			case input::gamepad_button::dpad_right:
				mapping_string = get_string(ctx, "gamepad_button_dpad_right");
				break;
			
			default:
			{
				const std::string format_string = get_string(ctx, "gamepad_button_n_format");
				mapping_string = std::vformat(format_string, std::make_format_args(std::to_underlying(gamepad_button_mapping.button)));
				break;
			}
		}
	}
	else
	{
		mapping_string = get_string(ctx, "control_unmapped");
	}
	
	return mapping_string;
}

void gamepad_config_menu_state::add_control_item(input::action_map& action_map, input::action& control, hash::fnv1a32_t control_name_hash)
{
	// Construct texts
	auto name_text = std::make_unique<scene::text>();
	auto value_text = std::make_unique<scene::text>();
	
	// Add texts to list of menu item texts
	ctx.menu_item_texts.push_back({name_text.get(), value_text.get()});
	
	// Set control name and mapping texts
	name_text->set_content(get_string(ctx, control_name_hash));
	value_text->set_content(get_mapping_string(action_map, control));
	
	// Callback invoked when an input has been mapped to the control
	auto input_mapped_callback = [this, &ctx = this->ctx, action_map = &action_map, control = &control, value_text = value_text.get()](const auto& event)
	{
		if (event.mapping.get_mapping_type() != input::mapping_type::key)
		{
			this->action_remapped = true;
			
			// Remove gamepad axis mappings and gamepad button mappings mapped to the control
			action_map->remove_mappings(*control, input::mapping_type::gamepad_axis);
			action_map->remove_mappings(*control, input::mapping_type::gamepad_button);
			
			action_map->add_mapping(*control, event.mapping);
		}
		
		// Update control mapping text
		value_text->set_content(this->get_mapping_string(*action_map, *control));
		::menu::align_text(ctx);
		
		// Queue disabling of input mapper re-enabling of menu controls
		ctx.function_queue.push
		(
			[&ctx]()
			{
				ctx.input_mapper.disconnect();
				::enable_menu_controls(ctx);
			}
		);
	};
	
	// Callback invoked when the control menu item has been selected
	auto select_callback = [this, &ctx = this->ctx, action_map = &action_map, control = &control, value_text = value_text.get(), input_mapped_callback]()
	{
		// Set control mapping text to "..."
		value_text->set_content(get_string(ctx, "control_mapping"));
		::menu::align_text(ctx);
		
		// Setup input mapped callbacks
		gamepad_axis_mapped_subscription = ctx.input_mapper.get_gamepad_axis_mapped_channel().subscribe
		(
			input_mapped_callback
		);
		gamepad_button_mapped_subscription = ctx.input_mapper.get_gamepad_button_mapped_channel().subscribe
		(
			input_mapped_callback
		);
		key_mapped_subscription = ctx.input_mapper.get_key_mapped_channel().subscribe
		(
			input_mapped_callback
		);
		
		// Queue disabling of menu controls and enabling of input mapper
		ctx.function_queue.push
		(
			[&]()
			{
				::disable_menu_controls(ctx);
				ctx.input_mapper.connect(ctx.input_manager->get_event_dispatcher());
			}
		);
	};
	
	control_item_texts.emplace_back(std::move(name_text));
	control_item_texts.emplace_back(std::move(value_text));
	
	// Register menu item callbacks
	ctx.menu_select_callbacks.push_back(select_callback);
	ctx.menu_left_callbacks.push_back(nullptr);
	ctx.menu_right_callbacks.push_back(nullptr);
}
