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

#ifndef ANTKEEPER_APP_DISPLAY_ORIENTATION_HPP
#define ANTKEEPER_APP_DISPLAY_ORIENTATION_HPP

#include <cstdint>

namespace app {

/**
 * Display orientations.
 */
enum class display_orientation: std::uint8_t
{
	/// Display orientation unknown.
	unknown,
	
	/// Display is in landscape mode, with the right side up, relative to portrait mode.
	landscape,
	
	/// Display is in landscape mode, with the left side up, relative to portrait mode.
	landscape_flipped,
	
	/// Display is in portrait mode.
	portrait,
	
	/// Display is in portrait mode, upside down.
	portrait_flipped
};

} // namespace app

#endif // ANTKEEPER_APP_DISPLAY_ORIENTATION_HPP
