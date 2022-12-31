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

#include "mapper.hpp"
#include "mouse.hpp"
#include "event/event-dispatcher.hpp"

namespace input {

mapper::mapper():
	event_dispatcher(nullptr),
	control(nullptr),
	callback(nullptr),
	enabled(false)
{}

mapper::~mapper()
{
	set_event_dispatcher(nullptr);
}

void mapper::set_event_dispatcher(::event_dispatcher* event_dispatcher)
{
	if (this->event_dispatcher)
	{
		this->event_dispatcher->unsubscribe<key_pressed_event>(this);
		this->event_dispatcher->unsubscribe<mouse_moved_event>(this);
		this->event_dispatcher->unsubscribe<mouse_wheel_scrolled_event>(this);
		this->event_dispatcher->unsubscribe<mouse_button_pressed_event>(this);
		this->event_dispatcher->unsubscribe<gamepad_axis_moved_event>(this);
		this->event_dispatcher->unsubscribe<gamepad_button_pressed_event>(this);
	}

	this->event_dispatcher = event_dispatcher;

	if (event_dispatcher)
	{
		event_dispatcher->subscribe<key_pressed_event>(this);
		event_dispatcher->subscribe<mouse_moved_event>(this);
		event_dispatcher->subscribe<mouse_wheel_scrolled_event>(this);
		event_dispatcher->subscribe<mouse_button_pressed_event>(this);
		event_dispatcher->subscribe<gamepad_axis_moved_event>(this);
		event_dispatcher->subscribe<gamepad_button_pressed_event>(this);
	}
}

void mapper::set_control(input::control* control)
{
	this->control = control;
}

void mapper::set_callback(std::function<void(const mapping&)> callback)
{
	this->callback = callback;
}

void mapper::set_enabled(bool enabled)
{
	this->enabled = enabled;
}

void mapper::handle_event(const key_pressed_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	callback(key_mapping(control, event.keyboard, event.scancode));
}

void mapper::handle_event(const mouse_moved_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	if (event.dx != 0)
	{
		mouse_motion_axis axis = (event.dx < 0) ? mouse_motion_axis::negative_x : mouse_motion_axis::positive_x;
		callback(mouse_motion_mapping(control, event.mouse, axis));
	}

	if (event.dy != 0)
	{
		mouse_motion_axis axis = (event.dy < 0) ? mouse_motion_axis::negative_y : mouse_motion_axis::positive_y;
		callback(mouse_motion_mapping(control, event.mouse, axis));
	}
}

void mapper::handle_event(const mouse_button_pressed_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	callback(mouse_button_mapping(control, event.mouse, event.button));
}

void mapper::handle_event(const mouse_wheel_scrolled_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	if (event.x != 0)
	{
		mouse_wheel_axis axis = (event.x < 0) ? mouse_wheel_axis::negative_x : mouse_wheel_axis::positive_x;
		callback(mouse_wheel_mapping(control, event.mouse, axis));
	}

	if (event.y != 0)
	{
		mouse_wheel_axis axis = (event.y < 0) ? mouse_wheel_axis::negative_y : mouse_wheel_axis::positive_y;
		callback(mouse_wheel_mapping(control, event.mouse, axis));
	}
}

void mapper::handle_event(const gamepad_button_pressed_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	callback(gamepad_button_mapping(control, event.controller, event.button));
}

void mapper::handle_event(const gamepad_axis_moved_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	callback(gamepad_axis_mapping(control, event.controller, event.axis, (event.value < 0.0f)));
}

} // namespace input
