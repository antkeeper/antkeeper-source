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

#include "game-controller.hpp"
#include "event/input-events.hpp"
#include "event/event-dispatcher.hpp"
#include "math/map.hpp"
#include <algorithm>
#include <cmath>

namespace input {

game_controller::game_controller():
	connected(true),
	left_deadzone_cross(true),
	right_deadzone_cross(true),
	left_deadzone_roundness(0.0f),
	right_deadzone_roundness(0.0f)
{
	for (int i = 0; i < 6; ++i)
	{
		axis_values[i] = 0.0f;
		axis_activation_min[i] = 0.0f;
		axis_activation_max[i] = 1.0f;
		axis_response_curves[i] = game_controller_response_curve::linear;
	}
}

void game_controller::set_activation_threshold(game_controller_axis axis, float min, float max)
{
	axis_activation_min[static_cast<int>(axis)] = min;
	axis_activation_max[static_cast<int>(axis)] = max;
}

void game_controller::set_response_curve(game_controller_axis axis, game_controller_response_curve curve)
{
	axis_response_curves[static_cast<int>(axis)] = curve;
}

void game_controller::set_left_deadzone_cross(bool cross)
{
	left_deadzone_cross = cross;
}

void game_controller::set_right_deadzone_cross(bool cross)
{
	right_deadzone_cross = cross;
}

void game_controller::set_left_deadzone_roundness(float roundness)
{
	left_deadzone_roundness = roundness;
}

void game_controller::set_right_deadzone_roundness(float roundness)
{
	right_deadzone_roundness = roundness;
}

void game_controller::press(game_controller_button button)
{
	if (!device::event_dispatcher)
	{
		return;
	}

	game_controller_button_pressed_event event;
	event.controller = this;
	event.button = button;

	device::event_dispatcher->queue(event);
}

void game_controller::release(game_controller_button button)
{
	if (!device::event_dispatcher)
	{
		return;
	}

	game_controller_button_released_event event;
	event.controller = this;
	event.button = button;

	device::event_dispatcher->queue(event);
}

void game_controller::move(game_controller_axis axis, float value)
{
	// Update axis value
	axis_values[static_cast<int>(axis)] = value;
	
	if (!device::event_dispatcher)
	{
		return;
	}
	
	switch (axis)
	{
		case game_controller_axis::left_x:
		case game_controller_axis::left_y:
			if (left_deadzone_cross)
				handle_axial_motion(axis);
			else
				handle_biaxial_motion(game_controller_axis::left_x, game_controller_axis::left_y);
			break;
			
		case game_controller_axis::right_x:
		case game_controller_axis::right_y:
			if (right_deadzone_cross)
				handle_axial_motion(axis);
			else
				handle_biaxial_motion(game_controller_axis::right_x, game_controller_axis::right_y);
			break;
		
		default:
			handle_axial_motion(axis);
			break;
	}
}

void game_controller::handle_axial_motion(game_controller_axis axis)
{
	// Get axis parameters
	const int axis_index = static_cast<int>(axis);
	const float activation_min = axis_activation_min[axis_index];
	const float activation_max = axis_activation_max[axis_index];
	const float axis_value = axis_values[axis_index];
	const game_controller_response_curve response_curve = axis_response_curves[axis_index];
	
	// Build event
	game_controller_axis_moved_event event;
	event.controller = this;
	event.axis = axis;
	
	if (std::abs(axis_value) > activation_min)
	{
		// Remap response value according to activation thresholds and clamp to `[0, 1]`.
		float response = math::map(std::abs(axis_value), activation_min, activation_max, 0.0f, 1.0f);
		response = std::clamp(response, 0.0f, 1.0f);
		
		// Remap response value according to axis response curve
		response = curve_response(axis, response);
		
		// Restore sign of axis motion
		response = (axis_value < 0.0f) ? -response : response;
		
		event.value = response;
	}
	else
	{
		event.value = 0.0f;
	}
	
	// Dispatch event
	device::event_dispatcher->queue(event);
}

void game_controller::handle_biaxial_motion(game_controller_axis axis_x, game_controller_axis axis_y)
{
	// Get axis parameters
	const int x_axis_index = static_cast<int>(axis_x);
	const int y_axis_index = static_cast<int>(axis_y);
	const float x_activation_min = axis_activation_min[x_axis_index];
	const float x_activation_max = axis_activation_max[x_axis_index];
	const float y_activation_min = axis_activation_min[y_axis_index];
	const float y_activation_max = axis_activation_max[y_axis_index];
	const float x_axis_value = axis_values[x_axis_index];
	const float y_axis_value = axis_values[y_axis_index];
	const game_controller_response_curve x_response_curve = axis_response_curves[x_axis_index];
	const game_controller_response_curve y_response_curve = axis_response_curves[y_axis_index];
	const float deadzone_roundness = (axis_x == game_controller_axis::left_x) ? left_deadzone_roundness : right_deadzone_roundness;
	
	const float radius = std::min<float>(x_activation_min, y_activation_min) * deadzone_roundness;
	const float dx = std::max<float>(0.0f, std::abs(x_axis_value) - x_activation_min + radius);
	const float dy = std::max<float>(0.0f, std::abs(y_axis_value) - y_activation_min + radius);
	const float distance = std::sqrt(dx * dx + dy * dy) - radius;
	
	// Build event
	game_controller_axis_moved_event event;
	event.controller = this;
	
	if (distance > 0.0f)
	{
		const float nx = std::abs(x_axis_value) / distance;
		const float ny = std::abs(y_axis_value) / distance;
		const float ndx = (distance - x_activation_min) / (x_activation_max - x_activation_min);
		const float ndy = (distance - y_activation_min) / (y_activation_max - y_activation_min);
		
		float response_x = std::clamp(nx * ndx, 0.0f, 1.0f);
		float response_y = std::clamp(ny * ndy, 0.0f, 1.0f);
		
		response_x = curve_response(axis_x, response_x);
		response_y = curve_response(axis_y, response_y);
		
		// Restore signs of axis motions
		response_x = (x_axis_value < 0.0f) ? -response_x : response_x;
		response_y = (y_axis_value < 0.0f) ? -response_y : response_y;
		
		event.value = response_x;
		event.axis = axis_x;
		device::event_dispatcher->queue(event);
		event.value = response_y;
		event.axis = axis_y;
		device::event_dispatcher->queue(event);
	}
	else
	{
		event.value = 0.0f;
		event.axis = axis_x;
		device::event_dispatcher->queue(event);
		event.axis = axis_y;
		device::event_dispatcher->queue(event);
	}
}

float game_controller::curve_response(game_controller_axis axis, float response) const
{
	const game_controller_response_curve response_curve = axis_response_curves[static_cast<int>(axis)];
	
	switch (response_curve)
	{
		case game_controller_response_curve::linear:
			break;
		
		case game_controller_response_curve::square:
			response = response * response;
			break;
		
		case game_controller_response_curve::cube:
			response = response * response * response;
			break;
	}
	
	return response;
}

void game_controller::connect(bool reconnected)
{
	connected = true;
	
	game_controller_connected_event event;
	event.controller = this;
	event.reconnected = reconnected;
	
	device::event_dispatcher->queue(event);
}

void game_controller::disconnect()
{
	connected = false;
	
	game_controller_disconnected_event event;
	event.controller = this;
	
	device::event_dispatcher->queue(event);
}

} // namespace input
