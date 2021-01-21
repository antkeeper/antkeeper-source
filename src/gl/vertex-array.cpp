/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "gl/vertex-array.hpp"
#include "gl/vertex-buffer.hpp"
#include <glad/glad.h>

namespace gl {

static constexpr GLenum vertex_attribute_type_lut[] =
{
	GL_BYTE,
	GL_UNSIGNED_BYTE,
	GL_SHORT,
	GL_UNSIGNED_SHORT,
	GL_INT,
	GL_UNSIGNED_INT,
	GL_HALF_FLOAT,
	GL_FLOAT,
	GL_DOUBLE
};

vertex_array::vertex_array():
	gl_array_id(0)
{
	glGenVertexArrays(1, &gl_array_id);
}

vertex_array::~vertex_array()
{
	glDeleteVertexArrays(1, &gl_array_id);
}

void vertex_array::bind_attribute(unsigned int index, const vertex_buffer& buffer, int size, vertex_attribute_type type, int stride, std::size_t offset)
{
	GLenum gl_type = vertex_attribute_type_lut[static_cast<std::size_t>(type)];

	glBindVertexArray(gl_array_id);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.gl_buffer_id);
	glVertexAttribPointer(index, size, gl_type, GL_FALSE, stride, (const GLvoid*)offset); 
	glEnableVertexAttribArray(index);
}

void vertex_array::bind_elements(const vertex_buffer& buffer)
{
	glBindVertexArray(gl_array_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.gl_buffer_id);
}

} // namespace gl
