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

#ifndef ANTKEEPER_INPUT_MAPPER_HPP
#define ANTKEEPER_INPUT_MAPPER_HPP

#include "event/subscription.hpp"
#include "event/queue.hpp"
#include "event/publisher.hpp"
#include "input/input-events.hpp"
#include "input/mapping.hpp"
#include <memory>
#include <vector>

namespace input {

/**
 * Listens for input events and generates corresponding input mappings.
 */
class mapper
{
public:
	/**
	 * Connects the input event signals of an event queue to the mapper.
	 *
	 * @param queue Event queue to connect.
	 */
	void connect(::event::queue& queue);
	
	/**
	 * Disconnects all input event signals from the mapper.
	 */
	void disconnect();
	
	/// Returns the channel through which input mapped events are published.
	[[nodiscard]] inline ::event::channel<input_mapped_event>& get_input_mapped_channel() noexcept
	{
		return input_mapped_publisher.channel();
	}
	
private:
	void handle_gamepad_axis_moved(const gamepad_axis_moved_event& event);
	void handle_gamepad_button_pressed(const gamepad_button_pressed_event& event);
	void handle_key_pressed(const key_pressed_event& event);
	void handle_mouse_button_pressed(const mouse_button_pressed_event& event);
	void handle_mouse_moved(const mouse_moved_event& event);
	void handle_mouse_scrolled(const mouse_scrolled_event& event);
	
	std::vector<std::shared_ptr<::event::subscription>> subscriptions;
	::event::publisher<input_mapped_event> input_mapped_publisher;
};

} // namespace input

#endif // ANTKEEPER_INPUT_MAPPER_HPP
