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
	/** Destructs a keyboard. */
	~keyboard() override = default;
	
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
	
	/**
	 * Simulates text input.
	 *
	 * @param text Input text.
	 */
	void input_text(const std::string& text);
	
	/**
	 * Simulates text editing.
	 *
	 * @param text Editing text.
	 * @param position Position from which to begin editing.
	 * @param length Number of characters to edit.
	 */
	void edit_text(const std::string& text, std::size_t position, std::size_t length);
	
	/// Returns the channel through which key pressed events are published.
	[[nodiscard]] inline auto& get_key_pressed_channel() noexcept
	{
		return m_key_pressed_publisher.channel();
	}
	
	/// Returns the channel through which key released events are published.
	[[nodiscard]] inline auto& get_key_released_channel() noexcept
	{
		return m_key_released_publisher.channel();
	}
	
	/// Returns the channel through which text input events are published.
	[[nodiscard]] inline auto& get_text_input_channel() noexcept
	{
		return m_text_input_publisher.channel();
	}
	
	/// Returns the channel through which text editing events are published.
	[[nodiscard]] inline auto& get_text_edit_channel() noexcept
	{
		return m_text_edit_publisher.channel();
	}
	
	/// Returns device_type::keyboard.
	[[nodiscard]] inline constexpr device_type get_device_type() const noexcept override
	{
		return device_type::keyboard;
	}

private:
	::event::publisher<key_pressed_event> m_key_pressed_publisher;
	::event::publisher<key_released_event> m_key_released_publisher;
	::event::publisher<text_input_event> m_text_input_publisher;
	::event::publisher<text_edit_event> m_text_edit_publisher;
};

} // namespace input

#endif // ANTKEEPER_INPUT_KEYBOARD_HPP
