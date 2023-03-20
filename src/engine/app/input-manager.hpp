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

#ifndef ANTKEEPER_APP_INPUT_MANAGER_HPP
#define ANTKEEPER_APP_INPUT_MANAGER_HPP

#include <engine/input/device.hpp>
#include <engine/input/gamepad.hpp>
#include <engine/input/keyboard.hpp>
#include <engine/input/mouse.hpp>
#include <engine/event/dispatcher.hpp>
#include <map>
#include <memory>
#include <unordered_set>

namespace app {

/**
 * Manages virtual input devices.
 */
class input_manager
{
public:
	/**
	 * Allocates and returns an input manager.
	 */
	static std::unique_ptr<input_manager> instance();

	/// Destructs an input manager.
	virtual ~input_manager() = default;
	
	/**
	 * Processes input events.
	 */
	virtual void update() = 0;
	
	/**
	 * Shows or hides the cursor.
	 *
	 * @param visible `true` to show the cursor, `false` to hide it.
	 */
	virtual void set_cursor_visible(bool visible) = 0;
	
	/**
	 * Enables or disables relative mouse mode.
	 *
	 * @param enabled `true` to enable relative mouse mode, `false` to disable it.
	 */
	virtual void set_relative_mouse_mode(bool enabled) = 0;
	
	/**
	 * Returns the event dispatcher associated with registered input devices.
	 */
	/// @{
	[[nodiscard]] inline const ::event::dispatcher& get_event_dispatcher() const noexcept
	{
		return m_event_dispatcher;
	}
	[[nodiscard]] inline ::event::dispatcher& get_event_dispatcher() noexcept
	{
		return m_event_dispatcher;
	}
	/// @}
	
	/// Returns the set of registered gamepads.
	[[nodiscard]] inline const std::unordered_set<input::gamepad*>& get_gamepads() noexcept
	{
		return m_gamepads;
	}
	
	/// Returns the set of registered keyboards.
	[[nodiscard]] inline const std::unordered_set<input::keyboard*>& get_keyboards() noexcept
	{
		return m_keyboards;
	}
	
	/// Returns the set of registered mice.
	[[nodiscard]] inline const std::unordered_set<input::mouse*>& get_mice() noexcept
	{
		return m_mice;
	}
	
protected:
	/**
	 * Registers an input device.
	 *
	 * @param device Input device to register.
	 */
	/// @{
	void register_device(input::device& device);
	void register_gamepad(input::gamepad& device);
	void register_keyboard(input::keyboard& device);
	void register_mouse(input::mouse& device);
	/// @}
	
	/**
	 * Unregisters an input device.
	 *
	 * @param device Input device to unregister.
	 */
	/// @{
	void unregister_device(input::device& device);
	void unregister_gamepad(input::gamepad& device);
	void unregister_keyboard(input::keyboard& device);
	void unregister_mouse(input::mouse& device);
	/// @}
	
	::event::dispatcher m_event_dispatcher;
	
private:
	std::multimap<input::device*, std::shared_ptr<::event::subscription>> m_subscriptions;
	std::unordered_set<input::gamepad*> m_gamepads;
	std::unordered_set<input::keyboard*> m_keyboards;
	std::unordered_set<input::mouse*> m_mice;
};

} // namespace app

#endif // ANTKEEPER_APP_INPUT_MANAGER_HPP
