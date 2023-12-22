// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/app/input-manager.hpp>
#include <engine/app/sdl/sdl-input-manager.hpp>

namespace app {

std::unique_ptr<input_manager> input_manager::instance()
{
	return std::make_unique<sdl_input_manager>();
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
	// Connect gamepad event signals to the event dispatcher
	m_subscriptions.emplace(&device, device.get_connected_channel().subscribe(m_event_dispatcher));
	m_subscriptions.emplace(&device, device.get_disconnected_channel().subscribe(m_event_dispatcher));
	m_subscriptions.emplace(&device, device.get_axis_moved_channel().subscribe(m_event_dispatcher));
	m_subscriptions.emplace(&device, device.get_button_pressed_channel().subscribe(m_event_dispatcher));
	m_subscriptions.emplace(&device, device.get_button_released_channel().subscribe(m_event_dispatcher));
	
	// Add gamepad to list of gamepads
	m_gamepads.emplace(&device);
}

void input_manager::register_keyboard(input::keyboard& device)
{
	// Connect keyboard event signals to the event dispatcher
	m_subscriptions.emplace(&device, device.get_connected_channel().subscribe(m_event_dispatcher));
	m_subscriptions.emplace(&device, device.get_disconnected_channel().subscribe(m_event_dispatcher));
	m_subscriptions.emplace(&device, device.get_key_pressed_channel().subscribe(m_event_dispatcher));
	m_subscriptions.emplace(&device, device.get_key_released_channel().subscribe(m_event_dispatcher));
	m_subscriptions.emplace(&device, device.get_text_input_channel().subscribe(m_event_dispatcher));
	m_subscriptions.emplace(&device, device.get_text_edit_channel().subscribe(m_event_dispatcher));
	
	// Add keyboard to list of keyboards
	m_keyboards.emplace(&device);
}

void input_manager::register_mouse(input::mouse& device)
{
	// Connect mouse event signals to the event dispatcher
	m_subscriptions.emplace(&device, device.get_connected_channel().subscribe(m_event_dispatcher));
	m_subscriptions.emplace(&device, device.get_disconnected_channel().subscribe(m_event_dispatcher));
	m_subscriptions.emplace(&device, device.get_button_pressed_channel().subscribe(m_event_dispatcher));
	m_subscriptions.emplace(&device, device.get_button_released_channel().subscribe(m_event_dispatcher));
	m_subscriptions.emplace(&device, device.get_moved_channel().subscribe(m_event_dispatcher));
	m_subscriptions.emplace(&device, device.get_scrolled_channel().subscribe(m_event_dispatcher));
	
	// Add mouse to list of mice
	m_mice.emplace(&device);
}

void input_manager::unregister_device(input::device& device)
{
	m_subscriptions.erase(&device);
	
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
	m_gamepads.erase(&gamepad);
}

void input_manager::unregister_keyboard(input::keyboard& keyboard)
{
	m_keyboards.erase(&keyboard);
}

void input_manager::unregister_mouse(input::mouse& mouse)
{
	m_mice.erase(&mouse);
}

} // namespace app
