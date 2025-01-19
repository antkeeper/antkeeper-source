// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_DEVICE_EVENTS_HPP
#define ANTKEEPER_INPUT_DEVICE_EVENTS_HPP

namespace input {

class device;

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

} // namespace input

#endif // ANTKEEPER_INPUT_DEVICE_EVENTS_HPP
