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

#include "input/mapping.hpp"

namespace input {

gamepad_axis_mapping::gamepad_axis_mapping(input::gamepad* gamepad, gamepad_axis axis, bool direction):
	gamepad(gamepad),
	axis(axis),
	direction(direction)
{}

gamepad_button_mapping::gamepad_button_mapping(input::gamepad* gamepad, gamepad_button button):
	gamepad(gamepad),
	button(button)
{}

key_mapping::key_mapping(input::keyboard* keyboard, input::scancode scancode, bool repeat):
	keyboard(keyboard),
	scancode(scancode),
	repeat(repeat)
{}

mouse_button_mapping::mouse_button_mapping(input::mouse* mouse, mouse_button button):
	mouse(mouse),
	button(button)
{}

mouse_motion_mapping::mouse_motion_mapping(input::mouse* mouse, mouse_motion_axis axis, bool direction):
	mouse(mouse),
	axis(axis),
	direction(direction)
{}

mouse_scroll_mapping::mouse_scroll_mapping(input::mouse* mouse, mouse_scroll_axis axis, bool direction):
	mouse(mouse),
	axis(axis),
	direction(direction)
{}

} // namespace input
