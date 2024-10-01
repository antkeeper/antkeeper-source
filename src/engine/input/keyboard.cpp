// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/input/keyboard.hpp>
#include <engine/input/scancode.hpp>

namespace input {

void keyboard::press(scancode scancode, std::uint16_t modifiers, bool repeat)
{
	m_key_pressed_publisher.publish({this, scancode, modifiers, repeat});
}

void keyboard::release(scancode scancode, std::uint16_t modifiers)
{
	m_key_released_publisher.publish({this, scancode, modifiers});
}

void keyboard::input_text(const std::string& text)
{
	m_text_input_publisher.publish({this, text});
}

void keyboard::edit_text(const std::string& text, std::size_t position, std::size_t length)
{
	m_text_edit_publisher.publish({this, text, position, length});
}

} // namespace input
