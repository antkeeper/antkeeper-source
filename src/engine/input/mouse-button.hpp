// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_MOUSE_BUTTON_HPP
#define ANTKEEPER_INPUT_MOUSE_BUTTON_HPP

#include <cstdint>

namespace input {

/// Mouse buttons.
enum class mouse_button: std::uint8_t
{
	/// Left mouse button.
	left = 1,
	
	/// Middle mouse button.
	middle = 2,
	
	/// Right mouse button.
	right = 3
};

} // namespace input

#endif // ANTKEEPER_INPUT_MOUSE_BUTTON_HPP
