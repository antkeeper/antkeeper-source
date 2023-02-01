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

#include "input/mapper.hpp"
#include <cmath>

namespace input {

void mapper::connect(::event::queue& queue)
{
	subscriptions.emplace_back(queue.subscribe<event::gamepad_axis_moved>(std::bind_front(&mapper::handle_gamepad_axis_moved, this)));
	subscriptions.emplace_back(queue.subscribe<event::gamepad_button_pressed>(std::bind_front(&mapper::handle_gamepad_button_pressed, this)));
	subscriptions.emplace_back(queue.subscribe<event::key_pressed>(std::bind_front(&mapper::handle_key_pressed, this)));
	subscriptions.emplace_back(queue.subscribe<event::mouse_button_pressed>(std::bind_front(&mapper::handle_mouse_button_pressed, this)));
	subscriptions.emplace_back(queue.subscribe<event::mouse_moved>(std::bind_front(&mapper::handle_mouse_moved, this)));
	subscriptions.emplace_back(queue.subscribe<event::mouse_scrolled>(std::bind_front(&mapper::handle_mouse_scrolled, this)));
}

void mapper::disconnect()
{
	subscriptions.clear();
}

void mapper::handle_gamepad_axis_moved(const event::gamepad_axis_moved& event)
{
	input_mapped_publisher.publish({std::shared_ptr<mapping>(new gamepad_axis_mapping(event.gamepad, event.axis, std::signbit(event.position)))});
}

void mapper::handle_gamepad_button_pressed(const event::gamepad_button_pressed& event)
{
	input_mapped_publisher.publish({std::shared_ptr<mapping>(new gamepad_button_mapping(event.gamepad, event.button))});
}

void mapper::handle_key_pressed(const event::key_pressed& event)
{
	input_mapped_publisher.publish({std::shared_ptr<mapping>(new key_mapping(event.keyboard, event.scancode))});
}

void mapper::handle_mouse_button_pressed(const event::mouse_button_pressed& event)
{
	input_mapped_publisher.publish({std::shared_ptr<mapping>(new mouse_button_mapping(event.mouse, event.button))});
}

void mapper::handle_mouse_moved(const event::mouse_moved& event)
{
	if (event.difference.x())
	{
		input_mapped_publisher.publish({std::shared_ptr<mapping>(new mouse_motion_mapping(event.mouse, mouse_motion_axis::x, std::signbit(static_cast<float>(event.difference.x()))))});
	}
	
	if (event.difference.y())
	{
		input_mapped_publisher.publish({std::shared_ptr<mapping>(new mouse_motion_mapping(event.mouse, mouse_motion_axis::y, std::signbit(static_cast<float>(event.difference.y()))))});
	}
}

void mapper::handle_mouse_scrolled(const event::mouse_scrolled& event)
{
	if (event.velocity.x())
	{
		input_mapped_publisher.publish({std::shared_ptr<mapping>(new mouse_scroll_mapping(event.mouse, mouse_scroll_axis::x, std::signbit(event.velocity.x())))});
	}
	
	if (event.velocity.y())
	{
		input_mapped_publisher.publish({std::shared_ptr<mapping>(new mouse_scroll_mapping(event.mouse, mouse_scroll_axis::y, std::signbit(event.velocity.y())))});
	}
}

} // namespace input
