// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_DEVICE_HPP
#define ANTKEEPER_INPUT_DEVICE_HPP

#include <engine/input/device-type.hpp>
#include <engine/input/device-events.hpp>
#include <engine/event/publisher.hpp>
#include <engine/utility/uuid.hpp>
#include <optional>
#include <string>

namespace input {

/**
 * Abstract base class for virtual devices that generate input events.
 */
class device
{
public:
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
	bool connected{false};
	
	::event::publisher<device_connected_event> connected_publisher;
	::event::publisher<device_disconnected_event> disconnected_publisher;
};

} // namespace input

#endif // ANTKEEPER_INPUT_DEVICE_HPP
