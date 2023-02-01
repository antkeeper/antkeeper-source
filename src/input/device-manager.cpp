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

#include "input/device-manager.hpp"

namespace input {

void device_manager::register_device(device& device)
{
	subscriptions.emplace(&device, device.get_connected_channel().subscribe(event_queue));
	subscriptions.emplace(&device, device.get_disconnected_channel().subscribe(event_queue));
	
	switch (device.get_device_type())
	{
		case device_type::gamepad:
			register_gamepad(static_cast<gamepad&>(device));
			break;
		
		case device_type::keyboard:
			register_keyboard(static_cast<keyboard&>(device));
			break;
		
		case device_type::mouse:
			register_mouse(static_cast<mouse&>(device));
			break;
		
		default:
			//std::unreachable();
			break;
	}
}

void device_manager::unregister_device(device& device)
{
	subscriptions.erase(&device);
	
	switch (device.get_device_type())
	{
		case device_type::gamepad:
			unregister_gamepad(static_cast<gamepad&>(device));
			break;
		
		case device_type::keyboard:
			unregister_keyboard(static_cast<keyboard&>(device));
			break;
		
		case device_type::mouse:
			unregister_mouse(static_cast<mouse&>(device));
			break;
		
		default:
			//std::unreachable();
			break;
	}
}

void device_manager::register_gamepad(gamepad& gamepad)
{
	// Connect gamepad event signals to the event queue
	subscriptions.emplace(&gamepad, gamepad.get_axis_moved_channel().subscribe(event_queue));
	subscriptions.emplace(&gamepad, gamepad.get_button_pressed_channel().subscribe(event_queue));
	subscriptions.emplace(&gamepad, gamepad.get_button_released_channel().subscribe(event_queue));
	
	// Add gamepad to list of gamepads
	gamepads.emplace(&gamepad);
}

void device_manager::register_keyboard(keyboard& keyboard)
{
	// Connect keyboard event signals to the event queue
	subscriptions.emplace(&keyboard, keyboard.get_key_pressed_channel().subscribe(event_queue));
	subscriptions.emplace(&keyboard, keyboard.get_key_released_channel().subscribe(event_queue));
	
	// Add keyboard to list of keyboards
	keyboards.emplace(&keyboard);
}

void device_manager::register_mouse(mouse& mouse)
{
	// Connect mouse event signals to the event queue
	subscriptions.emplace(&mouse, mouse.get_button_pressed_channel().subscribe(event_queue));
	subscriptions.emplace(&mouse, mouse.get_button_released_channel().subscribe(event_queue));
	subscriptions.emplace(&mouse, mouse.get_moved_channel().subscribe(event_queue));
	subscriptions.emplace(&mouse, mouse.get_scrolled_channel().subscribe(event_queue));
	
	// Add mouse to list of mice
	mice.emplace(&mouse);
}

void device_manager::unregister_gamepad(gamepad& gamepad)
{
	gamepads.erase(&gamepad);
}

void device_manager::unregister_keyboard(keyboard& keyboard)
{
	keyboards.erase(&keyboard);
}

void device_manager::unregister_mouse(mouse& mouse)
{
	mice.erase(&mouse);
}

} // namespace input
