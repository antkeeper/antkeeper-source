// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_MOUSE_SCROLL_AXIS_HPP
#define ANTKEEPER_INPUT_MOUSE_SCROLL_AXIS_HPP

#include <cstdint>

namespace input {

/// Mouse scroll axes.
enum class mouse_scroll_axis: std::uint8_t
{
	/// X-axis.
	x = 0,
	
	/// Y-axis.
	y = 1
};

} // namespace input

#endif // ANTKEEPER_INPUT_MOUSE_SCROLL_AXIS_HPP
