// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/states/keyboard-config-menu-state.hpp"
#include "game/states/controls-menu-state.hpp"
#include "game/controls.hpp"
#include <engine/scene/text.hpp>
#include <engine/debug/log.hpp>
#include <engine/resources/resource-manager.hpp>
#include "game/menu.hpp"
#include "game/controls.hpp"
#include "game/strings.hpp"
#include <engine/hash/fnv1a.hpp>
#include <format>
#include <utility>

using namespace hash::literals;

keyboard_config_menu_state::keyboard_config_menu_state(::game& ctx):
	game_state(ctx),
	action_remapped(false)
{
	debug::log_trace("Entering keyboard config menu state...");
	
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
	::menu::init_menu_item_index(ctx, "keyboard_config");
	
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
	
	// Enable menu controls next frame
	ctx.function_queue.push(std::bind(::enable_menu_controls, std::ref(ctx)));
	
	// Fade in menu
	::menu::fade_in(ctx, nullptr);
	
	debug::log_trace("Entered keyboard config menu state");
}

keyboard_config_menu_state::~keyboard_config_menu_state()
{
	debug::log_trace("Exiting keyboard config menu state...");
	
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
	
	debug::log_trace("Exited keyboard config menu state...");
}

std::string keyboard_config_menu_state::get_mapping_string(const input::action_map& action_map, const input::action& control)
{
	std::string mapping_string;
	
	if (auto key_mappings = action_map.get_key_mappings(control); !key_mappings.empty())
	{
		const auto& key_mapping = key_mappings.front();
		
		// Get name of scancode string from scancode
		std::string scancode_string_name = std::format("scancode_{:02x}", std::to_underlying(key_mapping.scancode));
		
		// Set mapping string to scancode string
		mapping_string = get_string(ctx, scancode_string_name);
	}
	else if (auto mouse_button_mappings = action_map.get_mouse_button_mappings(control); !mouse_button_mappings.empty())
	{
		const auto& mouse_button_mapping = mouse_button_mappings.front();
		switch (mouse_button_mapping.button)
		{
			case input::mouse_button::left:
				mapping_string = get_string(ctx, "mouse_button_left");
				break;
			
			case input::mouse_button::middle:
				mapping_string = get_string(ctx, "mouse_button_middle");
				break;
			
			case input::mouse_button::right:
				mapping_string = get_string(ctx, "mouse_button_right");
				break;
			
			default:
			{
				const auto format_string = get_string(ctx, "mouse_button_n_format");

				auto button = std::to_underlying(mouse_button_mapping.button);

				mapping_string = std::vformat(format_string, std::make_format_args(button));
				break;
			}
		}
	}
	else if (auto mouse_scroll_mappings = action_map.get_mouse_scroll_mappings(control); !mouse_scroll_mappings.empty())
	{
		const auto& mouse_scroll_mapping = mouse_scroll_mappings.front();
		
		if (mouse_scroll_mapping.axis == input::mouse_scroll_axis::x)
		{
			if (!mouse_scroll_mapping.direction)
			{
				mapping_string = get_string(ctx, "mouse_scroll_left");
			}
			else
			{
				mapping_string = get_string(ctx, "mouse_scroll_right");
			}
		}
		else
		{
			if (!mouse_scroll_mapping.direction)
			{
				mapping_string = get_string(ctx, "mouse_scroll_up");
			}
			else
			{
				mapping_string = get_string(ctx, "mouse_scroll_down");
			}
		}
	}
	else
	{
		mapping_string = get_string(ctx, "control_unmapped");
	}
	
	return mapping_string;
}

void keyboard_config_menu_state::add_control_item(input::action_map& action_map, input::action& control, std::string_view control_name)
{
	// Construct texts
	auto name_text = std::make_unique<scene::text>();
	auto value_text = std::make_unique<scene::text>();
	
	// Add texts to list of menu item texts
	ctx.menu_item_texts.push_back({name_text.get(), value_text.get()});
	
	// Set control name and mapping texts
	name_text->set_content(get_string(ctx, control_name));
	value_text->set_content(get_mapping_string(action_map, control));
	
	// Callback invoked when an input has been mapped to the control
	auto input_mapped_callback = [this, &ctx = this->ctx, action_map = &action_map, control = &control, value_text = value_text.get()](const auto& event)
	{
		this->action_remapped = true;
		
		// Remove key mappings, mouse button mappings, and mouse scroll mappings mapped to the control
		action_map->remove_mappings(*control, input::mapping_type::key);
		action_map->remove_mappings(*control, input::mapping_type::mouse_button);
		action_map->remove_mappings(*control, input::mapping_type::mouse_scroll);
		
		//if (event.mapping.scancode != input::scancode::escape && event.mapping.scancode != input::scancode::backspace)
		{
			// Map generated input mapping to the control
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
