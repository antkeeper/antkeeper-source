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

#ifndef ANTKEEPER_CONTROL_HPP
#define ANTKEEPER_CONTROL_HPP

#include <functional>

/**
 * A control can be bound to multiple types of input events.
 *
 * @ingroup input
 */
class control
{
public:
	/// Creates a control.
	control();

	/// Destroys a control.
	virtual ~control();

	/**
	 * Performs callbacks then sets the previous value equal to the current value.
	 */
	void update();

	/**
	 * Sets the current value of the control.
	 *
	 * @param value control value.
	 */
	void set_current_value(float value);

	/**
	 * This works the same as setting the current value, but causes the value to be reset on the next call to update.
	 */
	void set_temporary_value(float value);

	/**
	 * Sets the deadzone value. If the current value of the control is not greater than the deadzone value, the control will not be considered active.
	 *
	 * @param value Deadzone value.
	 */
	void set_deadzone(float value);

	/// Sets the callback for when the control is activated.
	void set_activated_callback(std::function<void()> callback);

	/// Sets the callback for when the control is deactivated.
	void set_deactivated_callback(std::function<void()> callback);

	/// Sets the callback for when the control value is changed.
	void set_value_changed_callback(std::function<void(float)> callback);

	/**
	 * Enables or disables callbacks.
	 *
	 * @param enabled Whether to enable or disable callbacks.
	 */
	void set_callbacks_enabled(bool enabled);

	/// Returns the deadzone value. The default value is 0.0.
	float get_deadzone() const;

	/// Returns the current value of the control.
	float get_current_value() const;

	/// Returns the previous value of the control.
	float get_previous_value() const;

	/// Returns true if the control is currently active.
	bool is_active() const;

	/// Returns true if the control was previously active when update() was last called.
	bool was_active() const;

private:
	float deadzone;
	float current_value;
	float previous_value;
	bool reset;
	std::function<void()> activated_callback;
	std::function<void()> deactivated_callback;
	std::function<void(float)> value_changed_callback;
	bool callbacks_enabled;
};

inline void control::set_callbacks_enabled(bool enabled)
{
	this->callbacks_enabled = enabled;
}

inline float control::get_deadzone() const
{
	return deadzone;
}

inline float control::get_current_value() const
{
	return current_value;
}

inline float control::get_previous_value() const
{
	return previous_value;
}

inline bool control::is_active() const
{
	return current_value > deadzone;
}

inline bool control::was_active() const
{
	return previous_value > deadzone;
}

#endif // ANTKEEPER_CONTROL_HPP

