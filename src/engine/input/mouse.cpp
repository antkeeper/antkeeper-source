// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.input.mouse;
import engine.event.publisher;

namespace engine::input
{
	void mouse::press(mouse_button button)
	{
		m_button_pressed_publisher.publish({this, m_position, button});
	}

	void mouse::release(mouse_button button)
	{
		m_button_released_publisher.publish({this, m_position, button});
	}

	void mouse::move(const math::fvec2& position, const math::fvec2& difference)
	{
		m_position = position;
		m_moved_publisher.publish({this, m_position, difference});
	}

	void mouse::scroll(const math::fvec2& velocity)
	{
		m_scrolled_publisher.publish({this, m_position, velocity});
	}
}
