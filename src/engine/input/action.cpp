// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
