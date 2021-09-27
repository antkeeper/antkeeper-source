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

#include "control.hpp"

namespace input {

control::control():
	deadzone(0.0f),
	current_value(0.0f),
	previous_value(0.0f),
	reset(false),
	activated_callback(nullptr),
	deactivated_callback(nullptr),
	value_changed_callback(nullptr),
	active_callback(nullptr),
	callbacks_enabled(true)
{}

control::~control()
{}

void control::update()
{
	// Perform callbacks, if enabled
	if (callbacks_enabled)
	{
		if (activated_callback)
		{
			if (is_active() && !was_active())
			{
				activated_callback();
			}
		}

		if (deactivated_callback)
		{
			if (!is_active() && was_active())
			{
				deactivated_callback();
			}
		}

		if (value_changed_callback)
		{
			if (current_value != previous_value)
			{
				if (is_active() || was_active())
				{
					value_changed_callback(current_value);
				}
			}
		}
		
		if (active_callback && is_active())
		{
			active_callback(current_value);
		}
	}

	// Update previous value
	previous_value = current_value;

	// Reset temporary values
	if (reset)
	{
		current_value = 0.0f;
		reset = false;
	}
}

void control::set_current_value(float value)
{
	current_value = value;
	reset = false;
}

void control::set_temporary_value(float value)
{
	current_value = value;
	reset = true;
}

void control::set_deadzone(float value)
{
	deadzone = value;
}

void control::set_activated_callback(std::function<void()> callback)
{
	this->activated_callback = callback;
}

void control::set_deactivated_callback(std::function<void()> callback)
{
	this->deactivated_callback = callback;
}

void control::set_value_changed_callback(std::function<void(float)> callback)
{
	this->value_changed_callback = callback;
}

void control::set_active_callback(std::function<void(float)> callback)
{
	this->active_callback = callback;
}

} // namespace input
