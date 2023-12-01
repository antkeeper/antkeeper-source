// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/input/keyboard.hpp>
#include <engine/input/scancode.hpp>

namespace input {

void keyboard::press(scancode scancode, std::uint16_t modifiers, bool repeat)
{
	key_pressed_publisher.publish({this, scancode, modifiers, repeat});
}

void keyboard::release(scancode scancode, std::uint16_t modifiers)
{
	key_released_publisher.publish({this, scancode, modifiers});
}

} // namespace input
