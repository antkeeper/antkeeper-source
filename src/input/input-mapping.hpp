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

#ifndef ANTKEEPER_INPUT_MAPPING_HPP
#define ANTKEEPER_INPUT_MAPPING_HPP

enum class mouse_motion_axis;
enum class mouse_wheel_axis;
enum class scancode;
enum class game_controller_axis;
enum class game_controller_button;
class control;
class keyboard;
class mouse;
class game_controller;

/**
 * Enumerates the supported types of control mappings.
 */
enum class input_mapping_type
{
	key,
	mouse_motion,
	mouse_wheel,
	mouse_button,
	game_controller_axis,
	game_controller_button
};

/**
 * Abstract base class for input mappings.
 */
class input_mapping
{
public:
	input_mapping() = default;
	input_mapping(::control* control);
	virtual ~input_mapping() = default;

	/// Returns this control mapping's type.
	virtual input_mapping_type get_type() const = 0;

	control* control;
};

/**
 * A mapping between a control and a keyboard key.
 */
class key_mapping: public input_mapping
{
public:
	key_mapping() = default;
	key_mapping(const key_mapping& mapping);
	key_mapping(::control* control, keyboard* keyboard, scancode scancode);
	virtual ~key_mapping() = default;
	key_mapping& operator=(const key_mapping& mapping);
	virtual input_mapping_type get_type() const;

	keyboard* keyboard;
	scancode scancode;
};

inline input_mapping_type key_mapping::get_type() const
{
	return input_mapping_type::key;
}

/**
 * A mapping between a control and a mouse motion axis.
 */
class mouse_motion_mapping: public input_mapping
{
public:
	mouse_motion_mapping() = default;
	mouse_motion_mapping(const mouse_motion_mapping& mapping);
	mouse_motion_mapping(::control* control, ::mouse* mouse, mouse_motion_axis axis);
	virtual ~mouse_motion_mapping() = default;
	mouse_motion_mapping& operator=(const mouse_motion_mapping& mapping);
	virtual input_mapping_type get_type() const;

	mouse* mouse;
	mouse_motion_axis axis;
};

inline input_mapping_type mouse_motion_mapping::get_type() const
{
	return input_mapping_type::mouse_motion;
}

/**
 * A mapping between a control and a mouse wheel axis.
 */
class mouse_wheel_mapping: public input_mapping
{
public:
	mouse_wheel_mapping() = default;
	mouse_wheel_mapping(const mouse_wheel_mapping& mapping);
	mouse_wheel_mapping(::control* control, mouse* mouse, mouse_wheel_axis axis);
	virtual ~mouse_wheel_mapping() = default;
	mouse_wheel_mapping& operator=(const mouse_wheel_mapping& mapping);
	virtual input_mapping_type get_type() const;

	mouse* mouse;
	mouse_wheel_axis axis;
};

inline input_mapping_type mouse_wheel_mapping::get_type() const
{
	return input_mapping_type::mouse_wheel;
}

/**
 * A mapping between a control and a mouse button.
 */
class mouse_button_mapping: public input_mapping
{
public:
	mouse_button_mapping() = default;
	mouse_button_mapping(const mouse_button_mapping& mapping);
	mouse_button_mapping(::control* control, mouse* mouse, int button);
	virtual ~mouse_button_mapping() = default;
	mouse_button_mapping& operator=(const mouse_button_mapping& mapping);
	virtual input_mapping_type get_type() const;

	mouse* mouse;
	int button;
};

inline input_mapping_type mouse_button_mapping::get_type() const
{
	return input_mapping_type::mouse_button;
}

/**
 * A mapping between a control and a game controller axis.
 */
class game_controller_axis_mapping: public input_mapping
{
public:
	game_controller_axis_mapping() = default;
	game_controller_axis_mapping(const game_controller_axis_mapping& mapping);
	game_controller_axis_mapping(::control* control, game_controller* game_controller, game_controller_axis axis, bool negative);
	virtual ~game_controller_axis_mapping() = default;
	game_controller_axis_mapping& operator=(const game_controller_axis_mapping& mapping);
	virtual input_mapping_type get_type() const;

	game_controller* game_controller;
	game_controller_axis axis;
	bool negative;
};

inline input_mapping_type game_controller_axis_mapping::get_type() const
{
	return input_mapping_type::game_controller_axis;
}

/**
 * A mapping between a control and a game controller button.
 *
 * @ingroup input.
 */
class game_controller_button_mapping: public input_mapping
{
public:
	game_controller_button_mapping() = default;
	game_controller_button_mapping(const game_controller_button_mapping& mapping);
	game_controller_button_mapping(::control* control, game_controller* game_controller, game_controller_button button);
	virtual ~game_controller_button_mapping() = default;
	game_controller_button_mapping& operator=(const game_controller_button_mapping& mapping);
	virtual input_mapping_type get_type() const;

	game_controller* game_controller;
	game_controller_button button;
};

inline input_mapping_type game_controller_button_mapping::get_type() const
{
	return input_mapping_type::game_controller_button;
}

#endif // ANTKEEPER_INPUT_MAPPING_HPP

