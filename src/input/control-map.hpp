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

#ifndef ANTKEEER_INPUT_CONTROL_MAP_HPP
#define ANTKEEER_INPUT_CONTROL_MAP_HPP

#include "event/subscription.hpp"
#include "event/queue.hpp"
#include "input/control.hpp"
#include "input/gamepad-events.hpp"
#include "input/keyboard-events.hpp"
#include "input/mouse-events.hpp"
#include "input/mapping.hpp"
#include <memory>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace input {

/**
 * Maps input to a set of contextually-related controls.
 */
class control_map
{
public:
	/**
	 * Connects the input event signals of an event queue to the control map.
	 *
	 * @param queue Event queue to connect.
	 */
	void connect(::event::queue& queue);
	
	/**
	 * Disconnects all input event signals from the control map.
	 */
	void disconnect();
	
	/**
	 * Maps input to a control.
	 *
	 * @param control Control to which input will be mapped.
	 * @param mapping Input mapping to add.
	 */
	/// @{
	void add_mapping(control& control, const mapping& mapping);
	void add_mapping(control& control, gamepad_axis_mapping&& mapping);
	void add_mapping(control& control, gamepad_button_mapping&& mapping);
	void add_mapping(control& control, key_mapping&& mapping);
	void add_mapping(control& control, mouse_button_mapping&& mapping);
	void add_mapping(control& control, mouse_motion_mapping&& mapping);
	void add_mapping(control& control, mouse_scroll_mapping&& mapping);
	/// @}
	
	/**
	 * Unmaps input from a control.
	 *
	 * @param control Control from which input will be unmapped.
	 * @param type Type of input mapping to remove.
	 */
	void remove_mappings(control& control, mapping_type type);
	
	/**
	 * Unmaps all input from a control.
	 *
	 * @param control Control from which input will be unmapped.
	 */
	void remove_mappings(control& control);
	
	/**
	 * Unmaps all input from all controls in the control map.
	 */
	void remove_mappings();
	
private:
	void handle_gamepad_axis_moved(const gamepad_axis_moved_event& event);
	void handle_gamepad_button_pressed(const gamepad_button_pressed_event& event);
	void handle_gamepad_button_released(const gamepad_button_released_event& event);
	void handle_key_pressed(const key_pressed_event& event);
	void handle_key_released(const key_released_event& event);
	void handle_mouse_button_pressed(const mouse_button_pressed_event& event);
	void handle_mouse_button_released(const mouse_button_released_event& event);
	void handle_mouse_moved(const mouse_moved_event& event);
	void handle_mouse_scrolled(const mouse_scrolled_event& event);
	
	std::vector<std::shared_ptr<::event::subscription>> subscriptions;
	std::vector<std::tuple<control*, gamepad_axis_mapping>> gamepad_axis_mappings;
	std::vector<std::tuple<control*, gamepad_button_mapping>> gamepad_button_mappings;
	std::vector<std::tuple<control*, key_mapping>> key_mappings;
	std::vector<std::tuple<control*, mouse_button_mapping>> mouse_button_mappings;
	std::vector<std::tuple<control*, mouse_motion_mapping>> mouse_motion_mappings;
	std::vector<std::tuple<control*, mouse_scroll_mapping>> mouse_scroll_mappings;
};

} // namespace input

#endif // ANTKEEER_INPUT_CONTROL_MAP_HPP
