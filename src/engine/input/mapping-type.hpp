// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_MAPPING_TYPE_HPP
#define ANTKEEPER_INPUT_MAPPING_TYPE_HPP

#include <cstdint>

namespace input {

/// Input mapping types.
/// @see input::mapping
enum class mapping_type: std::uint8_t
{
	/// Gamepad axis mapping.
	gamepad_axis,
	
	/// Gamepad button mapping.
	gamepad_button,
	
	/// Key mapping.
	key,
	
	/// Mouse button mapping.
	mouse_button,
	
	/// Mouse motion mapping.
	mouse_motion,
	
	/// Mouse scroll mapping.
	mouse_scroll
};

} // namespace input

#endif // ANTKEEPER_INPUT_MAPPING_TYPE_HPP
