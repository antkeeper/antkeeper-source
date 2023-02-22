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

#include <engine/app/input-manager.hpp>
#include <engine/app/sdl/sdl-input-manager.hpp>

namespace app {

input_manager* input_manager::instance()
{
	return new sdl_input_manager();
}

void input_manager::register_device(input::device& device)
{
	switch (device.get_device_type())
	{
		case input::device_type::gamepad:
			register_gamepad(static_cast<input::gamepad&>(device));
			break;
		
		case input::device_type::keyboard:
			register_keyboard(static_cast<input::keyboard&>(device));
			break;
		
		case input::device_type::mouse:
			register_mouse(static_cast<input::mouse&>(device));
			break;
		
		default:
			//std::unreachable();
			break;
	}
}

void input_manager::register_gamepad(input::gamepad& device)
{
	// Connect gamepad event signals to the event queue
	subscriptions.emplace(&device, device.get_connected_channel().subscribe(event_queue));
	subscriptions.emplace(&device, device.get_disconnected_channel().subscribe(event_queue));
	subscriptions.emplace(&device, device.get_axis_moved_channel().subscribe(event_queue));
	subscriptions.emplace(&device, device.get_button_pressed_channel().subscribe(event_queue));
	subscriptions.emplace(&device, device.get_button_released_channel().subscribe(event_queue));
	
	// Add gamepad to list of gamepads
	gamepads.emplace(&device);
}

void input_manager::register_keyboard(input::keyboard& device)
{
	// Connect keyboard event signals to the event queue
	subscriptions.emplace(&device, device.get_connected_channel().subscribe(event_queue));
	subscriptions.emplace(&device, device.get_disconnected_channel().subscribe(event_queue));
	subscriptions.emplace(&device, device.get_key_pressed_channel().subscribe(event_queue));
	subscriptions.emplace(&device, device.get_key_released_channel().subscribe(event_queue));
	
	// Add keyboard to list of keyboards
	keyboards.emplace(&device);
}

void input_manager::register_mouse(input::mouse& device)
{
	// Connect mouse event signals to the event queue
	subscriptions.emplace(&device, device.get_connected_channel().subscribe(event_queue));
	subscriptions.emplace(&device, device.get_disconnected_channel().subscribe(event_queue));
	subscriptions.emplace(&device, device.get_button_pressed_channel().subscribe(event_queue));
	subscriptions.emplace(&device, device.get_button_released_channel().subscribe(event_queue));
	subscriptions.emplace(&device, device.get_moved_channel().subscribe(event_queue));
	subscriptions.emplace(&device, device.get_scrolled_channel().subscribe(event_queue));
	
	// Add mouse to list of mice
	mice.emplace(&device);
}

void input_manager::unregister_device(input::device& device)
{
	subscriptions.erase(&device);
	
	switch (device.get_device_type())
	{
		case input::device_type::gamepad:
			unregister_gamepad(static_cast<input::gamepad&>(device));
			break;
		
		case input::device_type::keyboard:
			unregister_keyboard(static_cast<input::keyboard&>(device));
			break;
		
		case input::device_type::mouse:
			unregister_mouse(static_cast<input::mouse&>(device));
			break;
		
		default:
			//std::unreachable();
			break;
	}
}

void input_manager::unregister_gamepad(input::gamepad& gamepad)
{
	gamepads.erase(&gamepad);
}

void input_manager::unregister_keyboard(input::keyboard& keyboard)
{
	keyboards.erase(&keyboard);
}

void input_manager::unregister_mouse(input::mouse& mouse)
{
	mice.erase(&mouse);
}

} // namespace app
