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

#ifndef ANTKEEPER_GL_TEXTURE_TYPE_HPP
#define ANTKEEPER_GL_TEXTURE_TYPE_HPP

#include <cstdint>

namespace gl {

/// Texture types.
enum class texture_type: std::uint8_t
{
	/// 1D texture.
	one_dimensional,
	
	/// 2D texture.
	two_dimensional,
	
	/// 3D texture.
	three_dimensional,
	
	/// Cube texture.
	cube,
};

} // namespace gl

#endif // ANTKEEPER_GL_TEXTURE_TYPE_HPP
