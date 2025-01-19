// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_MAPPER_HPP
#define ANTKEEPER_INPUT_MAPPER_HPP

#include <engine/event/subscription.hpp>
#include <engine/event/dispatcher.hpp>
#include <engine/event/publisher.hpp>
#include <engine/input/gamepad-events.hpp>
#include <engine/input/keyboard-events.hpp>
#include <engine/input/mouse-events.hpp>
#include <engine/input/mapping-events.hpp>
#include <memory>
#include <vector>

namespace input {

/// Listens for input events and generates corresponding input mappings.
class mapper
{
public:
	/// Connects the input event signals of an event dispatcher to the mapper.
	/// @param dispatcher Event dispatcher to connect.
	void connect(::event::dispatcher& dispatcher);
	
	/// Disconnects all input event signals from the mapper.
	void disconnect();
	
	/// Returns the channel through which gamepad axis mapped events are published.
	[[nodiscard]] inline ::event::channel<gamepad_axis_mapped_event>& get_gamepad_axis_mapped_channel() noexcept
	{
		return gamepad_axis_mapped_publisher.channel();
	}
	
	/// Returns the channel through which gamepad button mapped events are published.
	[[nodiscard]] inline ::event::channel<gamepad_button_mapped_event>& get_gamepad_button_mapped_channel() noexcept
	{
		return gamepad_button_mapped_publisher.channel();
	}
	
	/// Returns the channel through which key mapped events are published.
	[[nodiscard]] inline ::event::channel<key_mapped_event>& get_key_mapped_channel() noexcept
	{
		return key_mapped_publisher.channel();
	}
	
	/// Returns the channel through which mouse button mapped events are published.
	[[nodiscard]] inline ::event::channel<mouse_button_mapped_event>& get_mouse_button_mapped_channel() noexcept
	{
		return mouse_button_mapped_publisher.channel();
	}
	
	/// Returns the channel through which mouse motion mapped events are published.
	[[nodiscard]] inline ::event::channel<mouse_motion_mapped_event>& get_mouse_motion_mapped_channel() noexcept
	{
		return mouse_motion_mapped_publisher.channel();
	}
	
	/// Returns the channel through which mouse scroll mapped events are published.
	[[nodiscard]] inline ::event::channel<mouse_scroll_mapped_event>& get_mouse_scroll_mapped_channel() noexcept
	{
		return mouse_scroll_mapped_publisher.channel();
	}
	
private:
	void handle_gamepad_axis_moved(const gamepad_axis_moved_event& event);
	void handle_gamepad_button_pressed(const gamepad_button_pressed_event& event);
	void handle_key_pressed(const key_pressed_event& event);
	void handle_mouse_button_pressed(const mouse_button_pressed_event& event);
	void handle_mouse_moved(const mouse_moved_event& event);
	void handle_mouse_scrolled(const mouse_scrolled_event& event);
	
	std::vector<std::shared_ptr<::event::subscription>> subscriptions;
	::event::publisher<gamepad_axis_mapped_event> gamepad_axis_mapped_publisher;
	::event::publisher<gamepad_button_mapped_event> gamepad_button_mapped_publisher;
	::event::publisher<key_mapped_event> key_mapped_publisher;
	::event::publisher<mouse_button_mapped_event> mouse_button_mapped_publisher;
	::event::publisher<mouse_motion_mapped_event> mouse_motion_mapped_publisher;
	::event::publisher<mouse_scroll_mapped_event> mouse_scroll_mapped_publisher;
};

} // namespace input

#endif // ANTKEEPER_INPUT_MAPPER_HPP
