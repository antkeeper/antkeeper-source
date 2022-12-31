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

#include "ui/mouse-tracker.hpp"

namespace ui {

mouse_tracker::mouse_tracker():
	mouse_moved_callback(nullptr),
	mouse_button_pressed_callback(nullptr),
	mouse_button_released_callback(nullptr),
	mouse_wheel_scrolled_callback(nullptr)
{}

void mouse_tracker::set_mouse_moved_callback(const std::function<void(const mouse_moved_event&)>& callback)
{
	mouse_moved_callback = callback;
}

void mouse_tracker::set_mouse_button_pressed_callback(const std::function<void(const mouse_button_pressed_event&)>& callback)
{
	mouse_button_pressed_callback = callback;
}

void mouse_tracker::set_mouse_button_released_callback(const std::function<void(const mouse_button_released_event&)>& callback)
{
	mouse_button_released_callback = callback;
}

void mouse_tracker::set_mouse_wheel_scrolled_callback(const std::function<void(const mouse_wheel_scrolled_event&)>& callback)
{
	mouse_wheel_scrolled_callback = callback;
}

void mouse_tracker::handle_event(const mouse_moved_event& event)
{
	if (mouse_moved_callback)
		mouse_moved_callback(event);
}

void mouse_tracker::handle_event(const mouse_button_pressed_event& event)
{
	if (mouse_button_pressed_callback)
		mouse_button_pressed_callback(event);
}

void mouse_tracker::handle_event(const mouse_button_released_event& event)
{
	if (mouse_button_released_callback)
		mouse_button_released_callback(event);
}

void mouse_tracker::handle_event(const mouse_wheel_scrolled_event& event)
{
	if (mouse_wheel_scrolled_callback)
		mouse_wheel_scrolled_callback(event);
}

} // namespace ui
