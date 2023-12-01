// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/input/device.hpp>

namespace input {

void device::connect()
{
	connected = true;
	connected_publisher.publish({this});
}

void device::disconnect()
{
	connected = false;
	disconnected_publisher.publish({this});
}

void device::set_uuid(const ::uuid& id)
{
	uuid = id;
}

} // namespace input
