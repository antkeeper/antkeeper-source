// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>

/// Modifier key bit mask flags.
namespace engine::input::modifier_key
{
	/// Modifier key bit flags.
	enum: u16
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
}
