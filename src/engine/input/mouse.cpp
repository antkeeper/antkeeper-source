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

#include <engine/input/mouse.hpp>

namespace input {

void mouse::press(mouse_button button)
{
	button_pressed_publisher.publish({this, position, button});
}

void mouse::release(mouse_button button)
{
	button_released_publisher.publish({this, position, button});
}

void mouse::move(const math::vec2<std::int32_t>& position, const math::vec2<std::int32_t>& difference)
{
	this->position = position;
	moved_publisher.publish({this, position, difference});
}

void mouse::scroll(const math::fvec2& velocity)
{
	scrolled_publisher.publish({this, position, velocity});
}

} // namespace input
