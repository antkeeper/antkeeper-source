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

#ifndef ANTKEEPER_UI_MOUSE_TRACKER_HPP
#define ANTKEEPER_UI_MOUSE_TRACKER_HPP

#include "event/input-events.hpp"
#include "event/event-handler.hpp"
#include <functional>

namespace ui {

class mouse_tracker:
	public event_handler<mouse_moved_event>,
	public event_handler<mouse_button_pressed_event>,
	public event_handler<mouse_button_released_event>,
	public event_handler<mouse_wheel_scrolled_event>
{
public:
	mouse_tracker();
	virtual ~mouse_tracker() = default;
	
	void set_mouse_moved_callback(const std::function<void(const mouse_moved_event&)>& callback);
	void set_mouse_button_pressed_callback(const std::function<void(const mouse_button_pressed_event&)>& callback);
	void set_mouse_button_released_callback(const std::function<void(const mouse_button_released_event&)>& callback);
	void set_mouse_wheel_scrolled_callback(const std::function<void(const mouse_wheel_scrolled_event&)>& callback);
	
private:
	virtual void handle_event(const mouse_moved_event& event);
	virtual void handle_event(const mouse_button_pressed_event& event);
	virtual void handle_event(const mouse_button_released_event& event);
	virtual void handle_event(const mouse_wheel_scrolled_event& event);
	
	std::function<void(const mouse_moved_event&)> mouse_moved_callback;
	std::function<void(const mouse_button_pressed_event&)> mouse_button_pressed_callback;
	std::function<void(const mouse_button_released_event&)> mouse_button_released_callback;
	std::function<void(const mouse_wheel_scrolled_event&)> mouse_wheel_scrolled_callback;
};
	
} // namespace ui

#endif // ANTKEEPER_UI_MOUSE_TRACKER_HPP
