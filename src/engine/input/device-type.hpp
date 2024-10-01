// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_DEVICE_TYPE_HPP
#define ANTKEEPER_INPUT_DEVICE_TYPE_HPP

#include <cstdint>

namespace input {

/// Input device types.
enum class device_type: std::uint8_t
{
	/// Gamepad input device.
	gamepad,
	
	/// Keyboard input device.
	keyboard,
	
	/// Mouse input device.
	mouse
};

} // namespace input

#endif // ANTKEEPER_INPUT_DEVICE_TYPE_HPP
