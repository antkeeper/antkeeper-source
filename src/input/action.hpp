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

#ifndef ANTKEEPER_INPUT_ACTION_HPP
#define ANTKEEPER_INPUT_ACTION_HPP

#include "input/action-events.hpp"
#include "event/publisher.hpp"
#include <functional>

namespace input {

/**
 * Evaluates an activation state given input values and publishes events on activation state changes.
 */
class action
{
public:
	/**
	 * Threshold function type.
	 *
	 * Given an input value, returns `true` if the action should be considered active, and `false` otherwise.
	 */
	typedef std::function<bool(float)> threshold_function_type;
	
	/// Constructs a action.
	action();
	
	/**
	 * Sets the threshold function.
	 *
	 * @param function Threshold function.
	 */
	void set_threshold_function(const threshold_function_type& function);
	
	/**
	 * Evaluates the activation state of the action, according to its threshold function and an input value.
	 *
	 * @param value Input value.
	 */
	void evaluate(float value);
	
	/**
	 * Resets the activation state of the action without publishing any events.
	 */
	void reset();
	
	/// Returns the threshold function.
	[[nodiscard]] inline const threshold_function_type& get_threshold_function() const noexcept
	{
		return threshold_function;
	}
	
	/// Returns `true` if the action is active, `false` otherwise.
	[[nodiscard]] inline bool is_active() const noexcept
	{
		return active;
	}
	
	/// Returns the channel through which action activated events are published.
	[[nodiscard]] inline ::event::channel<action_activated_event>& get_activated_channel() noexcept
	{
		return activated_publisher.channel();
	}
	
	/// Returns the channel through which action active events are published.
	[[nodiscard]] inline ::event::channel<action_active_event>& get_active_channel() noexcept
	{
		return active_publisher.channel();
	}
	
	/// Returns the channel through which action deactivated events are published.
	[[nodiscard]] inline ::event::channel<action_deactivated_event>& get_deactivated_channel() noexcept
	{
		return deactivated_publisher.channel();
	}

private:
	threshold_function_type threshold_function;
	bool active;
	
	action_activated_event activated_event;
	action_active_event active_event;
	action_deactivated_event deactivated_event;
	
	::event::publisher<action_activated_event> activated_publisher;
	::event::publisher<action_active_event> active_publisher;
	::event::publisher<action_deactivated_event> deactivated_publisher;
};

} // namespace input

#endif // ANTKEEPER_INPUT_ACTION_HPP
