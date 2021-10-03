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
#include <fstream>

namespace game {

std::string gamepad_calibration_path(const game::context* ctx, const input::gamepad* gamepad)
{
	return "gamepad-" + gamepad->get_guid() + ".json";
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
