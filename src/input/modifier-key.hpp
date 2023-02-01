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

#ifndef ANTKEEPER_INPUT_MODIFIER_KEY_HPP
#define ANTKEEPER_INPUT_MODIFIER_KEY_HPP

#include <cstdint>

namespace input {

/**
 * Modifier key bit mask flags.
 */
namespace modifier_key {

/// Modifier key bit flags.
enum: std::uint16_t
{
	/// No modifier key is pressed.
	none        = 0b0000000000000000,
	
	/// Left shift modifier key is pressed.
	left_shift  = 0b0000000000000001,
	
	/// Right shift modifier key is pressed.
	right_shift = 0b0000000000000010,
	
	/// One or both shift modifier keys are pressed.
	shift       = left_shift | right_shift,
	
	/// Left ctrl modifier key is pressed.
	left_ctrl   = 0b0000000000000100,
	
	/// Right ctrl modifier key is pressed.
	right_ctrl  = 0b0000000000001000,
	
	/// One or both ctrl modifier keys are pressed.
	ctrl        = left_ctrl | right_ctrl,
	
	/// Left alt modifier key is pressed.
	left_alt    = 0b0000000000010000,
	
	/// Right alt modifier key is pressed.
	right_alt   = 0b0000000000100000,
	
	/// One or both alt modifier keys are pressed.
	alt         = left_alt | right_alt,
	
	/// Left gui modifier key is pressed.
	left_gui    = 0b0000000001000000,
	
	/// Right gui modifier key is pressed.
	right_gui   = 0b0000000010000000,
	
	/// One or both gui modifier keys are pressed.
	gui         = left_gui | right_gui,
	
	/// Num lock modifier key is pressed.
	num_lock    = 0b0000000100000000,
	
	/// Caps lock modifier key is pressed.
	caps_lock   = 0b0000001000000000,
	
	/// Scroll lock modifier key is pressed.
	scroll_lock = 0b0000010000000000,
	
	/// AltGr modifier key is pressed.
	alt_gr      = 0b0000100000000000
};

} // namespace modifier_key
} // namespace input

#endif // ANTKEEPER_INPUT_MODIFIER_KEY_HPP
