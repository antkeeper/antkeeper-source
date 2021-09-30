/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEER_INPUT_EVENT_ROUTER_HPP
#define ANTKEEER_INPUT_EVENT_ROUTER_HPP

#include "event/input-events.hpp"
#include "event/event-handler.hpp"
#include "event/event-dispatcher.hpp"
#include <list>
#include <map>

namespace input {

class control;
class mapping;
class key_mapping;
class mouse_motion_mapping;
class mouse_wheel_mapping;
class mouse_button_mapping;
class gamepad_axis_mapping;
class gamepad_button_mapping;
enum class mouse_motion_axis;
enum class mouse_wheel_axis;

/**
 * Uses input mappings to route input events to controls.
 */
class event_router:
	public event_handler<key_pressed_event>,
	public event_handler<key_released_event>,
	public event_handler<mouse_moved_event>,
	public event_handler<mouse_wheel_scrolled_event>,
	public event_handler<mouse_button_pressed_event>,
	public event_handler<mouse_button_released_event>,
	public event_handler<gamepad_axis_moved_event>,
	public event_handler<gamepad_button_pressed_event>,
	public event_handler<gamepad_button_released_event>
{
public:
	/**
	 * Creates an input router and subscribes it to the input events of the specified event dispatcher.
	 */
	event_router();

	/**
	 * Destroys an input router and unsubscribes it from input events.
	 */
	~event_router();

	/**
	 * Adds an input mapping to the router.
	 *
	 * @param mapping Input mapping to add.
	 */
	void add_mapping(const mapping& mapping);


	/**
	 * Removes all input mappings from the router that are associated with the specified control.
	 *
	 * @param control control with which associated input mappings should be removed.
	 */
	void remove_mappings(control* control);

	/**
	 * Sets the event dispatcher to which this input event router will subscribe itself.
	 */
	void set_event_dispatcher(event_dispatcher* event_dispatcher);

	/**
	 * Removes all input mappings from the router.
	 */
	void remove_mappings();

	/// Returns a list of mappings for the specified control, or nullptr if the control is unmapped.
	const std::list<mapping*>* get_mappings(control* control) const;
	
private:
	virtual void handle_event(const key_pressed_event& event);
	virtual void handle_event(const key_released_event& event);
	virtual void handle_event(const mouse_moved_event& event);
	virtual void handle_event(const mouse_wheel_scrolled_event& event);
	virtual void handle_event(const mouse_button_pressed_event& event);
	virtual void handle_event(const mouse_button_released_event& event);
	virtual void handle_event(const gamepad_axis_moved_event& event);
	virtual void handle_event(const gamepad_button_pressed_event& event);
	virtual void handle_event(const gamepad_button_released_event& event);

	event_dispatcher* event_dispatcher;
	std::map<control*, std::list<mapping*>> controls;
	std::list<key_mapping*> key_mappings;
	std::list<mouse_motion_mapping*> mouse_motion_mappings;
	std::list<mouse_wheel_mapping*> mouse_wheel_mappings;
	std::list<mouse_button_mapping*> mouse_button_mappings;
	std::list<gamepad_axis_mapping*> gamepad_axis_mappings;
	std::list<gamepad_button_mapping*> gamepad_button_mappings;
};

} // namespace input

#endif // ANTKEEER_INPUT_EVENT_ROUTER_HPP

