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

#ifndef ANTKEEPER_INPUT_EVENTS_HPP
#define ANTKEEPER_INPUT_EVENTS_HPP

#include "event/event.hpp"

enum class scancode;
enum class game_controller_axis;
enum class game_controller_button;
class keyboard;
class mouse;
class game_controller;

/**
 * Input event which indicates a keyboard key has been pressed.
 */
class key_pressed_event: public event<key_pressed_event>
{
public:
	virtual event_base* clone() const;

	keyboard* keyboard;
	scancode scancode;
};

/**
 * Input event which indicates a keyboard key has been released.
 */
class key_released_event: public event<key_released_event>
{
public:
	virtual event_base* clone() const;

	keyboard* keyboard;
	scancode scancode;
};

/**
 * Input event which indicates a mouse has been moved.
 */
class mouse_moved_event: public event<mouse_moved_event>
{
public:
	virtual event_base* clone() const;

	mouse* mouse;
	int x;
	int y;
	int dx;
	int dy;
};

/**
 * Input event which indicates a mouse button has been pressed.
 */
class mouse_button_pressed_event: public event<mouse_button_pressed_event>
{
public:
	virtual event_base* clone() const;

	mouse* mouse;
	int button;
	int x;
	int y;
};

/**
 * Input event which indicates a mouse button has been released.
 */
class mouse_button_released_event: public event<mouse_button_released_event>
{
public:
	virtual event_base* clone() const;

	mouse* mouse;
	int button;
	int x;
	int y;
};

/**
 * Input event which indicates a mouse wheel has been scrolled.
 */
class mouse_wheel_scrolled_event: public event<mouse_wheel_scrolled_event>
{
public:
	virtual event_base* clone() const;

	mouse* mouse;
	int x;
	int y;
};

/**
 * Input event which indicates a controller has been connected.
 */
class game_controller_connected_event: public event<game_controller_connected_event>
{
public:
	virtual event_base* clone() const;

	game_controller* game_controller;
	bool reconnected;
};

/**
 * Input event which indicates a controller has been disconnected.
 */
class game_controller_disconnected_event: public event<game_controller_disconnected_event>
{
public:
	virtual event_base* clone() const;

	game_controller* game_controller;
};

/**
 * Input event which indicates a controller button has been pressed.
 */
class game_controller_button_pressed_event: public event<game_controller_button_pressed_event>
{
public:
	virtual event_base* clone() const;

	game_controller* game_controller;
	game_controller_button button;
};

/**
 * Input event which indicates a controller button has been released.
 */
class game_controller_button_released_event: public event<game_controller_button_released_event>
{
public:
	virtual event_base* clone() const;

	game_controller* game_controller;
	game_controller_button button;
};

/**
 * Input event which indicates a controller axis has been moved.
 */
class game_controller_axis_moved_event: public event<game_controller_axis_moved_event>
{
public:
	virtual event_base* clone() const;

	game_controller* game_controller;
	game_controller_axis axis;
	float value;
};

#endif // ANTKEEPER_INPUT_EVENTS_HPP

