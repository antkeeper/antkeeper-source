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

#include "mouse.hpp"
#include "event/input-events.hpp"
#include "event/event-dispatcher.hpp"

mouse::mouse()
{}

void mouse::press(int button, int x, int y)
{
	if (!input_device::event_dispatcher)
	{
		return;
	}

	mouse_button_pressed_event event;
	event.mouse = this;
	event.button = button;
	event.x = x;
	event.y = y;

	input_device::event_dispatcher->queue(event);
}

void mouse::release(int button, int x, int y)
{
	if (!input_device::event_dispatcher)
	{
		return;
	}

	mouse_button_released_event event;
	event.mouse = this;
	event.button = button;
	event.x = x;
	event.y = y;

	input_device::event_dispatcher->queue(event);
}

void mouse::move(int x, int y, int dx, int dy)
{
	previous_position = current_position;
	current_position = {x, y};

	if (!input_device::event_dispatcher)
	{
		return;
	}

	mouse_moved_event event;
	event.mouse = this;
	event.x = x;
	event.y = y;
	event.dx = dx;
	event.dy = dy;

	input_device::event_dispatcher->queue(event);
}

void mouse::scroll(int x, int y)
{
	if (!input_device::event_dispatcher)
	{
		return;
	}

	mouse_wheel_scrolled_event event;
	event.mouse = this;
	event.x = x;
	event.y = y;

	input_device::event_dispatcher->queue(event);
}

