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
#include "resources/resource-manager.hpp"
#include "resources/json.hpp"
#include "input/modifier-key.hpp"

namespace game {

void setup_window_controls(game::context& ctx)
{
	// Map window controls
	ctx.window_actions.add_mapping(ctx.fullscreen_action, input::key_mapping(nullptr, input::scancode::f11, false));
	ctx.window_actions.add_mapping(ctx.fullscreen_action, input::key_mapping(nullptr, input::scancode::enter, false, input::modifier_key::alt));
	ctx.window_actions.add_mapping(ctx.screenshot_action, input::key_mapping(nullptr, input::scancode::f12, false));
	ctx.window_actions.add_mapping(ctx.screenshot_action, input::key_mapping(nullptr, input::scancode::print_screen, false));
	
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
	// Map menu controls
	ctx.menu_actions.add_mapping(ctx.menu_up_action, input::key_mapping(nullptr, input::scancode::up, true));
	ctx.menu_actions.add_mapping(ctx.menu_up_action, input::key_mapping(nullptr, input::scancode::w, true));
	ctx.menu_actions.add_mapping(ctx.menu_up_action, input::key_mapping(nullptr, input::scancode::i, true));
	ctx.menu_actions.add_mapping(ctx.menu_up_action, input::gamepad_axis_mapping(nullptr, input::gamepad_axis::left_stick_y, true));
	ctx.menu_actions.add_mapping(ctx.menu_up_action, input::gamepad_axis_mapping(nullptr, input::gamepad_axis::right_stick_y, true));
	ctx.menu_actions.add_mapping(ctx.menu_up_action, input::gamepad_button_mapping(nullptr, input::gamepad_button::dpad_up));
	
	ctx.menu_actions.add_mapping(ctx.menu_down_action, input::key_mapping(nullptr, input::scancode::down, true));
	ctx.menu_actions.add_mapping(ctx.menu_down_action, input::key_mapping(nullptr, input::scancode::s, true));
	ctx.menu_actions.add_mapping(ctx.menu_down_action, input::key_mapping(nullptr, input::scancode::k, true));
	ctx.menu_actions.add_mapping(ctx.menu_down_action, input::gamepad_axis_mapping(nullptr, input::gamepad_axis::left_stick_y, false));
	ctx.menu_actions.add_mapping(ctx.menu_down_action, input::gamepad_axis_mapping(nullptr, input::gamepad_axis::right_stick_y, false));
	ctx.menu_actions.add_mapping(ctx.menu_down_action, input::gamepad_button_mapping(nullptr, input::gamepad_button::dpad_down));
	
	ctx.menu_actions.add_mapping(ctx.menu_left_action, input::key_mapping(nullptr, input::scancode::left, true));
	ctx.menu_actions.add_mapping(ctx.menu_left_action, input::key_mapping(nullptr, input::scancode::a, true));
	ctx.menu_actions.add_mapping(ctx.menu_left_action, input::key_mapping(nullptr, input::scancode::j, true));
	ctx.menu_actions.add_mapping(ctx.menu_left_action, input::gamepad_axis_mapping(nullptr, input::gamepad_axis::left_stick_x, true));
	ctx.menu_actions.add_mapping(ctx.menu_left_action, input::gamepad_axis_mapping(nullptr, input::gamepad_axis::right_stick_x, true));
	ctx.menu_actions.add_mapping(ctx.menu_left_action, input::gamepad_button_mapping(nullptr, input::gamepad_button::dpad_left));
	ctx.menu_actions.add_mapping(ctx.menu_left_action, input::mouse_scroll_mapping(nullptr, input::mouse_scroll_axis::x, true));
	ctx.menu_actions.add_mapping(ctx.menu_left_action, input::mouse_scroll_mapping(nullptr, input::mouse_scroll_axis::y, true));
	
	ctx.menu_actions.add_mapping(ctx.menu_right_action, input::key_mapping(nullptr, input::scancode::right, true));
	ctx.menu_actions.add_mapping(ctx.menu_right_action, input::key_mapping(nullptr, input::scancode::d, true));
	ctx.menu_actions.add_mapping(ctx.menu_right_action, input::key_mapping(nullptr, input::scancode::l, true));
	ctx.menu_actions.add_mapping(ctx.menu_right_action, input::gamepad_axis_mapping(nullptr, input::gamepad_axis::left_stick_x, false));
	ctx.menu_actions.add_mapping(ctx.menu_right_action, input::gamepad_axis_mapping(nullptr, input::gamepad_axis::right_stick_x, false));
	ctx.menu_actions.add_mapping(ctx.menu_right_action, input::gamepad_button_mapping(nullptr, input::gamepad_button::dpad_right));
	ctx.menu_actions.add_mapping(ctx.menu_right_action, input::mouse_scroll_mapping(nullptr, input::mouse_scroll_axis::x, false));
	ctx.menu_actions.add_mapping(ctx.menu_right_action, input::mouse_scroll_mapping(nullptr, input::mouse_scroll_axis::y, false));
	
	ctx.menu_actions.add_mapping(ctx.menu_select_action, input::key_mapping(nullptr, input::scancode::enter, false));
	ctx.menu_actions.add_mapping(ctx.menu_select_action, input::key_mapping(nullptr, input::scancode::space, false));
	ctx.menu_actions.add_mapping(ctx.menu_select_action, input::key_mapping(nullptr, input::scancode::e, false));
	ctx.menu_actions.add_mapping(ctx.menu_select_action, input::gamepad_button_mapping(nullptr, input::gamepad_button::a));
	
	ctx.menu_actions.add_mapping(ctx.menu_back_action, input::key_mapping(nullptr, input::scancode::escape, false));
	ctx.menu_actions.add_mapping(ctx.menu_back_action, input::key_mapping(nullptr, input::scancode::backspace, false));
	ctx.menu_actions.add_mapping(ctx.menu_back_action, input::key_mapping(nullptr, input::scancode::q, false));
	ctx.menu_actions.add_mapping(ctx.menu_back_action, input::gamepad_button_mapping(nullptr, input::gamepad_button::b));
	ctx.menu_actions.add_mapping(ctx.menu_back_action, input::gamepad_button_mapping(nullptr, input::gamepad_button::back));
	
	ctx.menu_actions.add_mapping(ctx.menu_modifier_action, input::key_mapping(nullptr, input::scancode::left_shift, false));
	ctx.menu_actions.add_mapping(ctx.menu_modifier_action, input::key_mapping(nullptr, input::scancode::right_shift, false));
	
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

std::filesystem::path gamepad_calibration_path(const game::context& ctx, const input::gamepad& gamepad)
{
	return std::filesystem::path("gamepad-" + gamepad.get_uuid().string() + ".json");
}

json default_control_profile()
{
	return json();
}

json default_gamepad_calibration()
{
	const float activation_min = 0.15f;
	const float activation_max = 0.98f;
	const bool deadzone_cross = false;
	const float deadzone_roundness = 1.0f;
	const std::string response_curve = "linear";
	
	json calibration;
	
	calibration["leftx_activation"] = {activation_min, activation_max};
	calibration["lefty_activation"] = {activation_min, activation_max};
	calibration["rightx_activation"] = {activation_min, activation_max};
	calibration["righty_activation"] = {activation_min, activation_max};
	calibration["lefttrigger_activation"] = {activation_min, activation_max};
	calibration["righttrigger_activation"] = {activation_min, activation_max};
	calibration["leftx_response_curve"] = response_curve;
	calibration["lefty_response_curve"] = response_curve;
	calibration["rightx_response_curve"] = response_curve;
	calibration["righty_response_curve"] = response_curve;
	calibration["lefttrigger_response_curve"] = response_curve;
	calibration["righttrigger_response_curve"] = response_curve;
	calibration["left_deadzone_cross"] = deadzone_cross;
	calibration["right_deadzone_cross"] = deadzone_cross;
	calibration["left_deadzone_roundness"] = deadzone_roundness;
	calibration["right_deadzone_roundness"] = deadzone_roundness;
	
	return calibration;
}

json* load_gamepad_calibration(game::context& ctx, const input::gamepad& gamepad)
{
	// Determine path to gamepad calibration file
	std::filesystem::path path = gamepad_calibration_path(ctx, gamepad);
	
	// Load gamepad calibration file
	json* calibration = ctx.resource_manager->load<json>(path.string());
	
	return calibration;
}

bool save_gamepad_calibration(const game::context& ctx, const input::gamepad& gamepad, const json& calibration)
{
	/*
	// Determine absolute path to gamepad calibration file
	std::filesystem::path path = ctx.controls_path / gamepad_calibration_path(ctx, gamepad);
	
	// Open calibration file
	std::ofstream stream;
	stream.open(path);
	if (!stream)
		return false;
	
	// Write calibration to file
	stream << calibration.dump(1, '\t');
	if (stream.bad())
	{
		stream.close();
		return false;
	}
	
	// Close calibration file
	stream.close();
	*/
	
	return true;
}

void apply_control_profile(game::context& ctx, const json& profile)
{
	/*
	// Map gamepad buttons to strings
	const std::unordered_map<std::string, input::gamepad_button> gamepad_button_map =
	{
		{"a", input::gamepad_button::a},
		{"b", input::gamepad_button::b},
		{"x", input::gamepad_button::x},
		{"y", input::gamepad_button::y},
		{"back", input::gamepad_button::back},
		{"guide", input::gamepad_button::guide},
		{"start", input::gamepad_button::start},
		{"leftstick", input::gamepad_button::left_stick},
		{"rightstick", input::gamepad_button::right_stick},
		{"leftshoulder", input::gamepad_button::left_shoulder},
		{"rightshoulder", input::gamepad_button::right_shoulder},
		{"dpup", input::gamepad_button::dpad_up},
		{"dpdown", input::gamepad_button::dpad_down},
		{"dpleft", input::gamepad_button::dpad_left},
		{"dpright", input::gamepad_button::dpad_right}
	};
	
	// Map gamepad axes to strings
	const std::unordered_map<std::string, input::gamepad_axis> gamepad_axis_map =
	{
		{"leftx", input::gamepad_axis::left_x},
		{"lefty", input::gamepad_axis::left_y},
		{"rightx", input::gamepad_axis::right_x},
		{"righty", input::gamepad_axis::right_y},
		{"lefttrigger", input::gamepad_axis::left_trigger},
		{"righttrigger", input::gamepad_axis::right_trigger}
	};
	
	// Remove all existing input mappings
	for (auto control = ctx.controls.begin(); control != ctx.controls.end(); ++control)
	{
		ctx.input_event_router->remove_mappings(control->second);
	}
	
	// Get keyboard and mouse devices
	input::keyboard* keyboard = ctx.input_manager->get_keyboards().front();
	input::mouse* mouse = ctx.input_manager->get_mice().front();
	
	// Find profile gamepad device
	input::gamepad* gamepad = nullptr;
	auto gamepad_element = profile.find("gamepad");
	if (gamepad_element != profile.end())
	{
		// Get gamepad UUID string
		const std::string uuid_string = gamepad_element->get<std::string>();
		
		// Find gamepad with matching UUID
		for (input::gamepad* device: ctx.input_manager->get_gamepads())
		{
			if (device->get_uuid().string() == uuid_string)
			{
				gamepad = device;
				break;
			}
		}
	}
	
	// Find controls element
	auto controls_element = profile.find("controls");
	if (controls_element != profile.end())
	{
		// For each control in the profile
		for (auto control_element = controls_element->cbegin(); control_element != controls_element->cend(); ++control_element)
		{
			// Get the control name
			std::string control_name = control_element.key();
			
			// Find or create control
			input::action* control;
			if (ctx.controls.count(control_name))
			{
				control = ctx.controls[control_name];
			}
			else
			{
				control = new input::action();
				ctx.controls[control_name] = control;
			}
			
			// For each mapping in the control
			for (auto mapping_element = control_element.value().cbegin(); mapping_element != control_element.value().cend(); ++mapping_element)
			{
				if (!mapping_element->contains("device"))
				{
					debug::log::warning("Control \"" + control_name + "\" not mapped to a device");
					continue;
				}
				
				// Get the mapping device
				const std::string device = (*mapping_element)["device"];
				
				if (device == "keyboard")
				{
					// Parse key name
					if (!mapping_element->contains("key"))
					{
						debug::log::warning("Control \"" + control_name + "\" has invalid keyboard mapping");
						continue;
					}
					std::string key = (*mapping_element)["key"].get<std::string>();
					
					// Get scancode from key name
					input::scancode scancode = keyboard->get_scancode_from_name(key.c_str());
					if (scancode == input::scancode::unknown)
					{
						debug::log::warning("Control \"" + control_name + "\" mapped to unknown keyboard key \"" + key + "\"");
						continue;
					}
					
					// Map control to keyboard key
					ctx.input_event_router->add_mapping(input::key_mapping(control, keyboard, scancode));
					
					debug::log::info("Mapped control \"" + control_name + "\" to keyboard key \"" + key + "\"");
				}
				else if (device == "mouse")
				{
					if (mapping_element->contains("button"))
					{
						// Parse mouse button index
						int button = (*mapping_element)["button"].get<int>();
						
						// Map control to mouse button
						ctx.input_event_router->add_mapping(input::mouse_button_mapping(control, mouse, button));
						
						debug::log::info("Mapped control \"" + control_name + "\" to mouse button " + std::to_string(button));
					}
					else if (mapping_element->contains("wheel"))
					{
						// Parse mouse wheel axis
						std::string wheel = (*mapping_element)["wheel"].get<std::string>();
						input::mouse_axis axis;
						if (wheel == "x+")
							axis = input::mouse_axis::positive_x;
						else if (wheel == "x-")
							axis = input::mouse_axis::negative_x;
						else if (wheel == "y+")
							axis = input::mouse_axis::positive_y;
						else if (wheel == "y-")
							axis = input::mouse_axis::negative_y;
						else
						{
							debug::log::warning("Control \"" + control_name + "\" is mapped to invalid mouse wheel axis \"" + wheel + "\"");
							continue;
						}
						
						// Map control to mouse wheel axis
						ctx.input_event_router->add_mapping(input::mouse_wheel_mapping(control, mouse, axis));
						
						debug::log::info("Mapped control \"" + control_name + "\" to mouse wheel axis " + wheel);
					}
					else if (mapping_element->contains("motion"))
					{
						std::string motion = (*mapping_element)["motion"].get<std::string>();
						input::mouse_axis axis;
						if (motion == "x+")
							axis = input::mouse_axis::positive_x;
						else if (motion == "x-")
							axis = input::mouse_axis::negative_x;
						else if (motion == "y+")
							axis = input::mouse_axis::positive_y;
						else if (motion == "y-")
							axis = input::mouse_axis::negative_y;
						else
						{
							debug::log::warning("Control \"" + control_name + "\" is mapped to invalid mouse motion axis \"" + motion + "\"");
							continue;
						}
						
						// Map control to mouse motion axis
						ctx.input_event_router->add_mapping(input::mouse_motion_mapping(control, mouse, axis));
						
						debug::log::info("Mapped control \"" + control_name + "\" to mouse motion axis " + motion);
					}
					else
					{
						debug::log::warning("Control \"" + control_name + "\" has invalid mouse mapping");
						continue;
					}
				}
				else if (device == "gamepad")
				{
					if (mapping_element->contains("button"))
					{
						// Parse gamepad button
						std::string button = (*mapping_element)["button"].get<std::string>();
						
						auto button_it = gamepad_button_map.find(button);
						if (button_it == gamepad_button_map.end())
						{
							debug::log::warning("Control \"" + control_name + "\" is mapped to invalid gamepad button \"" + button + "\"");
							continue;
						}
						
						// Map control to gamepad button
						ctx.input_event_router->add_mapping(input::gamepad_button_mapping(control, gamepad, button_it->second));
						
						debug::log::info("Mapped control \"" + control_name + "\" to gamepad button " + button);
					}
					else if (mapping_element->contains("axis"))
					{
						std::string axis = (*mapping_element)["axis"].get<std::string>();
						
						// Parse gamepad axis name
						const std::string axis_name = axis.substr(0, axis.length() - 1);
						auto axis_it = gamepad_axis_map.find(axis_name);
						if (axis_it == gamepad_axis_map.end())
						{
							debug::log::warning("Control \"" + control_name + "\" is mapped to invalid gamepad axis \"" + axis_name + "\"");
							continue;
						}
						
						// Parse gamepad axis sign
						const char axis_sign = axis.back();
						if (axis_sign != '-' && axis_sign != '+')
						{
							debug::log::warning("Control \"" + control_name + "\" is mapped to gamepad axis with invalid sign \"" + axis_sign + "\"");
							continue;
						}
						bool axis_negative = (axis_sign == '-');
						
						// Map control to gamepad axis
						ctx.input_event_router->add_mapping(input::gamepad_axis_mapping(control, gamepad, axis_it->second, axis_negative));
						
						debug::log::info("Mapped control \"" + control_name + "\" to gamepad axis " + axis);
					}
					else
					{
						debug::log::info("Control \"" + control_name + "\" has invalid gamepad mapping");
						continue;
					}
				}
				else
				{
					debug::log::warning("Control \"" + control_name + "\" bound to unknown device \"" + device + "\"");
				}
			}
		}
	}
	*/
}

void save_control_profile(game::context& ctx)
{
	/*
	std::filesystem::path path;
	if (ctx.config->contains("control_profile"))
		path = ctx.shared_config_path / "controls" / (*ctx.config)["control_profile"].get<std::string>();
	
	debug::log::trace("Saving control profile to \"{}\"...", path.string());
	try
	{
		json control_profile;
		
		// Add controls element
		auto& controls_element = control_profile["controls"];
		controls_element = json::object();
		
		for (auto controls_it = ctx.controls.begin(); controls_it != ctx.controls.end(); ++controls_it)
		{
			const std::string& control_name = controls_it->first;
			input::action* control = controls_it->second;
			
			// Add control element
			auto& control_element = controls_element[control_name];
			control_element = json::array();
			
			// Get control mappings
			auto mappings = ctx.input_event_router->get_mappings(control);
			
			// Skip unmapped controls
			if (!mappings)
				continue;
			
			// Add control mapping elements
			for (input::mapping* mapping: *mappings)
			{
				json mapping_element;
				
				switch (mapping->get_type())
				{
					case input::mapping_type::key:
					{
						const input::key_mapping* key_mapping = static_cast<const input::key_mapping*>(mapping);
						mapping_element["device"] = "keyboard";
						mapping_element["key"] = input::keyboard::get_scancode_name(key_mapping->scancode);
						break;
					}
					
					case input::mapping_type::mouse_wheel:
					{
						const input::mouse_wheel_mapping* wheel_mapping = static_cast<const input::mouse_wheel_mapping*>(mapping);
						
						mapping_element["device"] = "mouse";
						switch (wheel_mapping->axis)
						{
							case input::mouse_axis::negative_x:
								mapping_element["wheel"] = "x-";
								break;
							case input::mouse_axis::positive_x:
								mapping_element["wheel"] = "x+";
								break;
							case input::mouse_axis::negative_y:
								mapping_element["wheel"] = "y-";
								break;
							case input::mouse_axis::positive_y:
								mapping_element["wheel"] = "y+";
								break;
							default:
								break;
						}
						break;
					}
					
					case input::mapping_type::mouse_motion:
					{
						const input::mouse_motion_mapping* motion_mapping = static_cast<const input::mouse_motion_mapping*>(mapping);
						
						mapping_element["device"] = "mouse";
						switch (motion_mapping->axis)
						{
							case input::mouse_axis::negative_x:
								mapping_element["motion"] = "x-";
								break;
							case input::mouse_axis::positive_x:
								mapping_element["motion"] = "x+";
								break;
							case input::mouse_axis::negative_y:
								mapping_element["motion"] = "y-";
								break;
							case input::mouse_axis::positive_y:
								mapping_element["motion"] = "y+";
								break;
							default:
								break;
						}
						break;
					}
					
					case input::mapping_type::mouse_button:
					{
						const input::mouse_button_mapping* button_mapping = static_cast<const input::mouse_button_mapping*>(mapping);
						mapping_element["device"] = "mouse";
						mapping_element["button"] = button_mapping->button;
						break;
					}
					
					case input::mapping_type::gamepad_axis:
					{
						const input::gamepad_axis_mapping* axis_mapping = static_cast<const input::gamepad_axis_mapping*>(mapping);
						mapping_element["device"] = "gamepad";
						switch (axis_mapping->axis)
						{
							case input::gamepad_axis::left_x:
								if (axis_mapping->negative)
									mapping_element["axis"] = "leftx-";
								else
									mapping_element["axis"] = "leftx+";
								break;
							case input::gamepad_axis::left_y:
								if (axis_mapping->negative)
									mapping_element["axis"] = "lefty-";
								else
									mapping_element["axis"] = "lefty+";
								break;
							case input::gamepad_axis::right_x:
								if (axis_mapping->negative)
									mapping_element["axis"] = "rightx-";
								else
									mapping_element["axis"] = "rightx+";
								break;
							case input::gamepad_axis::right_y:
								if (axis_mapping->negative)
									mapping_element["axis"] = "righty-";
								else
									mapping_element["axis"] = "righty+";
								break;
							case input::gamepad_axis::left_trigger:
								mapping_element["axis"] = "lefttrigger+";
								break;
							case input::gamepad_axis::right_trigger:
								mapping_element["axis"] = "righttrigger+";
								break;
							default:
								break;
						}
						break;
					}
					
					case input::mapping_type::gamepad_button:
					{
						const input::gamepad_button_mapping* button_mapping = static_cast<const input::gamepad_button_mapping*>(mapping);
						mapping_element["device"] = "gamepad";
						
						switch (button_mapping->button)
						{
							case input::gamepad_button::a:
								mapping_element["button"] = "a";
								break;
							case input::gamepad_button::b:
								mapping_element["button"] = "b";
								break;
							case input::gamepad_button::x:
								mapping_element["button"] = "x";
								break;
							case input::gamepad_button::y:
								mapping_element["button"] = "y";
								break;
							case input::gamepad_button::back:
								mapping_element["button"] = "back";
								break;
							case input::gamepad_button::guide:
								mapping_element["button"] = "guide";
								break;
							case input::gamepad_button::start:
								mapping_element["button"] = "start";
								break;
							case input::gamepad_button::left_stick:
								mapping_element["button"] = "leftstick";
								break;
							case input::gamepad_button::right_stick:
								mapping_element["button"] = "rightstick";
								break;
							case input::gamepad_button::left_shoulder:
								mapping_element["button"] = "leftshoulder";
								break;
							case input::gamepad_button::right_shoulder:
								mapping_element["button"] = "rightshoulder";
								break;
							case input::gamepad_button::dpad_up:
								mapping_element["button"] = "dpup";
								break;
							case input::gamepad_button::dpad_down:
								mapping_element["button"] = "dpdown";
								break;
							case input::gamepad_button::dpad_left:
								mapping_element["button"] = "dpleft";
								break;
							case input::gamepad_button::dpad_right:
								mapping_element["button"] = "dpright";
								break;
							default:
								break;
						}
						break;
					}
					
					default:
						break;
				}
				
				control_element.push_back(mapping_element);
			}
		}
		
		std::ofstream control_profile_file(path);
		control_profile_file << control_profile;
	}
	catch (...)
	{
		debug::log::pop_task(EXIT_FAILURE);
	}
	debug::log::pop_task(EXIT_SUCCESS);
	*/
}

void apply_gamepad_calibration(input::gamepad& gamepad, const json& calibration)
{
	/*
	// Parse and apply activation thresholds
	if (calibration.contains("leftx_activation"))
	{
		float min = calibration["leftx_activation"][0].get<float>();
		float max = calibration["leftx_activation"][1].get<float>();
		gamepad.set_activation_threshold(input::gamepad_axis::left_x, min, max);
	}
	if (calibration.contains("lefty_activation"))
	{
		float min = calibration["lefty_activation"][0].get<float>();
		float max = calibration["lefty_activation"][1].get<float>();
		gamepad.set_activation_threshold(input::gamepad_axis::left_y, min, max);
	}
	if (calibration.contains("rightx_activation"))
	{
		float min = calibration["rightx_activation"][0].get<float>();
		float max = calibration["rightx_activation"][1].get<float>();
		gamepad.set_activation_threshold(input::gamepad_axis::right_x, min, max);
	}
	if (calibration.contains("righty_activation"))
	{
		float min = calibration["righty_activation"][0].get<float>();
		float max = calibration["righty_activation"][1].get<float>();
		gamepad.set_activation_threshold(input::gamepad_axis::right_y, min, max);
	}
	if (calibration.contains("lefttrigger_activation"))
	{
		float min = calibration["lefttrigger_activation"][0].get<float>();
		float max = calibration["lefttrigger_activation"][1].get<float>();
		gamepad.set_activation_threshold(input::gamepad_axis::left_trigger, min, max);
	}
	if (calibration.contains("righttrigger_activation"))
	{
		float min = calibration["righttrigger_activation"][0].get<float>();
		float max = calibration["righttrigger_activation"][1].get<float>();
		gamepad.set_activation_threshold(input::gamepad_axis::right_trigger, min, max);
	}
	
	// Parse and apply deadzone shapes
	if (calibration.contains("left_deadzone_cross"))
		gamepad.set_left_deadzone_cross(calibration["left_deadzone_cross"].get<bool>());
	if (calibration.contains("right_deadzone_cross"))
		gamepad.set_right_deadzone_cross(calibration["right_deadzone_cross"].get<bool>());
	if (calibration.contains("left_deadzone_roundness"))
		gamepad.set_left_deadzone_roundness(calibration["left_deadzone_roundness"].get<float>());
	if (calibration.contains("right_deadzone_roundness"))
		gamepad.set_right_deadzone_roundness(calibration["right_deadzone_roundness"].get<float>());
	
	auto parse_response_curve = [](const std::string& curve) -> input::gamepad_response_curve
	{
		if (curve == "square")
			return input::gamepad_response_curve::square;
		else if (curve == "cube")
			return input::gamepad_response_curve::cube;
		return input::gamepad_response_curve::linear;
	};
	
	// Parse and apply axis response curves
	if (calibration.contains("leftx_response_curve"))
	{
		auto curve = parse_response_curve(calibration["leftx_response_curve"].get<std::string>());
		gamepad.set_response_curve(input::gamepad_axis::left_x, curve);
	}
	if (calibration.contains("lefty_response_curve"))
	{
		auto curve = parse_response_curve(calibration["lefty_response_curve"].get<std::string>());
		gamepad.set_response_curve(input::gamepad_axis::left_y, curve);
	}
	if (calibration.contains("rightx_response_curve"))
	{
		auto curve = parse_response_curve(calibration["rightx_response_curve"].get<std::string>());
		gamepad.set_response_curve(input::gamepad_axis::right_x, curve);
	}
	if (calibration.contains("righty_response_curve"))
	{
		auto curve = parse_response_curve(calibration["righty_response_curve"].get<std::string>());
		gamepad.set_response_curve(input::gamepad_axis::right_y, curve);
	}
	if (calibration.contains("lefttrigger_response_curve"))
	{
		auto curve = parse_response_curve(calibration["lefttrigger_response_curve"].get<std::string>());
		gamepad.set_response_curve(input::gamepad_axis::left_trigger, curve);
	}
	if (calibration.contains("righttrigger_response_curve"))
	{
		auto curve = parse_response_curve(calibration["righttrigger_response_curve"].get<std::string>());
		gamepad.set_response_curve(input::gamepad_axis::right_trigger, curve);
	}
	*/
}

} // namespace game
