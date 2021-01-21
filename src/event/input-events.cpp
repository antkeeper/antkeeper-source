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

#include "input-events.hpp"

event_base* key_pressed_event::clone() const
{
	key_pressed_event* event = new key_pressed_event();
	event->keyboard = keyboard;
	event->scancode = scancode;
	return event;
}

event_base* key_released_event::clone() const
{
	key_released_event* event = new key_released_event();
	event->keyboard = keyboard;
	event->scancode = scancode;
	return event;
}

event_base* mouse_moved_event::clone() const
{
	mouse_moved_event* event = new mouse_moved_event();
	event->mouse = mouse;
	event->x = x;
	event->y = y;
	event->dx = dx;
	event->dy = dy;
	return event;
}

event_base* mouse_button_pressed_event::clone() const
{
	mouse_button_pressed_event* event = new mouse_button_pressed_event();
	event->mouse = mouse;
	event->button = button;
	event->x = x;
	event->y = y;
	return event;
}

event_base* mouse_button_released_event::clone() const
{
	mouse_button_released_event* event = new mouse_button_released_event();
	event->mouse = mouse;
	event->button = button;
	event->x = x;
	event->y = y;
	return event;
}

event_base* mouse_wheel_scrolled_event::clone() const
{
	mouse_wheel_scrolled_event* event = new mouse_wheel_scrolled_event();
	event->mouse = mouse;
	event->x = x;
	event->y = y;
	return event;
}

event_base* game_controller_connected_event::clone() const
{
	game_controller_connected_event* event = new game_controller_connected_event();
	event->controller = controller;
	event->reconnected = reconnected;
	return event;
}

event_base* game_controller_disconnected_event::clone() const
{
	game_controller_disconnected_event* event = new game_controller_disconnected_event();
	event->controller = controller;
	return event;
}

event_base* game_controller_button_pressed_event::clone() const
{
	game_controller_button_pressed_event* event = new game_controller_button_pressed_event();
	event->controller = controller;
	event->button = button;
	return event;
}

event_base* game_controller_button_released_event::clone() const
{
	game_controller_button_released_event* event = new game_controller_button_released_event();
	event->controller = controller;
	event->button = button;
	return event;
}

event_base* game_controller_axis_moved_event::clone() const
{
	game_controller_axis_moved_event* event = new game_controller_axis_moved_event();
	event->controller = controller;
	event->axis = axis;
	event->value = value;
	return event;
}

