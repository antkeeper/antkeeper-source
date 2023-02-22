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
