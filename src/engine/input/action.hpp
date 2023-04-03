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

#include <engine/input/action-events.hpp>
#include <engine/event/publisher.hpp>
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
	using threshold_function_type = std::function<bool(float)>;
	
	/// Constructs an action.
	action();
	
	/**
	 * Sets the threshold function.
	 *
	 * @param function Threshold function.
	 */
	inline void set_threshold_function(const threshold_function_type& function) noexcept
	{
		m_threshold_function = function;
	}
	
	/**
	 * Evaluates the activation state of the action, according to its threshold function and an input value.
	 *
	 * @param value Input value.
	 */
	void evaluate(float value);
	
	/**
	 * Publishes an action active event if the action is active.
	 */
	void update() const;
	
	/**
	 * Resets the activation state of the action without publishing any events.
	 */
	void reset() noexcept;
	
	/// Returns the threshold function.
	[[nodiscard]] inline const threshold_function_type& get_threshold_function() const noexcept
	{
		return m_threshold_function;
	}
	
	/// Returns `true` if the action is active, `false` otherwise.
	[[nodiscard]] inline bool is_active() const noexcept
	{
		return m_active;
	}
	
	/// Returns the msot recently evaluated input value.
	[[nodiscard]] inline float get_input_value() const noexcept
	{
		return m_active_event.input_value;
	}
	
	/// Returns the channel through which action activated events are published.
	[[nodiscard]] inline ::event::channel<action_activated_event>& get_activated_channel() noexcept
	{
		return m_activated_publisher.channel();
	}
	
	/// Returns the channel through which action active events are published.
	[[nodiscard]] inline ::event::channel<action_active_event>& get_active_channel() noexcept
	{
		return m_active_publisher.channel();
	}
	
	/// Returns the channel through which action deactivated events are published.
	[[nodiscard]] inline ::event::channel<action_deactivated_event>& get_deactivated_channel() noexcept
	{
		return m_deactivated_publisher.channel();
	}

private:
	threshold_function_type m_threshold_function;
	bool m_active{false};
	
	action_activated_event m_activated_event{this};
	action_active_event m_active_event{this, 0.0f};
	action_deactivated_event m_deactivated_event{this};
	
	::event::publisher<action_activated_event> m_activated_publisher;
	::event::publisher<action_active_event> m_active_publisher;
	::event::publisher<action_deactivated_event> m_deactivated_publisher;
};

} // namespace input

#endif // ANTKEEPER_INPUT_ACTION_HPP
