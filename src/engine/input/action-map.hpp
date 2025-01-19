// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEER_INPUT_ACTION_MAP_HPP
#define ANTKEEER_INPUT_ACTION_MAP_HPP

#include <engine/event/subscription.hpp>
#include <engine/event/dispatcher.hpp>
#include <engine/input/action.hpp>
#include <engine/input/gamepad-events.hpp>
#include <engine/input/keyboard-events.hpp>
#include <engine/input/mouse-events.hpp>
#include <engine/input/mapping.hpp>
#include <engine/input/input-update-event.hpp>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace input {

/// Maps input to a set of contextually-related actions.
class action_map
{
public:
	/// Enables the mapping of input events to actions.
	void enable();
	
	/// Disables the mapping of input events to actions.
	void disable();
	
	/// Resets the activation states of each action in the action map.
	void reset();
	
	/// Sets the event dispatcher from which this action map will receive input events.
	/// @param dispatcher Event dispatcher from which this action map will receive input events.
	void set_event_dispatcher(event::dispatcher* dispatcher);
	
	/// Maps input to an action.
	/// @param action Action to which input will be mapped.
	/// @param mapping Input mapping to add.
	void add_mapping(action& action, const mapping& mapping);

	/// Maps a gamepad axis to an action.
	/// @param action Action to which input will be mapped.
	/// @param mapping Gamepad axis mapping to add.
	void add_gamepad_axis_mapping(action& action, gamepad_axis_mapping mapping);

	/// Maps a gamepad button to an action.
	/// @param action Action to which input will be mapped.
	/// @param mapping Gamepad button mapping to add.
	void add_gamepad_button_mapping(action& action, gamepad_button_mapping mapping);

	/// Maps a key to an action.
	/// @param action Action to which input will be mapped.
	/// @param mapping Key mapping to add.
	void add_key_mapping(action& action, key_mapping mapping);

	/// Maps a mouse button to an action.
	/// @param action Action to which input will be mapped.
	/// @param mapping Mouse button mapping to add.
	void add_mouse_button_mapping(action& action, mouse_button_mapping mapping);

	/// Maps mouse motion to an action.
	/// @param action Action to which input will be mapped.
	/// @param mapping Mouse motion mapping to add.
	void add_mouse_motion_mapping(action& action, mouse_motion_mapping mapping);

	/// Maps mouse scroll to an action.
	/// @param action Action to which input will be mapped.
	/// @param mapping Mouse scroll mapping to add.
	void add_mouse_scroll_mapping(action& action, mouse_scroll_mapping mapping);
	
	/// Unmaps input from an action.
	/// @param action Action from which input will be unmapped.
	/// @param type Type of input mapping to remove.
	void remove_mappings(action& action, mapping_type type);
	
	/// Unmaps all input from an action.
	/// @param action Action from which input will be unmapped.
	void remove_mappings(action& action);
	
	/// Unmaps all input from all actions in the action map.
	void remove_mappings();
	
	/// Returns all of the gamepad axis mappings associated with an action.
	/// @param action Action with which associated mappings will be returned.
	[[nodiscard]] std::vector<gamepad_axis_mapping> get_gamepad_axis_mappings(const action& action) const;
	
	/// Returns all of the gamepad button mappings associated with an action.
	/// @param action Action with which associated mappings will be returned.
	[[nodiscard]] std::vector<gamepad_button_mapping> get_gamepad_button_mappings(const action& action) const;
	
	/// Returns all of the key mappings associated with an action.
	/// @param action Action with which associated mappings will be returned.
	[[nodiscard]] std::vector<key_mapping> get_key_mappings(const action& action) const;
	
	/// Returns all of the mouse button mappings associated with an action.
	/// @param action Action with which associated mappings will be returned.
	[[nodiscard]] std::vector<mouse_button_mapping> get_mouse_button_mappings(const action& action) const;
	
	/// Returns all of the mouse motion mappings associated with an action.
	/// @param action Action with which associated mappings will be returned.
	[[nodiscard]] std::vector<mouse_motion_mapping> get_mouse_motion_mappings(const action& action) const;
	
	/// Returns all of the mouse scroll associated with an action.
	/// @param action Action with which associated mappings will be returned.
	[[nodiscard]] std::vector<mouse_scroll_mapping> get_mouse_scroll_mappings(const action& action) const;
	
	/// Returns `true` if the action map is enabled, `false` otherwise.
	[[nodiscard]] inline constexpr bool is_enabled() const noexcept
	{
		return m_enabled;
	}
	
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
	void handle_update(const update_event& event);
	
	event::dispatcher* m_event_dispatcher{nullptr};
	bool m_enabled{false};
	std::unordered_set<action*> m_actions;
	std::vector<std::shared_ptr<::event::subscription>> m_subscriptions;
	std::vector<std::tuple<action*, gamepad_axis_mapping>> m_gamepad_axis_mappings;
	std::vector<std::tuple<action*, gamepad_button_mapping>> m_gamepad_button_mappings;
	std::vector<std::tuple<action*, key_mapping>> m_key_mappings;
	std::vector<std::tuple<action*, mouse_button_mapping>> m_mouse_button_mappings;
	std::vector<std::tuple<action*, mouse_motion_mapping>> m_mouse_motion_mappings;
	std::vector<std::tuple<action*, mouse_scroll_mapping>> m_mouse_scroll_mappings;
};

} // namespace input

#endif // ANTKEEER_INPUT_ACTION_MAP_HPP
