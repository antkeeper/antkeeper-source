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

#include "listener.hpp"
#include "event/event-dispatcher.hpp"

namespace input {

listener::listener():
	event_dispatcher(nullptr),
	callback(nullptr),
	enabled(false)
{}

listener::~listener()
{
	set_event_dispatcher(nullptr);
}

void listener::set_event_dispatcher(::event_dispatcher* event_dispatcher)
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

void listener::set_callback(std::function<void(const event_base&)> callback)
{
	this->callback = callback;
}

void listener::set_enabled(bool enabled)
{
	this->enabled = enabled;
}

void listener::handle_event(const key_pressed_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	callback(event);
}

void listener::handle_event(const mouse_moved_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	callback(event);
}

void listener::handle_event(const mouse_button_pressed_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	callback(event);
}

void listener::handle_event(const mouse_wheel_scrolled_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	callback(event);
}

void listener::handle_event(const gamepad_button_pressed_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	callback(event);
}

void listener::handle_event(const gamepad_axis_moved_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	callback(event);
}

} // namespace input
