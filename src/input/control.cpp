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

#include "input/control.hpp"

namespace input {

static bool default_threshold_function(float x) noexcept
{
	return x > 0.0f;
}

control::control():
	threshold_function(default_threshold_function),
	active(false),
	activated_event{this},
	active_event{this, 0.0f},
	deactivated_event{this}
{}

void control::set_threshold_function(const threshold_function_type& function)
{
	threshold_function = function;
}

void control::evaluate(float value)
{
	// Store activation state
	const bool was_active = active;
	
	// Re-evaluate activation state
	active = threshold_function(value);
	
	// Emit events
	if (active)
	{
		if (!was_active)
		{
			activated_publisher.publish(activated_event);
		}
		
		active_event.input_value = value;
		active_publisher.publish(active_event);
	}
	else
	{
		if (was_active)
		{
			deactivated_publisher.publish(deactivated_event);
		}
	}
}

void control::reset()
{
	active = false;
}

} // namespace input
