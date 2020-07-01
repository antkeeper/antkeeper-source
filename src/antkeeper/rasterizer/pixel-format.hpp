/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_PIXEL_FORMAT_HPP
#define ANTKEEPER_PIXEL_FORMAT_HPP

enum class pixel_format
{
	d, ///< Depth
	ds, ///< Depth, stencil
	r, ///< Red
	rg, ///< Red, green
	rgb, ///< Red, green, blue
	bgr, ///< Blue, green, red
	rgba, ///< Red, green, blue, alpha
	bgra ///< Blue, green, red, alpha
};

#endif // ANTKEEPER_PIXEL_FORMAT_HPP

