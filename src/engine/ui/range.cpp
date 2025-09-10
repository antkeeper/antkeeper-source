// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/ui/range.hpp>
#include <engine/math/functions.hpp>

namespace engine::ui
{
	void range::set_value(float value)
	{
		const float clamped_value = math::clamp(value, m_min_value, m_max_value);

		if (m_value != clamped_value)
		{
			if (m_value_changed_callback)
			{
				if (m_value_changed_callback({this, clamped_value}))
				{
					m_value = clamped_value;
					refresh_text();
				}
			}
			else
			{
				m_value = clamped_value;
				refresh_text();
			}
		}
	}

	void range::set_min_value(float value)
	{
		m_min_value = value;

		if (m_value < m_min_value)
		{
			set_value(m_min_value);
		}
	}

	void range::set_max_value(float value)
	{
		m_max_value = value;

		if (m_value > m_max_value)
		{
			set_value(m_max_value);
		}
	}

	void range::set_step(float step)
	{
		m_step = step;
	}

	void range::increment()
	{
		set_value(m_value + m_step);
	}

	void range::decrement()
	{
		set_value(m_value - m_step);
	}

	bool range::is_boolean() const noexcept
	{
		return m_min_value == 0.0f && m_max_value == 1.0f && m_step == 1.0f;
	}

	void range::set_value_changed_callback(std::function<bool(const range_value_changed_event&)> callback)
	{
		m_value_changed_callback = std::move(callback);
	}
}
