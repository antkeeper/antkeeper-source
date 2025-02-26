// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_KEYBOARD_EVENTS_HPP
#define ANTKEEPER_INPUT_KEYBOARD_EVENTS_HPP

#include <engine/input/scancode.hpp>
#include <engine/input/modifier-key.hpp>
#include <cstdint>
#include <string>

namespace input {

class keyboard;

/// Event generated when a keyboard key has been pressed.
struct key_pressed_event
{
	/// Keyboard that generated the event.
	keyboard* keyboard{nullptr};
	
	/// Scancode of the key being pressed.
	scancode scancode{0};
	
	/// Bit mask containing the active modifier keys.
	std::uint16_t modifiers{0};
	
	/// `true` if the key press was generated by a key repeat, `false` otherwise.
	bool repeat{false};
};

/// Event generated when a keyboard key has been released.
struct key_released_event
{
	/// Keyboard that generated the event.
	keyboard* keyboard{nullptr};
	
	/// Scancode of the key being released.
	scancode scancode{0};
	
	/// Bit mask containing the active modifier keys.
	std::uint16_t modifiers{0};
};

/// Event generated when text has been input.
struct text_input_event
{
	/// Keyboard that generated the event.
	keyboard* keyboard{nullptr};
	
	/// Input text.
	std::string text;
};

/// Event generated when input text is edited.
struct text_edit_event
{
	/// Keyboard that generated the event.
	keyboard* keyboard{nullptr};
	
	/// Edited text.
	std::string text;
	
	/// Position from which to begin editing.
	std::size_t position{};
	
	/// Number of characters to edit.
	std::size_t length{};
};

} // namespace input

#endif // ANTKEEPER_INPUT_KEYBOARD_EVENTS_HPP
