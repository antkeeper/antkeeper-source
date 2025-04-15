// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.input.device;
export import engine.utility.uuid;
import engine.event.publisher;
export import engine.event.channel;

export namespace engine::input
{
	class device;

	/// Input device types.
	enum class device_type
	{
		/// Gamepad input device.
		gamepad,

		/// Keyboard input device.
		keyboard,

		/// Mouse input device.
		mouse
	};

	/// Event generated when an input device has been connected.
	struct device_connected_event
	{
		/// Device that was connected.
		device* device{nullptr};
	};

	/// Event generated when an input device has been disconnected.
	struct device_disconnected_event
	{
		/// Device that was disconnected.
		device* device{nullptr};
	};

	/// Abstract base class for virtual devices that generate input events.
	class device
	{
	public:
		/// Destructs an input device.
		virtual ~device() = default;
	
		/// Simulates the device being connected.
		void connect();
	
		/// Simulates the device being disconnected.
		/// @note Disconnected devices can still generate input events.
		void disconnect();
	
		/// Returns `true` if the device is currently connected.
		[[nodiscard]] inline bool is_connected() const noexcept
		{
			return m_connected;
		}
	
		/// Sets the universally unique identifier (UUID) of this input device.
		/// @param id UUID.
		void set_uuid(const uuid& id);
	
		/// Returns the universally unique identifier (UUID) of this input device.
		[[nodiscard]] inline const auto& get_uuid() const noexcept
		{
			return m_uuid;
		}
	
		/// Returns the channel through which device connected events are published.
		[[nodiscard]] inline auto& get_connected_channel() noexcept
		{
			return m_connected_publisher.channel();
		}
	
		/// Returns the channel through which device disconnected events are published.
		[[nodiscard]] inline auto& get_disconnected_channel() noexcept
		{
			return m_disconnected_publisher.channel();
		}
	
		/// Returns the input device type.
		[[nodiscard]] virtual constexpr device_type get_device_type() const noexcept = 0;
	
	private:
		uuid m_uuid;
		bool m_connected{false};
	
		event::publisher<device_connected_event> m_connected_publisher;
		event::publisher<device_disconnected_event> m_disconnected_publisher;
	};
}
