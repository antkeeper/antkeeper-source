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
