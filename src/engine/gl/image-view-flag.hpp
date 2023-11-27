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

#ifndef ANTKEEPER_GL_IMAGE_VIEW_FLAG_HPP
#define ANTKEEPER_GL_IMAGE_VIEW_FLAG_HPP

#include <cstdint>

namespace gl {

/// Image flags.
enum class image_view_flag: std::uint8_t
{
	/// Image array view.
	array = 0b00000001,
	
	/// Cube map view.
	cube = 0b00000010
};

} // namespace gl

#endif // ANTKEEPER_GL_IMAGE_VIEW_FLAG_HPP
