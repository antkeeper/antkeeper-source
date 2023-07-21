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

#ifndef ANTKEEPER_INPUT_MOUSE_EVENTS_HPP
#define ANTKEEPER_INPUT_MOUSE_EVENTS_HPP

#include <engine/input/mouse-button.hpp>
#include <engine/input/mouse-motion-axis.hpp>
#include <engine/input/mouse-scroll-axis.hpp>
#include <engine/math/vector.hpp>
#include <cstdint>

namespace input {

class mouse;

/**
 * Event generated when a mouse has been moved.
 */
struct mouse_moved_event
{
	/// Mouse that generated the event.
	mouse* mouse{nullptr};
	
	/// Mouse position, in pixels, relative to the window.
	math::vec2<std::int32_t> position{0, 0};
	
	/// Relative movement of the mouse, in pixels.
	math::vec2<std::int32_t> difference{0, 0};
};

/**
 * Event generated when a mouse button has been pressed.
 */
struct mouse_button_pressed_event
{
	/// Mouse that generated the event.
	mouse* mouse{nullptr};
	
	/// Mouse position, in pixels, relative to the window, when the button was pressed.
	math::vec2<std::int32_t> position{0, 0};
	
	/// Mouse button being pressed.
	mouse_button button{0};
};

/**
 * Event generated when a mouse button has been released.
 */
struct mouse_button_released_event
{
	/// Mouse that generated the event.
	mouse* mouse{nullptr};
	
	/// Mouse position, in pixels, relative to the window, when the button was released.
	math::vec2<std::int32_t> position{0, 0};
	
	/// Mouse button being released.
	mouse_button button{0};
};

/**
 * Event generated when a mouse has been scrolled.
 */
struct mouse_scrolled_event
{
	/// Mouse that generated the event.
	mouse* mouse{nullptr};
	
	/// Mouse position, in pixels, relative to the window, when the mouse was scrolled.
	math::vec2<std::int32_t> position{0, 0};
	
	/// Scroll velocity.
	math::fvec2 velocity{0.0f, 0.0f};
};

} // namespace input

#endif // ANTKEEPER_INPUT_MOUSE_EVENTS_HPP
