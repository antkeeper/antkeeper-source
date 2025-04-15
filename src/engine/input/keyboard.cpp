// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.input.keyboard;
import engine.event.publisher;
import engine.utility.sized_types;

namespace engine::input
{
	void keyboard::press(scancode scancode, u16 modifiers, bool repeat)
	{
		m_key_pressed_publisher.publish({this, scancode, modifiers, repeat});
	}

	void keyboard::release(scancode scancode, u16 modifiers)
	{
		m_key_released_publisher.publish({this, scancode, modifiers});
	}

	void keyboard::input_text(const std::string& text)
	{
		m_text_input_publisher.publish({this, text});
	}

	void keyboard::edit_text(const std::string& text, usize position, usize length)
	{
		m_text_edit_publisher.publish({this, text, position, length});
	}
}
