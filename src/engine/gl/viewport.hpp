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

#ifndef ANTKEEPER_GL_VIEWPORT_HPP
#define ANTKEEPER_GL_VIEWPORT_HPP

#include <cstdint>

namespace gl {

/**
 * Viewport position, dimensions, and depth range.
 */
struct viewport
{
	/// X-coordinate of the viewport's lower left corner.
	float x{0.0f};
	
	/// Y-coordinate of the viewport's lower left corner.
	float y{0.0f};
	
	/// Width of the viewport.
	float width{0.0f};
	
	/// Height of the viewport.
	float height{0.0f};
	
	/// Minimum depth range of the viewport.
	float min_depth{0.0f};
	
	/// Maximum depth range of the viewport.
	float max_depth{1.0f};
};

} // namespace gl

#endif // ANTKEEPER_GL_VIEWPORT_HPP
