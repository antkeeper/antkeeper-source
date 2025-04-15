// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.input.device;
import engine.event.publisher;

namespace engine::input
{
	void device::connect()
	{
		m_connected = true;
		m_connected_publisher.publish({this});
	}

	void device::disconnect()
	{
		m_connected = false;
		m_disconnected_publisher.publish({this});
	}

	void device::set_uuid(const uuid& id)
	{
		m_uuid = id;
	}
}
