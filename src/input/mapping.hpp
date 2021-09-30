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

#ifndef ANTKEEPER_INPUT_MAPPING_HPP
#define ANTKEEPER_INPUT_MAPPING_HPP

namespace input {

enum class mouse_motion_axis;
enum class mouse_wheel_axis;
enum class scancode;
enum class gamepad_axis;
enum class gamepad_button;
class control;
class keyboard;
class mouse;
class gamepad;

/**
 * Enumerates the supported types of control mappings.
 */
enum class mapping_type
{
	key,
	mouse_motion,
	mouse_wheel,
	mouse_button,
	gamepad_axis,
	gamepad_button
};

/**
 * Abstract base class for input mappings.
 */
class mapping
{
public:
	mapping() = default;
	mapping(input::control* control);
	virtual ~mapping() = default;

	/// Returns this control mapping's type.
	virtual mapping_type get_type() const = 0;

	control* control;
};

/**
 * A mapping between a control and a keyboard key.
 */
class key_mapping: public mapping
{
public:
	key_mapping() = default;
	key_mapping(const key_mapping& mapping);
	key_mapping(input::control* control, input::keyboard* keyboard, scancode scancode);
	virtual ~key_mapping() = default;
	key_mapping& operator=(const key_mapping& mapping);
	virtual mapping_type get_type() const;

	input::keyboard* keyboard;
	scancode scancode;
};

inline mapping_type key_mapping::get_type() const
{
	return mapping_type::key;
}

/**
 * A mapping between a control and a mouse motion axis.
 */
class mouse_motion_mapping: public mapping
{
public:
	mouse_motion_mapping() = default;
	mouse_motion_mapping(const mouse_motion_mapping& mapping);
	mouse_motion_mapping(input::control* control, input::mouse* mouse, mouse_motion_axis axis);
	virtual ~mouse_motion_mapping() = default;
	mouse_motion_mapping& operator=(const mouse_motion_mapping& mapping);
	virtual mapping_type get_type() const;

	input::mouse* mouse;
	mouse_motion_axis axis;
};

inline mapping_type mouse_motion_mapping::get_type() const
{
	return mapping_type::mouse_motion;
}

/**
 * A mapping between a control and a mouse wheel axis.
 */
class mouse_wheel_mapping: public mapping
{
public:
	mouse_wheel_mapping() = default;
	mouse_wheel_mapping(const mouse_wheel_mapping& mapping);
	mouse_wheel_mapping(input::control* control, input::mouse* mouse, mouse_wheel_axis axis);
	virtual ~mouse_wheel_mapping() = default;
	mouse_wheel_mapping& operator=(const mouse_wheel_mapping& mapping);
	virtual mapping_type get_type() const;

	input::mouse* mouse;
	mouse_wheel_axis axis;
};

inline mapping_type mouse_wheel_mapping::get_type() const
{
	return mapping_type::mouse_wheel;
}

/**
 * A mapping between a control and a mouse button.
 */
class mouse_button_mapping: public mapping
{
public:
	mouse_button_mapping() = default;
	mouse_button_mapping(const mouse_button_mapping& mapping);
	mouse_button_mapping(input::control* control, input::mouse* mouse, int button);
	virtual ~mouse_button_mapping() = default;
	mouse_button_mapping& operator=(const mouse_button_mapping& mapping);
	virtual mapping_type get_type() const;

	input::mouse* mouse;
	int button;
};

inline mapping_type mouse_button_mapping::get_type() const
{
	return mapping_type::mouse_button;
}

/**
 * A mapping between a control and a gamepad axis.
 */
class gamepad_axis_mapping: public mapping
{
public:
	gamepad_axis_mapping() = default;
	gamepad_axis_mapping(const gamepad_axis_mapping& mapping);
	gamepad_axis_mapping(input::control* control, gamepad* controller, gamepad_axis axis, bool negative);
	virtual ~gamepad_axis_mapping() = default;
	gamepad_axis_mapping& operator=(const gamepad_axis_mapping& mapping);
	virtual mapping_type get_type() const;

	gamepad* controller;
	gamepad_axis axis;
	bool negative;
};

inline mapping_type gamepad_axis_mapping::get_type() const
{
	return mapping_type::gamepad_axis;
}

/**
 * A mapping between a control and a gamepad button.
 *
 * @ingroup input.
 */
class gamepad_button_mapping: public mapping
{
public:
	gamepad_button_mapping() = default;
	gamepad_button_mapping(const gamepad_button_mapping& mapping);
	gamepad_button_mapping(input::control* control, gamepad* controller, gamepad_button button);
	virtual ~gamepad_button_mapping() = default;
	gamepad_button_mapping& operator=(const gamepad_button_mapping& mapping);
	virtual mapping_type get_type() const;

	gamepad* controller;
	gamepad_button button;
};

inline mapping_type gamepad_button_mapping::get_type() const
{
	return mapping_type::gamepad_button;
}

} // namespace input

#endif // ANTKEEPER_INPUT_MAPPING_HPP

