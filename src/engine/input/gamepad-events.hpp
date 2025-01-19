// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_GAMEPAD_EVENTS_HPP
#define ANTKEEPER_INPUT_GAMEPAD_EVENTS_HPP

#include <engine/input/gamepad-axis.hpp>
#include <engine/input/gamepad-button.hpp>

namespace input {

class gamepad;

/// Event generated when a gamepad button has been pressed.
struct gamepad_button_pressed_event
{
	/// Gamepad that generated the event.
	gamepad* gamepad{nullptr};
	
	/// Gamepad button being pressed.
	gamepad_button button{0};
};

/// Event generated when a gamepad button has been released.
struct gamepad_button_released_event
{
	/// Gamepad that generated the event.
	gamepad* gamepad{nullptr};
	
	/// Gamepad button being released.
	gamepad_button button{0};
};

/// Event generated when a gamepad axis has been moved.
struct gamepad_axis_moved_event
{
	/// Gamepad that generated the event.
	gamepad* gamepad{nullptr};
	
	/// Gamepad axis being moved.
	gamepad_axis axis{0};
	
	/// Position of the gamepad axis, on `[-1, 1]`.
	float position{0.0f};
};

} // namespace input

#endif // ANTKEEPER_INPUT_GAMEPAD_EVENTS_HPP
