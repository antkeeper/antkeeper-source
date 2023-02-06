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

#ifndef ANTKEEPER_APP_WINDOW_EVENTS_HPP
#define ANTKEEPER_APP_WINDOW_EVENTS_HPP

#include "math/vector.hpp"

namespace app {

class window;

/**
 * Event generated when a window has been requested to close.
 */
struct window_closed_event
{
	/// Pointer to the window that has been requested to close.
	window* window;
};

/**
 * Event generated when a window has gained or lost focus.
 */
struct window_focus_changed_event
{
	/// Pointer to the window that has gained or lost focus.
	window* window;
	
	/// `true` if the window is in focus, `false` otherwise.
	bool in_focus;
};

/**
 * Event generated when a window has been moved.
 */
struct window_moved_event
{
	/// Pointer to the window that has been moved.
	window* window;
	
	/// Position of the window, in display units.
	math::vector<int, 2> position;
};

/**
 * Event generated when a window has been maximized.
 */
struct window_maximized_event
{
	/// Pointer to the window that has been maximized.
	window* window;
};

/**
 * Event generated when a window has been minimized.
 */
struct window_minimized_event
{
	/// Pointer to the window that has been minimized.
	window* window;
};

/**
 * Event generated when a window has been restored.
 */
struct window_restored_event
{
	/// Pointer to the window that has been restored.
	window* window;
};

/**
 * Event generated when a window has been resized.
 */
struct window_resized_event
{
	/// Pointer to the window that has been resized.
	window* window;
	
	/// Window size, in display units.
	math::vector<int, 2> size;
};

} // namespace app

#endif // ANTKEEPER_APP_WINDOW_EVENTS_HPP
