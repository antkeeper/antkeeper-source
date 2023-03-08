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

#ifndef ANTKEEER_INPUT_ACTION_MAP_HPP
#define ANTKEEER_INPUT_ACTION_MAP_HPP

#include <engine/event/subscription.hpp>
#include <engine/event/queue.hpp>
#include <engine/input/action.hpp>
#include <engine/input/gamepad-events.hpp>
#include <engine/input/keyboard-events.hpp>
#include <engine/input/mouse-events.hpp>
#include <engine/input/mapping.hpp>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace input {

/**
 * Maps input to a set of contextually-related actions.
 */
class action_map
{
public:
	/**
	 * Enables the mapping of input events to actions.
	 */
	void enable();
	
	/**
	 * Disables the mapping of input events to actions.
	 */
	void disable();
	
	/**
	 * Sets the event queue from which this action map will receive input events.
	 *
	 * @param queue Event queue from which this action map will receive input events.
	 */
	void set_event_queue(event::queue* queue);
	
	/**
	 * Maps input to an action.
	 *
	 * @param action Action to which input will be mapped.
	 * @param mapping Input mapping to add.
	 */
	/// @{
	void add_mapping(action& action, const mapping& mapping);
	void add_gamepad_axis_mapping(action& action, gamepad_axis_mapping mapping);
	void add_gamepad_button_mapping(action& action, gamepad_button_mapping mapping);
	void add_key_mapping(action& action, key_mapping mapping);
	void add_mouse_button_mapping(action& action, mouse_button_mapping mapping);
	void add_mouse_motion_mapping(action& action, mouse_motion_mapping mapping);
	void add_mouse_scroll_mapping(action& action, mouse_scroll_mapping mapping);
	/// @}
	
	/**
	 * Unmaps input from an action.
	 *
	 * @param action Action from which input will be unmapped.
	 * @param type Type of input mapping to remove.
	 */
	void remove_mappings(const action& action, mapping_type type);
	
	/**
	 * Unmaps all input from an action.
	 *
	 * @param action Action from which input will be unmapped.
	 */
	void remove_mappings(const action& action);
	
	/**
	 * Unmaps all input from all actions in the action map.
	 */
	void remove_mappings();
	
	/**
	 * Returns all of the gamepad axis mappings associated with an action.
	 *
	 * @param action Action with which associated mappings will be returned.
	 */
	std::vector<gamepad_axis_mapping> get_gamepad_axis_mappings(const action& action) const;
	
	/**
	 * Returns all of the gamepad button mappings associated with an action.
	 *
	 * @param action Action with which associated mappings will be returned.
	 */
	std::vector<gamepad_button_mapping> get_gamepad_button_mappings(const action& action) const;
	
	/**
	 * Returns all of the key mappings associated with an action.
	 *
	 * @param action Action with which associated mappings will be returned.
	 */
	std::vector<key_mapping> get_key_mappings(const action& action) const;
	
	/**
	 * Returns all of the mouse button mappings associated with an action.
	 *
	 * @param action Action with which associated mappings will be returned.
	 */
	std::vector<mouse_button_mapping> get_mouse_button_mappings(const action& action) const;
	
	/**
	 * Returns all of the mouse motion mappings associated with an action.
	 *
	 * @param action Action with which associated mappings will be returned.
	 */
	std::vector<mouse_motion_mapping> get_mouse_motion_mappings(const action& action) const;
	
	/**
	 * Returns all of the mouse scroll associated with an action.
	 *
	 * @param action Action with which associated mappings will be returned.
	 */
	std::vector<mouse_scroll_mapping> get_mouse_scroll_mappings(const action& action) const;
	
private:
	void subscribe();
	void unsubscribe();
	
	void handle_gamepad_axis_moved(const gamepad_axis_moved_event& event);
	void handle_gamepad_button_pressed(const gamepad_button_pressed_event& event);
	void handle_gamepad_button_released(const gamepad_button_released_event& event);
	void handle_key_pressed(const key_pressed_event& event);
	void handle_key_released(const key_released_event& event);
	void handle_mouse_button_pressed(const mouse_button_pressed_event& event);
	void handle_mouse_button_released(const mouse_button_released_event& event);
	void handle_mouse_moved(const mouse_moved_event& event);
	void handle_mouse_scrolled(const mouse_scrolled_event& event);
	
	event::queue* event_queue{nullptr};
	bool enabled{false};
	std::vector<std::shared_ptr<::event::subscription>> subscriptions;
	std::vector<std::tuple<action*, gamepad_axis_mapping>> gamepad_axis_mappings;
	std::vector<std::tuple<action*, gamepad_button_mapping>> gamepad_button_mappings;
	std::vector<std::tuple<action*, key_mapping>> key_mappings;
	std::vector<std::tuple<action*, mouse_button_mapping>> mouse_button_mappings;
	std::vector<std::tuple<action*, mouse_motion_mapping>> mouse_motion_mappings;
	std::vector<std::tuple<action*, mouse_scroll_mapping>> mouse_scroll_mappings;
};

} // namespace input

#endif // ANTKEEER_INPUT_ACTION_MAP_HPP
