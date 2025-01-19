// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_MAPPING_EVENTS_HPP
#define ANTKEEPER_INPUT_MAPPING_EVENTS_HPP

#include <engine/input/mapping.hpp>

namespace input {

/// Event generated when a gamepad axis mapping has been generated.
struct gamepad_axis_mapped_event
{
	/// Gamepad axis mapping that was generated.
	gamepad_axis_mapping mapping;
};

/// Event generated when a gamepad button mapping has been generated.
struct gamepad_button_mapped_event
{
	/// Gamepad button mapping that was generated.
	gamepad_button_mapping mapping;
};

/// Event generated when a key mapping has been generated.
struct key_mapped_event
{
	/// Key mapping that was generated.
	key_mapping mapping;
};

/// Event generated when a mouse button mapping has been generated.
struct mouse_button_mapped_event
{
	/// Mouse button mapping that was generated.
	mouse_button_mapping mapping;
};

/// Event generated when a mouse motion mapping has been generated.
struct mouse_motion_mapped_event
{
	/// Mouse motion mapping that was generated.
	mouse_motion_mapping mapping;
};

/// Event generated when a mouse scroll mapping has been generated.
struct mouse_scroll_mapped_event
{
	/// Mouse scroll mapping that was generated.
	mouse_scroll_mapping mapping;
};

} // namespace input

#endif // ANTKEEPER_INPUT_MAPPING_EVENTS_HPP
