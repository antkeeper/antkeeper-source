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

#include <engine/input/action.hpp>

namespace input {

namespace {

inline bool default_threshold_function(float x) noexcept
{
	return x > 0.0f;
}

} // namespace

action::action():
	m_threshold_function(default_threshold_function)
{}

void action::evaluate(float value)
{
	// Update input value
	m_active_event.input_value = value;
	
	// Store activation state
	const bool was_active = m_active;
	
	// Re-evaluate activation state
	m_active = m_threshold_function(value);
	
	if (m_active)
	{
		if (!was_active)
		{
			// Publish activated event
			m_activated_publisher.publish(m_activated_event);
		}
	}
	else
	{
		if (was_active)
		{
			// Publish deactivated event
			m_deactivated_publisher.publish(m_deactivated_event);
		}
	}
}

void action::update() const
{
	if (m_active)
	{
		// Publish active event
		m_active_publisher.publish(m_active_event);
	}
}

void action::reset() noexcept
{
	m_active = false;
	m_active_event.input_value = 0.0f;
}

} // namespace input
