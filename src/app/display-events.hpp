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

#ifndef ANTKEEPER_APP_DISPLAY_EVENTS_HPP
#define ANTKEEPER_APP_DISPLAY_EVENTS_HPP

#include "app/display-orientation.hpp"

namespace app {

class display;

/**
 * Event generated when a display has been connected.
 */
struct display_connected_event
{
	/// Pointer to the display that has been connected.
	const display* display;
};

/**
 * Event generated when a display has been disconnected.
 */
struct display_disconnected_event
{
	/// Pointer to the display that has been disconnected.
	const display* display;
};

/**
 * Event generated when the orientation of a display has changed.
 */
struct display_orientation_changed_event
{
	/// Pointer to the display that has had it's orientation changed.
	const display* display;
	
	/// Orientation of the display.
	display_orientation orientation;
};

} // namespace app

#endif // ANTKEEPER_APP_DISPLAY_EVENTS_HPP
