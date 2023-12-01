// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_KEYBOARD_HPP
#define ANTKEEPER_INPUT_KEYBOARD_HPP

#include <engine/input/device.hpp>
#include <engine/input/keyboard-events.hpp>
#include <engine/input/scancode.hpp>
#include <engine/input/modifier-key.hpp>
#include <engine/event/publisher.hpp>

namespace input {

/**
 * A virtual keyboard which generates keyboard-related input events.
 */
class keyboard: public device
{
public:
	/**
	 * Simulates a key press.
	 *
	 * @param scancode Scancode of the key to press.
	 * @param modifiers Bit mask containing the active modifier keys.
	 * @param repeat `true` if the key press is from a key repeat, `false` otherwise.
	 */
	void press(scancode scancode, std::uint16_t modifiers = modifier_key::none, bool repeat = false);
	
	/**
	 * Simulates a key release.
	 *
	 * @param scancode Scancode of the key to release.
	 * @param modifiers Bit mask containing the active modifier keys.
	 */
	void release(scancode scancode, std::uint16_t modifiers = modifier_key::none);
	
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
	[[nodiscard]] inline constexpr device_type get_device_type() const noexcept override
	{
		return device_type::keyboard;
	}

private:
	::event::publisher<key_pressed_event> key_pressed_publisher;
	::event::publisher<key_released_event> key_released_publisher;
};

} // namespace input

#endif // ANTKEEPER_INPUT_KEYBOARD_HPP
