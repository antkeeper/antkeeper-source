/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

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
