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

#ifndef ANTKEEPER_INPUT_MAPPING_HPP
#define ANTKEEPER_INPUT_MAPPING_HPP

#include "input/mapping-type.hpp"
#include "input/gamepad-axis.hpp"
#include "input/gamepad-button.hpp"
#include "input/mouse-button.hpp"
#include "input/mouse-motion-axis.hpp"
#include "input/mouse-scroll-axis.hpp"
#include "input/scancode.hpp"
#include <cstdint>

namespace input {

class control;
class gamepad;
class keyboard;
class mouse;

/**
 * Abstract base class for input mappings.
 */
class mapping
{
public:
	/**
	 * Constructs an input mapping.
	 */
	mapping() = default;
	
	/// Destructs an input mapping.
	virtual ~mapping() = default;
	
	/// Returns the input mapping type.
	[[nodiscard]] virtual constexpr mapping_type get_mapping_type() const noexcept = 0;
};

/**
 * Maps a direction along a gamepad axis to a control input value.
 */
class gamepad_axis_mapping: public mapping
{
public:
	/**
	 * Constructs a gamepad axis mapping.
	 *
	 * @param gamepad Pointer to the gamepad to map, or `nullptr` if input from any gamepad will be mapped.
	 * @param axis Gamepad axis to map.
	 * @param direction Sign bit of the direction to map.
	 */
	/// @{
	gamepad_axis_mapping(input::gamepad* gamepad, gamepad_axis axis, bool direction);
	gamepad_axis_mapping() = default;
	/// @}
	
	/// Destructs a gamepad axis mapping.
	virtual ~gamepad_axis_mapping() = default;
	
	/// Returns mapping_type::gamepad_axis.
	[[nodiscard]] inline virtual constexpr mapping_type get_mapping_type() const noexcept
	{
		return mapping_type::gamepad_axis;
	}
	
	/// Pointer to the mapped gamepad, or `nullptr` if input from any gamepad is accepted.
	input::gamepad* gamepad;
	
	/// Mapped gamepad axis.
	gamepad_axis axis;
	
	/// Sign bit of the mapped direction.
	bool direction;
};

/**
 * Maps a gamepad button to a control input value.
 */
class gamepad_button_mapping: public mapping
{
public:
	/**
	 * Constructs a gamepad button mapping.
	 *
	 * @param gamepad Pointer to the gamepad to map, or `nullptr` if input from any gamepad will be mapped.
	 * @param button Gamepad button to map.
	 */
	/// @{
	gamepad_button_mapping(input::gamepad* gamepad, gamepad_button button);
	gamepad_button_mapping() = default;
	/// @}
	
	/// Destructs a gamepad button mapping.
	virtual ~gamepad_button_mapping() = default;
	
	/// Returns mapping_type::gamepad_button.
	[[nodiscard]] inline virtual constexpr mapping_type get_mapping_type() const noexcept
	{
		return mapping_type::gamepad_button;
	}
	
	/// Pointer to the mapped gamepad, or `nullptr` if input from any gamepad is accepted.
	input::gamepad* gamepad;
	
	/// Mapped gamepad button.
	gamepad_button button;
};

/**
 * Maps a keyboard key to a control input value.
 */
class key_mapping: public mapping
{
public:
	/**
	 * Constructs a key mapping.
	 *
	 * @param keyboard Pointer to the keyboard to map, or `nullptr` if input from any keyboard will be mapped.
	 * @param scancode Scancode of the key to map.
	 * @param repeat `false` if the mapping should ignore key repeats, `true` otherwise.
	 * @param modifiers Modifier keys bitmask.
	 */
	/// @{
	key_mapping(input::keyboard* keyboard, input::scancode scancode, bool repeat = false, std::uint16_t modifiers = 0);
	key_mapping() = default;
	/// @}
	
	/// Destructs a keyboard key mapping.
	virtual ~key_mapping() = default;
	
