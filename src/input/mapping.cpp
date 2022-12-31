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

#include "mapping.hpp"

namespace input {

mapping::mapping(input::control* control):
	control(control)
{}

key_mapping::key_mapping(const key_mapping& mapping)
{
	*this = mapping;
}

key_mapping::key_mapping(input::control* control, input::keyboard* keyboard, input::scancode scancode):
	mapping(control),
	keyboard(keyboard),
	scancode(scancode)
{}

key_mapping& key_mapping::operator=(const key_mapping& mapping)
{
	control = mapping.control;
	keyboard = mapping.keyboard;
	scancode = mapping.scancode;
	return *this;
}

mouse_motion_mapping::mouse_motion_mapping(const mouse_motion_mapping& mapping)
{
	*this = mapping;
}

mouse_motion_mapping::mouse_motion_mapping(input::control* control, input::mouse* mouse, mouse_motion_axis axis):
	mapping(control),
	mouse(mouse),
	axis(axis)
{}

mouse_motion_mapping& mouse_motion_mapping::operator=(const mouse_motion_mapping& mapping)
{
	control = mapping.control;
	mouse = mapping.mouse;
	axis = mapping.axis;
	return *this;
}

mouse_wheel_mapping::mouse_wheel_mapping(const mouse_wheel_mapping& mapping)
{
	*this = mapping;
}

mouse_wheel_mapping::mouse_wheel_mapping(input::control* control, input::mouse* mouse, mouse_wheel_axis axis):
	mapping(control),
	mouse(mouse),
	axis(axis)
{}

mouse_wheel_mapping& mouse_wheel_mapping::operator=(const mouse_wheel_mapping& mapping)
{
	control = mapping.control;
	mouse = mapping.mouse;
	axis = mapping.axis;
	return *this;
}

mouse_button_mapping::mouse_button_mapping(const mouse_button_mapping& mapping)
{
	*this = mapping;
}

mouse_button_mapping::mouse_button_mapping(input::control* control, input::mouse* mouse, int button):
	mapping(control),
	mouse(mouse),
	button(button)
{}

mouse_button_mapping& mouse_button_mapping::operator=(const mouse_button_mapping& mapping)
{
	control = mapping.control;
	mouse = mapping.mouse;
	button = mapping.button;
	return *this;
}

gamepad_axis_mapping::gamepad_axis_mapping(const gamepad_axis_mapping& mapping)
{
	*this = mapping;
}

gamepad_axis_mapping::gamepad_axis_mapping(input::control* control, gamepad* controller, gamepad_axis axis, bool negative):
	mapping(control),
	controller(controller),
	axis(axis),
	negative(negative)
{}

gamepad_axis_mapping& gamepad_axis_mapping::operator=(const gamepad_axis_mapping& mapping)
{
	control = mapping.control;
	controller = mapping.controller;
	axis = mapping.axis;
	negative = mapping.negative;
	return *this;
}

gamepad_button_mapping::gamepad_button_mapping(const gamepad_button_mapping& mapping)
{
	*this = mapping;
}

gamepad_button_mapping::gamepad_button_mapping(input::control* control, gamepad* controller, gamepad_button button):
	mapping(control),
	controller(controller),
	button(button)
{}

gamepad_button_mapping& gamepad_button_mapping::operator=(const gamepad_button_mapping& mapping)
{
	control = mapping.control;
	controller = mapping.controller;
	button = mapping.button;
	return *this;
}

} // namespace input
