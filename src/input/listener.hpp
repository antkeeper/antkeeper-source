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

#ifndef ANTKEEPER_INPUT_LISTENER_HPP
#define ANTKEEPER_INPUT_LISTENER_HPP

#include "event/input-events.hpp"
#include "event/event-handler.hpp"
#include "event/event-dispatcher.hpp"
#include <functional>

namespace input {

/**
 * Listens for all types of input events.
 */
class listener:
	public event_handler<key_pressed_event>,
	public event_handler<mouse_moved_event>,
	public event_handler<mouse_wheel_scrolled_event>,
	public event_handler<mouse_button_pressed_event>,
	public event_handler<gamepad_axis_moved_event>,
	public event_handler<gamepad_button_pressed_event>
{
public:
	/**
	 * Creates an input listener.
	 */
	listener();

	/**
	 * Destroys an input listener.
	 */
	virtual ~listener();

	/**
	 * Sets the event dispatcher to which this input event router will subscribe itself.
	 */
	void set_event_dispatcher(event_dispatcher* event_dispatcher);

	/**
	 * Sets the input event callback function.
	 *
	 * @param callback Callback function which operates on an input event.
	 */
	void set_callback(std::function<void(const event_base&)> event);

	/**
	 * Enables or disables the input listening.
	 *
	 * @param enabled Whether to enable input listening or not.
	 */
	void set_enabled(bool enabled);

	/**
	 * Returns true if input listening is enabled.
	 */
	bool is_enabled() const;
	
private:
	void handle_event(const key_pressed_event& event);
	void handle_event(const mouse_moved_event& event);
	void handle_event(const mouse_wheel_scrolled_event& event);
	void handle_event(const mouse_button_pressed_event& event);
	void handle_event(const gamepad_axis_moved_event& event);
	void handle_event(const gamepad_button_pressed_event& event);

	event_dispatcher* event_dispatcher;
	std::function<void(const event_base&)> callback;
	bool enabled;
};

inline bool listener::is_enabled() const
{
	return enabled;
}

} // namespace input

#endif // ANTKEEPER_INPUT_LISTENER_HPP

