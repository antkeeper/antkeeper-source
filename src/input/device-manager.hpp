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

#ifndef ANTKEEPER_INPUT_DEVICE_MANAGER_HPP
#define ANTKEEPER_INPUT_DEVICE_MANAGER_HPP

#include "input/device.hpp"
#include "input/gamepad.hpp"
#include "input/keyboard.hpp"
#include "input/mouse.hpp"
#include "event/queue.hpp"
#include <map>
#include <memory>
#include <unordered_set>

namespace input {

/**
 * Manages virtual input devices.
 */
class device_manager
{
public:
	/**
	 * Registers an input device.
	 *
	 * @param device Input device to register.
	 */
	void register_device(device& device);
	
	/**
	 * Unregisters an input device.
	 *
	 * @param device Input device to unregister.
	 */
	void unregister_device(device& device);
	
	/**
	 * Returns the event queue associated with registered input devices.
	 */
	[[nodiscard]] inline ::event::queue& get_event_queue() noexcept
	{
		return event_queue;
	}
	
	/// Returns the set of registered gamepads.
	[[nodiscard]] inline const std::unordered_set<gamepad*>& get_gamepads() noexcept
	{
		return gamepads;
	}
	
	/// Returns the set of registered keyboards.
	[[nodiscard]] inline const std::unordered_set<keyboard*>& get_keyboards() noexcept
	{
		return keyboards;
	}
	
	/// Returns the set of registered mice.
	[[nodiscard]] inline const std::unordered_set<mouse*>& get_mice() noexcept
	{
		return mice;
	}
	
private:
	void register_gamepad(gamepad& gamepad);
	void register_keyboard(keyboard& keyboard);
	void register_mouse(mouse& mouse);
	void unregister_gamepad(gamepad& gamepad);
	void unregister_keyboard(keyboard& keyboard);
	void unregister_mouse(mouse& mouse);
	
	::event::queue event_queue;
	std::multimap<device*, std::shared_ptr<::event::subscription>> subscriptions;
	std::unordered_set<gamepad*> gamepads;
	std::unordered_set<keyboard*> keyboards;
	std::unordered_set<mouse*> mice;
};

} // namespace input

#endif // ANTKEEPER_INPUT_DEVICE_MANAGER_HPP
