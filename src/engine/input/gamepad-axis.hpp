// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_GAMEPAD_AXIS_HPP
#define ANTKEEPER_INPUT_GAMEPAD_AXIS_HPP

#include <cstdint>

namespace input {

/// Gamepad axes.
enum class gamepad_axis: std::uint8_t
{
	/// Left stick X-axis.
	left_stick_x,
	
	/// Left stick Y-axis.
	left_stick_y,
	
	/// Right stick X-axis.
	right_stick_x,
	
	/// Right stick Y-axis.
	right_stick_y,
	
	/// Left trigger.
	left_trigger,
	
	/// Right trigger.
	right_trigger
};

} // namespace input

#endif // ANTKEEPER_INPUT_GAMEPAD_AXIS_HPP
