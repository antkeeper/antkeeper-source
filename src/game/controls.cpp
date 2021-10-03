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

#include "controls.hpp"
#include "resources/resource-manager.hpp"
#include "application.hpp"
#include <fstream>

namespace game {

std::string gamepad_calibration_path(const game::context* ctx, const input::gamepad* gamepad)
{
	return "gamepad-" + gamepad->get_guid() + ".json";
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

json* load_gamepad_calibration(game::context* ctx, input::gamepad* gamepad)
{
	// Determine path to gamepad calibration file
	std::string filepath = gamepad_calibration_path(ctx, gamepad);
	
	// Load gamepad calibration file
	json* calibration = ctx->resource_manager->load<json>(filepath);
	
	return calibration;
}

bool save_gamepad_calibration(const game::context* ctx, const input::gamepad* gamepad, const json& calibration)
{
	// Determine path to gamepad calibration file
	std::string filepath = ctx->controls_path + gamepad_calibration_path(ctx, gamepad);
	
	// Open calibration file
	std::ofstream stream;
	stream.open(filepath);
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
	
	return true;
}

void apply_control_profile(game::context* ctx, const json& profile)
{
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
	for (auto control = ctx->controls.begin(); control != ctx->controls.end(); ++control)
	{
		ctx->input_event_router->remove_mappings(control->second);
	}
	
	// Get keyboard and mouse devices
	input::keyboard* keyboard = ctx->app->get_keyboard();
	input::mouse* mouse = ctx->app->get_mouse();
	
	// Find profile gamepad device
	input::gamepad* gamepad = nullptr;
	auto gamepad_element = profile.find("gamepad");
	if (gamepad_element != profile.end())
	{
		// Get gamepad GUID
		const std::string gamepad_guid = gamepad_element->get<std::string>();
		
		// Find gamepad with matching GUID
		for (input::gamepad* device: ctx->app->get_gamepads())
		{
			if (device->get_guid() == gamepad_guid)
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
			input::control* control;
			if (ctx->controls.count(control_name))
			{
				control = ctx->controls[control_name];
			}
			else
			{
				control = new input::control();
				ctx->controls[control_name] = control;
			}
			
			// For each mapping in the control
			for (auto mapping_element = control_element.value().cbegin(); mapping_element != control_element.value().cend(); ++mapping_element)
			{
				if (!mapping_element->contains("device"))
				{
					ctx->logger->warning("Control \"" + control_name + "\" not mapped to a device");
					continue;
				}
				
				// Get the mapping device
				const std::string device = (*mapping_element)["device"];
				
				if (device == "keyboard")
				{
					// Parse key name
					if (!mapping_element->contains("key"))
					{
						ctx->logger->warning("Control \"" + control_name + "\" has invalid keyboard mapping");
						continue;
					}
					std::string key = (*mapping_element)["key"].get<std::string>();
					
					// Get scancode from key name
					input::scancode scancode = keyboard->get_scancode_from_name(key.c_str());
					if (scancode == input::scancode::unknown)
					{
						ctx->logger->warning("Control \"" + control_name + "\" mapped to unknown keyboard key \"" + key + "\"");
						continue;
					}
					
					// Map control to keyboard key
					ctx->input_event_router->add_mapping(input::key_mapping(control, keyboard, scancode));
					
					ctx->logger->log("Mapped control \"" + control_name + "\" to keyboard key \"" + key + "\"");
				}
				else if (device == "mouse")
				{
					if (mapping_element->contains("button"))
					{
						// Parse mouse button index
						int button = (*mapping_element)["button"].get<int>();
						
						// Map control to mouse button
						ctx->input_event_router->add_mapping(input::mouse_button_mapping(control, mouse, button));
						
						ctx->logger->log("Mapped control \"" + control_name + "\" to mouse button " + std::to_string(button));
					}
					else if (mapping_element->contains("wheel"))
					{
						// Parse mouse wheel axis
						std::string wheel = (*mapping_element)["wheel"].get<std::string>();
						input::mouse_wheel_axis axis;
						if (wheel == "x+")
							axis = input::mouse_wheel_axis::positive_x;
						else if (wheel == "x-")
							axis = input::mouse_wheel_axis::negative_x;
						else if (wheel == "y+")
							axis = input::mouse_wheel_axis::positive_y;
						else if (wheel == "y-")
							axis = input::mouse_wheel_axis::negative_y;
						else
						{
							ctx->logger->warning("Control \"" + control_name + "\" is mapped to invalid mouse wheel axis \"" + wheel + "\"");
							continue;
						}
						
						// Map control to mouse wheel axis
						ctx->input_event_router->add_mapping(input::mouse_wheel_mapping(control, mouse, axis));
						
						ctx->logger->log("Mapped control \"" + control_name + "\" to mouse wheel axis " + wheel);
					}
					else if (mapping_element->contains("motion"))
					{
						std::string motion = (*mapping_element)["motion"].get<std::string>();
						input::mouse_motion_axis axis;
						if (motion == "x+")
							axis = input::mouse_motion_axis::positive_x;
						else if (motion == "x-")
							axis = input::mouse_motion_axis::negative_x;
						else if (motion == "y+")
							axis = input::mouse_motion_axis::positive_y;
						else if (motion == "y-")
							axis = input::mouse_motion_axis::negative_y;
						else
						{
							ctx->logger->warning("Control \"" + control_name + "\" is mapped to invalid mouse motion axis \"" + motion + "\"");
							continue;
						}
						
						// Map control to mouse motion axis
						ctx->input_event_router->add_mapping(input::mouse_motion_mapping(control, mouse, axis));
						
						ctx->logger->log("Mapped control \"" + control_name + "\" to mouse motion axis " + motion);
					}
					else
					{
						ctx->logger->warning("Control \"" + control_name + "\" has invalid mouse mapping");
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
							ctx->logger->warning("Control \"" + control_name + "\" is mapped to invalid gamepad button \"" + button + "\"");
							continue;
						}
						
						// Map control to gamepad button
						ctx->input_event_router->add_mapping(input::gamepad_button_mapping(control, gamepad, button_it->second));
						
						ctx->logger->log("Mapped control \"" + control_name + "\" to gamepad button " + button);
					}
					else if (mapping_element->contains("axis"))
					{
						std::string axis = (*mapping_element)["axis"].get<std::string>();
						
						// Parse gamepad axis name
						const std::string axis_name = axis.substr(0, axis.length() - 1);
						auto axis_it = gamepad_axis_map.find(axis_name);
						if (axis_it == gamepad_axis_map.end())
						{
							ctx->logger->warning("Control \"" + control_name + "\" is mapped to invalid gamepad axis \"" + axis_name + "\"");
							continue;
						}
						
						// Parse gamepad axis sign
						const char axis_sign = axis.back();
						if (axis_sign != '-' && axis_sign != '+')
						{
							ctx->logger->warning("Control \"" + control_name + "\" is mapped to gamepad axis with invalid sign \"" + axis_sign + "\"");
							continue;
						}
						bool axis_negative = (axis_sign == '-');
						
						// Map control to gamepad axis
						ctx->input_event_router->add_mapping(input::gamepad_axis_mapping(control, gamepad, axis_it->second, axis_negative));
						
						ctx->logger->log("Mapped control \"" + control_name + "\" to gamepad axis " + axis);
					}
					else
					{
						ctx->logger->log("Control \"" + control_name + "\" has invalid gamepad mapping");
						continue;
					}
				}
				else
				{
					ctx->logger->warning("Control \"" + control_name + "\" bound to unknown device \"" + device + "\"");
				}
			}
		}
	}
}

void apply_gamepad_calibration(input::gamepad* gamepad, const json& calibration)
{
	// Parse and apply activation thresholds
	if (calibration.contains("leftx_activation"))
	{
		float min = calibration["leftx_activation"][0].get<float>();
		float max = calibration["leftx_activation"][1].get<float>();
		gamepad->set_activation_threshold(input::gamepad_axis::left_x, min, max);
	}
	if (calibration.contains("lefty_activation"))
	{
		float min = calibration["lefty_activation"][0].get<float>();
		float max = calibration["lefty_activation"][1].get<float>();
		gamepad->set_activation_threshold(input::gamepad_axis::left_y, min, max);
	}
	if (calibration.contains("rightx_activation"))
	{
		float min = calibration["rightx_activation"][0].get<float>();
		float max = calibration["rightx_activation"][1].get<float>();
		gamepad->set_activation_threshold(input::gamepad_axis::right_x, min, max);
	}
	if (calibration.contains("righty_activation"))
	{
		float min = calibration["righty_activation"][0].get<float>();
		float max = calibration["righty_activation"][1].get<float>();
		gamepad->set_activation_threshold(input::gamepad_axis::right_y, min, max);
	}
	if (calibration.contains("lefttrigger_activation"))
	{
		float min = calibration["lefttrigger_activation"][0].get<float>();
		float max = calibration["lefttrigger_activation"][1].get<float>();
		gamepad->set_activation_threshold(input::gamepad_axis::left_trigger, min, max);
	}
	if (calibration.contains("righttrigger_activation"))
	{
		float min = calibration["righttrigger_activation"][0].get<float>();
		float max = calibration["righttrigger_activation"][1].get<float>();
		gamepad->set_activation_threshold(input::gamepad_axis::right_trigger, min, max);
	}
	
	// Parse and apply deadzone shapes
	if (calibration.contains("left_deadzone_cross"))
		gamepad->set_left_deadzone_cross(calibration["left_deadzone_cross"].get<bool>());
	if (calibration.contains("right_deadzone_cross"))
		gamepad->set_right_deadzone_cross(calibration["right_deadzone_cross"].get<bool>());
	if (calibration.contains("left_deadzone_roundness"))
		gamepad->set_left_deadzone_roundness(calibration["left_deadzone_roundness"].get<float>());
	if (calibration.contains("right_deadzone_roundness"))
		gamepad->set_right_deadzone_roundness(calibration["right_deadzone_roundness"].get<float>());
	
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
		gamepad->set_response_curve(input::gamepad_axis::left_x, curve);
	}
	if (calibration.contains("lefty_response_curve"))
	{
		auto curve = parse_response_curve(calibration["lefty_response_curve"].get<std::string>());
		gamepad->set_response_curve(input::gamepad_axis::left_y, curve);
	}
	if (calibration.contains("rightx_response_curve"))
	{
		auto curve = parse_response_curve(calibration["rightx_response_curve"].get<std::string>());
		gamepad->set_response_curve(input::gamepad_axis::right_x, curve);
	}
	if (calibration.contains("righty_response_curve"))
	{
		auto curve = parse_response_curve(calibration["righty_response_curve"].get<std::string>());
		gamepad->set_response_curve(input::gamepad_axis::right_y, curve);
	}
	if (calibration.contains("lefttrigger_response_curve"))
	{
		auto curve = parse_response_curve(calibration["lefttrigger_response_curve"].get<std::string>());
		gamepad->set_response_curve(input::gamepad_axis::left_trigger, curve);
	}
	if (calibration.contains("righttrigger_response_curve"))
	{
		auto curve = parse_response_curve(calibration["righttrigger_response_curve"].get<std::string>());
		gamepad->set_response_curve(input::gamepad_axis::right_trigger, curve);
	}
}

} // namespace game
