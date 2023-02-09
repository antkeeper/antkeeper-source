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

#ifndef ANTKEEPER_INPUT_GAMEPAD_EVENTS_HPP
#define ANTKEEPER_INPUT_GAMEPAD_EVENTS_HPP

#include "input/gamepad-axis.hpp"
#include "input/gamepad-button.hpp"

namespace input {

class gamepad;

/**
 * Event generated when a gamepad button has been pressed.
 */
struct gamepad_button_pressed_event
{
	/// Gamepad that generated the event.
	gamepad* gamepad;
	
	/// Gamepad button being pressed.
	gamepad_button button;
};

/**
 * Event generated when a gamepad button has been released.
 */
struct gamepad_button_released_event
{
	/// Gamepad that generated the event.
	gamepad* gamepad;
	
	/// Gamepad button being released.
	gamepad_button button;
};

/**
 * Event generated when a gamepad axis has been moved.
 */
struct gamepad_axis_moved_event
{
	/// Gamepad that generated the event.
	gamepad* gamepad;
	
	/// Gamepad axis being moved.
	gamepad_axis axis;
	
	/// Position of the gamepad axis, on `[-1, 1]`.
	float position;
};

} // namespace input

#endif // ANTKEEPER_INPUT_GAMEPAD_EVENTS_HPP
