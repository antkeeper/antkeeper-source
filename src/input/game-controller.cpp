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
#include <cmath>

game_controller::game_controller():
	connected(true)
{}

void game_controller::press(game_controller_button button)
{
	if (!input_device::event_dispatcher)
	{
		return;
	}

	game_controller_button_pressed_event event;
	event.game_controller = this;
	event.button = button;

	input_device::event_dispatcher->queue(event);
}

void game_controller::release(game_controller_button button)
{
	if (!input_device::event_dispatcher)
	{
		return;
	}

	game_controller_button_released_event event;
	event.game_controller = this;
	event.button = button;

	input_device::event_dispatcher->queue(event);
}

void game_controller::move(game_controller_axis axis, float value)
{
	if (!input_device::event_dispatcher)
	{
		return;
	}

	game_controller_axis_moved_event event;
	event.game_controller = this;
	event.axis = axis;
	event.value = value;

	input_device::event_dispatcher->queue(event);
}

void game_controller::connect(bool reconnected)
{
	connected = true;
	
	game_controller_connected_event event;
	event.game_controller = this;
	event.reconnected = reconnected;
	
	input_device::event_dispatcher->queue(event);
}

void game_controller::disconnect()
{
	connected = false;
	
	game_controller_disconnected_event event;
	event.game_controller = this;
	
	input_device::event_dispatcher->queue(event);
}
