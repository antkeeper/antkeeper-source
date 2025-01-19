// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_MOUSE_MOTION_AXIS_HPP
#define ANTKEEPER_INPUT_MOUSE_MOTION_AXIS_HPP

#include <cstdint>

namespace input {

/// Mouse motion axes.
enum class mouse_motion_axis: std::uint8_t
{
	/// X-axis.
	x = 0,
	
	/// Y-axis.
	y = 1
};

} // namespace input

#endif // ANTKEEPER_INPUT_MOUSE_MOTION_AXIS_HPP