	/// Returns mapping_type::key.
	[[nodiscard]] inline virtual constexpr mapping_type get_mapping_type() const noexcept
	{
		return mapping_type::key;
	}
	
	/// Pointer to the mapped keyboard, or `nullptr` if input from any keyboard is accepted.
	input::keyboard* keyboard;
	
	/// Scancode of the mapped key.
	scancode scancode;
	
	/// `false` if the mapping ignores key repeats, `true` otherwise.
	bool repeat;
	
	/// Modifier keys bitbask.
	std::uint16_t modifiers;
};

/**
 * Maps a mouse button to a control input value.
 */
class mouse_button_mapping: public mapping
{
public:
	/**
	 * Constructs a mouse button mapping.
	 *
	 * @param mouse Pointer to the mouse to map, or `nullptr` if input from any mouse will be mapped.
	 * @param button Mouse button to map.
	 */
	/// @{
	mouse_button_mapping(input::mouse* mouse, mouse_button button);
	mouse_button_mapping() = default;
	/// @}
	
	/// Destructs a mouse button mapping.
	virtual ~mouse_button_mapping() = default;
	
	/// Returns mapping_type::mouse_button.
	[[nodiscard]] inline virtual constexpr mapping_type get_mapping_type() const noexcept
	{
		return mapping_type::mouse_button;
	}
	
	/// Pointer to the mapped mouse, or `nullptr` if input from any mouse is accepted.
	input::mouse* mouse;
	
	/// Mapped mouse button.
	mouse_button button;
};

/**
 * Maps a direction along a mouse motion axis to a control input value.
 */
class mouse_motion_mapping: public mapping
{
public:
	/**
	 * Constructs a mouse motion mapping.
	 *
	 * @param mouse Pointer to the mouse to map, or `nullptr` if input from any mouse will be mapped.
	 * @param axis Mouse motion axis to map.
	 * @param direction Sign bit of the direction to map.
	 */
	/// @{
	mouse_motion_mapping(input::mouse* mouse, mouse_motion_axis axis, bool direction);
	mouse_motion_mapping() = default;
	/// @}
	
	/// Destructs a mouse motion mapping.
	virtual ~mouse_motion_mapping() = default;
	
	/// Returns mapping_type::mouse_motion.
	[[nodiscard]] inline virtual constexpr mapping_type get_mapping_type() const noexcept
	{
		return mapping_type::mouse_motion;
	}
	
	/// Pointer to the mapped mouse, or `nullptr` if input from any mouse is accepted.
	input::mouse* mouse;
	
	/// Mapped mouse motion axis.
	mouse_motion_axis axis;
	
	/// Sign bit of the mapped direction.
	bool direction;
};

/**
 * Maps a direction along a mouse scroll axis to a control input value.
 */
class mouse_scroll_mapping: public mapping
{
public:
	/**
	 * Constructs a mouse scroll mapping.
	 *
	 * @param control Control to which input will be mapped.
	 * @param mouse Pointer to the mouse to map, or `nullptr` if input from any mouse will be mapped.
	 * @param axis Mouse scroll axis to map.
	 * @param direction Sign bit of the direction to map.
	 */
	/// @{
	mouse_scroll_mapping(input::mouse* mouse, mouse_scroll_axis axis, bool direction);
	mouse_scroll_mapping() = default;
	/// @}
	
	/// Destructs a mouse scroll mapping.
	virtual ~mouse_scroll_mapping() = default;
	
	/// Returns mapping_type::mouse_scroll.
	[[nodiscard]] inline virtual constexpr mapping_type get_mapping_type() const noexcept
	{
		return mapping_type::mouse_scroll;
	}
	
	/// Pointer to the mapped mouse, or `nullptr` if input from any mouse is accepted.
	input::mouse* mouse;
	
	/// Mapped mouse scroll axis.
	mouse_scroll_axis axis;
	
	/// Sign bit of the mapped direction.
	bool direction;
};

} // namespace input

#endif // ANTKEEPER_INPUT_MAPPING_HPP
