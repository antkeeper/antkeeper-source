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

#ifndef ANTKEEPER_INPUT_KEYBOARD_HPP
#define ANTKEEPER_INPUT_KEYBOARD_HPP

#include "input/device.hpp"
#include "input/input-events.hpp"
#include "input/scancode.hpp"
#include "input/modifier-key.hpp"
#include "event/publisher.hpp"

namespace input {

/**
 * A virtual keyboard which generates keyboard-related input events.
 */
class keyboard: public device
{
public:
	/**
	 * Constructs a keyboard input device.
	 */
	keyboard() = default;
	
	/// Destructs a keyboard input device.
	virtual ~keyboard() = default;
	
	/**
	 * Simulates a key press.
	 *
	 * @param scancode Scancode of the key to press.
	 * @param repeat `true` if the key press is from a key repeat, `false` otherwise.
	 * @param modifiers Bit mask containing the active modifier keys.
	 */
	void press(scancode scancode, bool repeat = false, std::uint16_t modifiers = modifier_key::none);
	
	/**
	 * Simulates a key release.
	 *
	 * @param scancode Scancode of the key to release.
	 * @param repeat `true` if the key release is from a key repeat, `false` otherwise.
	 * @param modifiers Bit mask containing the active modifier keys.
	 */
	void release(scancode scancode, bool repeat = false, std::uint16_t modifiers = modifier_key::none);
	
	/// Returns the channel through which key pressed events are published.
	[[nodiscard]] inline ::event::channel<key_pressed_event>& get_key_pressed_channel() noexcept
	{
		return key_pressed_publisher.channel();
	}
	
	/// Returns the channel through which key released events are published.
	[[nodiscard]] inline ::event::channel<key_released_event>& get_key_released_channel() noexcept
	{
		return key_released_publisher.channel();
	}
	
	/// Returns device_type::keyboard.
	[[nodiscard]] inline virtual constexpr device_type get_device_type() const noexcept
	{
		return device_type::keyboard;
	}

private:
	::event::publisher<key_pressed_event> key_pressed_publisher;
	::event::publisher<key_released_event> key_released_publisher;
};

} // namespace input

#endif // ANTKEEPER_INPUT_KEYBOARD_HPP
