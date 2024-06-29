// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/input/gamepad.hpp>
#include <engine/math/functions.hpp>
#include <algorithm>
#include <type_traits>
#include <cmath>

namespace input {

gamepad::gamepad()
{
	for (int i = 0; i < 6; ++i)
	{
		axis_positions[i] = 0.0f;
		axis_activation_min[i] = 0.15f;
		axis_activation_max[i] = 0.98f;
		axis_response_curves[i] = gamepad_response_curve::linear;
	}
}

void gamepad::set_activation_threshold(gamepad_axis axis, float min, float max)
{
	axis_activation_min[static_cast<std::underlying_type_t<gamepad_axis>>(axis)] = min;
	axis_activation_max[static_cast<std::underlying_type_t<gamepad_axis>>(axis)] = max;
}

void gamepad::set_response_curve(gamepad_axis axis, gamepad_response_curve curve)
{
	axis_response_curves[static_cast<std::underlying_type_t<gamepad_axis>>(axis)] = curve;
}

void gamepad::set_left_deadzone_cross(bool cross)
{
	left_deadzone_cross = cross;
}

void gamepad::set_right_deadzone_cross(bool cross)
{
	right_deadzone_cross = cross;
}

void gamepad::set_left_deadzone_roundness(float roundness)
{
	left_deadzone_roundness = roundness;
}

void gamepad::set_right_deadzone_roundness(float roundness)
{
	right_deadzone_roundness = roundness;
}

void gamepad::press(gamepad_button button)
{
	button_pressed_publisher.publish({this, button});
}

void gamepad::release(gamepad_button button)
{
	button_released_publisher.publish({this, button});
}

void gamepad::move(gamepad_axis axis, float position)
{
	const auto axis_index = static_cast<std::underlying_type_t<gamepad_axis>>(axis);
	
	/// @todo Support arbitrary number of gamepad axes.
	if (axis_index >= 6)
		return;
	
	// Update axis position
	axis_positions[axis_index] = position;
	
	switch (axis)
	{
		case gamepad_axis::left_stick_x:
		case gamepad_axis::left_stick_y:
			if (left_deadzone_cross)
				handle_axial_motion(axis);
			else
				handle_biaxial_motion(gamepad_axis::left_stick_x, gamepad_axis::left_stick_y);
			break;
		
		case gamepad_axis::right_stick_x:
		case gamepad_axis::right_stick_y:
			if (right_deadzone_cross)
				handle_axial_motion(axis);
			else
				handle_biaxial_motion(gamepad_axis::right_stick_x, gamepad_axis::right_stick_y);
			break;
		
		case gamepad_axis::left_trigger:
		case gamepad_axis::right_trigger:
		default:
			handle_axial_motion(axis);
			break;
	}
}

void gamepad::handle_axial_motion(gamepad_axis axis)
{
	const auto axis_index = static_cast<std::underlying_type_t<gamepad_axis>>(axis);
	
	// Get axis parameters
	const float activation_min = axis_activation_min[axis_index];
	const float activation_max = axis_activation_max[axis_index];
	const float axis_position = axis_positions[axis_index];
	
	// Remap axis position
	float remapped_position = 0.0f;
	if (std::abs(axis_position) > activation_min)
	{
		// Remap position according to activation thresholds and clamp to `[0, 1]`.
		float response = math::map_range(std::abs(axis_position), activation_min, activation_max, 0.0f, 1.0f);
		response = std::clamp(response, 0.0f, 1.0f);
		
		// Remap position according to axis response curve
		response = curve_response(axis, response);
		
		// Restore sign of axis motion
		response = (axis_position < 0.0f) ? -response : response;
		
		remapped_position = response;
	}
	
	axis_moved_publisher.publish({this, axis, remapped_position});
}

void gamepad::handle_biaxial_motion(gamepad_axis axis_x, gamepad_axis axis_y)
{
	// Get axis parameters
	const int x_axis_index = static_cast<std::underlying_type_t<gamepad_axis>>(axis_x);
	const int y_axis_index = static_cast<std::underlying_type_t<gamepad_axis>>(axis_y);
	const float x_activation_min = axis_activation_min[x_axis_index];
	const float x_activation_max = axis_activation_max[x_axis_index];
	const float y_activation_min = axis_activation_min[y_axis_index];
	const float y_activation_max = axis_activation_max[y_axis_index];
	const float x_axis_position = axis_positions[x_axis_index];
	const float y_axis_position = axis_positions[y_axis_index];
	const float deadzone_roundness = (axis_x == gamepad_axis::left_stick_x) ? left_deadzone_roundness : right_deadzone_roundness;
	
	const float radius = std::min<float>(x_activation_min, y_activation_min) * deadzone_roundness;
	const float dx = std::max<float>(0.0f, std::abs(x_axis_position) - x_activation_min + radius);
	const float dy = std::max<float>(0.0f, std::abs(y_axis_position) - y_activation_min + radius);
	const float distance = std::sqrt(dx * dx + dy * dy) - radius;
	
	if (distance > 0.0f)
	{
		const float nx = std::abs(x_axis_position) / distance;
		const float ny = std::abs(y_axis_position) / distance;
		const float ndx = (distance - x_activation_min) / (x_activation_max - x_activation_min);
		const float ndy = (distance - y_activation_min) / (y_activation_max - y_activation_min);
		
		float response_x = std::clamp(nx * ndx, 0.0f, 1.0f);
		float response_y = std::clamp(ny * ndy, 0.0f, 1.0f);
		
		response_x = curve_response(axis_x, response_x);
		response_y = curve_response(axis_y, response_y);
		
		// Restore signs of axis motions
		response_x = (x_axis_position < 0.0f) ? -response_x : response_x;
		response_y = (y_axis_position < 0.0f) ? -response_y : response_y;
		
		axis_moved_publisher.publish({this, axis_x, response_x});
		axis_moved_publisher.publish({this, axis_y, response_y});
	}
	else
	{
		axis_moved_publisher.publish({this, axis_x, 0.0f});
		axis_moved_publisher.publish({this, axis_y, 0.0f});
	}
}

float gamepad::curve_response(gamepad_axis axis, float response) const
{
	const auto axis_index = static_cast<std::underlying_type_t<gamepad_axis>>(axis);
	const gamepad_response_curve response_curve = axis_response_curves[axis_index];
	
	switch (response_curve)
	{
		case gamepad_response_curve::linear:
			break;
		
		case gamepad_response_curve::square:
			response = response * response;
			break;
		
		case gamepad_response_curve::cube:
			response = response * response * response;
			break;
	}
	
	return response;
}

} // namespace input
