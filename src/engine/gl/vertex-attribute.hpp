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

#ifndef ANTKEEPER_GL_VERTEX_ATTRIBUTE_HPP
#define ANTKEEPER_GL_VERTEX_ATTRIBUTE_HPP

#include <cstddef>
#include <cstdint>

namespace gl {

class vertex_buffer;

enum class vertex_attribute_type: std::uint8_t
{
	int_8,
	uint_8,
	int_16,
	uint_16,
	int_32,
	uint_32,
	float_16,
	float_32,
	float_64
};

/**
 * Describes a vertex attribute within a vertex buffer.
 *
 * @see gl::vertex_buffer
 * @see gl::vertex_array
 */
struct vertex_attribute
{
	/// Pointer to the vertex buffer containing vertex attribute data.
	const vertex_buffer* buffer{nullptr};
	
	/// Offset to the first component of the first instance of this attribute in the vertex buffer, in bytes.
	std::size_t offset{0};
	
	/// Number of bytes between consecutive instances of this attribute in the vertex buffer. A value of `0` indicates attribute instances are tightly packed.
	std::size_t stride{0};
	
	/// Data type of each component in the attribute.
	vertex_attribute_type type{0};
	
	/// Number of components per attribute instance. Supported values are `1`, `2`, `3`, and `4`.
	std::uint8_t components{0};
};

} // namespace gl

#endif // ANTKEEPER_GL_VERTEX_ATTRIBUTE_HPP
