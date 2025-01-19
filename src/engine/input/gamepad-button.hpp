// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_GAMEPAD_BUTTON_HPP
#define ANTKEEPER_INPUT_GAMEPAD_BUTTON_HPP

#include <cstdint>

namespace input {

/// Gamepad buttons.
enum class gamepad_button: std::uint8_t
{
	/// A button.
	a,
	
	/// B button.
	b,
	
	/// X button.
	x,
	
	/// Y button.
	y,
	
	/// Back button.
	back,
	
	/// Guide button.
	guide,
	
	/// Start button.
	start,
	
	/// Left stick button.
	left_stick,
	
	/// Right stick button.
	right_stick,
	
	/// Left shoulder button.
	left_shoulder,
	
	/// Right shoulder button.
	right_shoulder,
	
	/// D-pad up button.
	dpad_up,
	
	/// D-pad down button.
	dpad_down,
	
	/// D-pad left button.
	dpad_left,
	
	/// D-pad right button.
	dpad_right
};

} // namespace input

#endif // ANTKEEPER_INPUT_GAMEPAD_BUTTON_HPP
