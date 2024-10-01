// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/input/mapper.hpp>
#include <cmath>

namespace input {

void mapper::connect(::event::dispatcher& dispatcher)
{
	subscriptions.emplace_back(dispatcher.subscribe<gamepad_axis_moved_event>(std::bind_front(&mapper::handle_gamepad_axis_moved, this)));
	subscriptions.emplace_back(dispatcher.subscribe<gamepad_button_pressed_event>(std::bind_front(&mapper::handle_gamepad_button_pressed, this)));
	subscriptions.emplace_back(dispatcher.subscribe<key_pressed_event>(std::bind_front(&mapper::handle_key_pressed, this)));
	subscriptions.emplace_back(dispatcher.subscribe<mouse_button_pressed_event>(std::bind_front(&mapper::handle_mouse_button_pressed, this)));
	subscriptions.emplace_back(dispatcher.subscribe<mouse_moved_event>(std::bind_front(&mapper::handle_mouse_moved, this)));
	subscriptions.emplace_back(dispatcher.subscribe<mouse_scrolled_event>(std::bind_front(&mapper::handle_mouse_scrolled, this)));
}

void mapper::disconnect()
{
	subscriptions.clear();
}

void mapper::handle_gamepad_axis_moved(const gamepad_axis_moved_event& event)
{
	if (std::abs(event.position) > 0.5f)
	{
		gamepad_axis_mapped_publisher.publish({gamepad_axis_mapping(event.gamepad, event.axis, std::signbit(event.position))});
	}
}

void mapper::handle_gamepad_button_pressed(const gamepad_button_pressed_event& event)
{
	gamepad_button_mapped_publisher.publish({gamepad_button_mapping(event.gamepad, event.button)});
}

void mapper::handle_key_pressed(const key_pressed_event& event)
{
	if (!event.repeat)
	{
		key_mapped_publisher.publish({key_mapping(event.keyboard, event.scancode)});
	}
}

void mapper::handle_mouse_button_pressed(const mouse_button_pressed_event& event)
{
	mouse_button_mapped_publisher.publish({mouse_button_mapping(event.mouse, event.button)});
}

void mapper::handle_mouse_moved(const mouse_moved_event& event)
{
	if (event.difference.x())
	{
		mouse_motion_mapped_publisher.publish({mouse_motion_mapping(event.mouse, mouse_motion_axis::x, std::signbit(static_cast<float>(event.difference.x())))});
	}
	
	if (event.difference.y())
	{
		mouse_motion_mapped_publisher.publish({mouse_motion_mapping(event.mouse, mouse_motion_axis::y, std::signbit(static_cast<float>(event.difference.y())))});
	}
}

void mapper::handle_mouse_scrolled(const mouse_scrolled_event& event)
{
	if (event.velocity.x())
	{
		mouse_scroll_mapped_publisher.publish({mouse_scroll_mapping(event.mouse, mouse_scroll_axis::x, std::signbit(event.velocity.x()))});
	}
	
	if (event.velocity.y())
	{
		mouse_scroll_mapped_publisher.publish({mouse_scroll_mapping(event.mouse, mouse_scroll_axis::y, std::signbit(event.velocity.y()))});
	}
}

} // namespace input
