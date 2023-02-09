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

#ifndef ANTKEEPER_INPUT_DEVICE_HPP
#define ANTKEEPER_INPUT_DEVICE_HPP

#include "input/device-type.hpp"
#include "input/device-events.hpp"
#include "event/publisher.hpp"
#include "utility/uuid.hpp"
#include <optional>
#include <string>

namespace input {

/**
 * Abstract base class for virtual devices that generate input events.
 */
class device
{
public:
	/// Constructs an input device.
	device();
	
	/// Destructs an input device.
	virtual ~device() = default;
	
	/**
	 * Simulates the device being connected.
	 */
	void connect();
	
	/**
	 * Simulates the device being disconnected.
	 *
	 * @note Disconnected devices can still generate input events.
	 */
	void disconnect();
	
	/// Returns `true` if the device is currently connected.
	[[nodiscard]] inline bool is_connected() const noexcept
	{
		return connected;
	}
	
	/**
	 * Sets the universally unique identifier (UUID) of this input device.
	 *
	 * @param id UUID.
	 */
	void set_uuid(const ::uuid& id);
	
	/// Returns the universally unique identifier (UUID) of this input device.
	[[nodiscard]] inline const ::uuid& get_uuid() const noexcept
	{
		return uuid;
	}
	
	/// Returns the channel through which device connected events are published.
	[[nodiscard]] inline ::event::channel<device_connected_event>& get_connected_channel() noexcept
	{
		return connected_publisher.channel();
	}
	
	/// Returns the channel through which device disconnected events are published.
	[[nodiscard]] inline ::event::channel<device_disconnected_event>& get_disconnected_channel() noexcept
	{
		return disconnected_publisher.channel();
	}
	
	/// Returns the input device type.
	[[nodiscard]] virtual constexpr device_type get_device_type() const noexcept = 0;
	
private:
	::uuid uuid;
	bool connected;
	
	::event::publisher<device_connected_event> connected_publisher;
	::event::publisher<device_disconnected_event> disconnected_publisher;
};

} // namespace input

#endif // ANTKEEPER_INPUT_DEVICE_HPP
