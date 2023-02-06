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

#ifndef ANTKEEPER_INPUT_CONTROL_HPP
#define ANTKEEPER_INPUT_CONTROL_HPP

#include "input/input-events.hpp"
#include "event/publisher.hpp"
#include <functional>

namespace input {

/**
 * Generates control-related input events on activation state changes.
 */
class control
{
public:
	/**
	 * Threshold function type.
	 *
	 * Given an input value, returns `true` if the control should be considered active, and `false` otherwise.
	 */
	typedef std::function<bool(float)> threshold_function_type;
	
	/// Constructs a control.
	control();
	
	/**
	 * Sets the threshold function.
	 *
	 * @param function Threshold function.
	 */
	void set_threshold_function(const threshold_function_type& function);
	
	/**
	 * Evaluates the activation state of the control, according to its threshold function and an input value.
	 *
	 * @param value Input value.
	 */
	void evaluate(float value);
	
	/// Returns the threshold function.
	[[nodiscard]] inline const threshold_function_type& get_threshold_function() const noexcept
	{
		return threshold_function;
	}
	
	/// Returns `true` if the control is active, `false` otherwise.
	[[nodiscard]] inline bool is_active() const noexcept
	{
		return active;
	}
	
	/// Returns the channel through which control activated events are published.
	[[nodiscard]] inline ::event::channel<control_activated_event>& get_activated_channel() noexcept
	{
		return activated_publisher.channel();
	}
	
	/// Returns the channel through which control active events are published.
	[[nodiscard]] inline ::event::channel<control_active_event>& get_active_channel() noexcept
	{
		return active_publisher.channel();
	}
	
	/// Returns the channel through which control deactivated events are published.
	[[nodiscard]] inline ::event::channel<control_deactivated_event>& get_deactivated_channel() noexcept
	{
		return deactivated_publisher.channel();
	}

private:
	threshold_function_type threshold_function;
	bool active;
	
	control_activated_event activated_event;
	control_active_event active_event;
	control_deactivated_event deactivated_event;
	
	::event::publisher<control_activated_event> activated_publisher;
	::event::publisher<control_active_event> active_publisher;
	::event::publisher<control_deactivated_event> deactivated_publisher;
};

} // namespace input

#endif // ANTKEEPER_INPUT_CONTROL_HPP
