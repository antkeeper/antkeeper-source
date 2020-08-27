/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "input-listener.hpp"
#include "event/event-dispatcher.hpp"

input_listener::input_listener():
	event_dispatcher(nullptr),
	callback(nullptr),
	enabled(false)
{}

input_listener::~input_listener()
{
	set_event_dispatcher(nullptr);
}

void input_listener::set_event_dispatcher(::event_dispatcher* event_dispatcher)
{
	if (this->event_dispatcher)
	{
		this->event_dispatcher->unsubscribe<key_pressed_event>(this);
		this->event_dispatcher->unsubscribe<mouse_moved_event>(this);
		this->event_dispatcher->unsubscribe<mouse_wheel_scrolled_event>(this);
		this->event_dispatcher->unsubscribe<mouse_button_pressed_event>(this);
		this->event_dispatcher->unsubscribe<game_controller_axis_moved_event>(this);
		this->event_dispatcher->unsubscribe<game_controller_button_pressed_event>(this);
	}

	this->event_dispatcher = event_dispatcher;

	if (event_dispatcher)
	{
		event_dispatcher->subscribe<key_pressed_event>(this);
		event_dispatcher->subscribe<mouse_moved_event>(this);
		event_dispatcher->subscribe<mouse_wheel_scrolled_event>(this);
		event_dispatcher->subscribe<mouse_button_pressed_event>(this);
		event_dispatcher->subscribe<game_controller_axis_moved_event>(this);
		event_dispatcher->subscribe<game_controller_button_pressed_event>(this);
	}
}

void input_listener::set_callback(std::function<void(const event_base&)> callback)
{
	this->callback = callback;
}

void input_listener::set_enabled(bool enabled)
{
	this->enabled = enabled;
}

void input_listener::handle_event(const key_pressed_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	callback(event);
}

void input_listener::handle_event(const mouse_moved_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	callback(event);
}

void input_listener::handle_event(const mouse_button_pressed_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	callback(event);
}

void input_listener::handle_event(const mouse_wheel_scrolled_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	callback(event);
}

void input_listener::handle_event(const game_controller_button_pressed_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	callback(event);
}

void input_listener::handle_event(const game_controller_axis_moved_event& event)
{
	if (!is_enabled() || !callback)
	{
		return;
	}

	callback(event);
}
