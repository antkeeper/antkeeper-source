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

#ifndef ANTKEEPER_GL_STENCIL_FACE_BITS_HPP
#define ANTKEEPER_GL_STENCIL_FACE_BITS_HPP

#include <cstdint>

namespace gl {

/// Stencil face mask bits specifying which stencil state(s) to update.
enum: std::uint8_t
{
	/// Only the front set of stencil state is updated.
	stencil_face_front_bit = 0b01,
	
	/// Only the back set of stencil state is updated.
	stencil_face_back_bit = 0b10,
	
	/// Both sets of stencil state are updated.
	stencil_face_front_and_back = 0b11,
};

} // namespace gl

#endif // ANTKEEPER_GL_STENCIL_FACE_BITS_HPP
