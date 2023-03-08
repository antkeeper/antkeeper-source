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

#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <glad/glad.h>
#include <stdexcept>
#include <string>

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

vertex_array::vertex_array()
{
	glGenVertexArrays(1, &gl_array_id);
}

vertex_array::~vertex_array()
{
	glDeleteVertexArrays(1, &gl_array_id);
}

void vertex_array::bind(attribute_location_type location, const vertex_attribute& attribute)
{
	if (attribute.buffer == nullptr)
	{
		throw std::invalid_argument("Cannot bind vertex attribute that has a null vertex buffer.");
	}
	
	if (attribute.components < 1 || attribute.components > 4)
	{
		throw std::invalid_argument("Cannot bind vertex attribute that has an unsupported number of components (" + std::to_string(attribute.components) + ")");
	}
	
	m_attributes[location] = attribute;
	
	GLenum gl_type = vertex_attribute_type_lut[static_cast<std::size_t>(attribute.type)];
	glBindVertexArray(gl_array_id);
	glBindBuffer(GL_ARRAY_BUFFER, attribute.buffer->gl_buffer_id);
	glVertexAttribPointer
	(
		static_cast<GLuint>(location),
		static_cast<GLint>(attribute.components),
		gl_type,
		GL_FALSE,
		static_cast<GLsizei>(attribute.stride),
		(const GLvoid*)attribute.offset
	); 
	glEnableVertexAttribArray(static_cast<GLuint>(location));
}

void vertex_array::unbind(attribute_location_type location)
{
	if (auto it = m_attributes.find(location); it != m_attributes.end())
	{
		glBindVertexArray(gl_array_id);
		glDisableVertexAttribArray(static_cast<GLuint>(location));
		
		m_attributes.erase(it);
	}
	else
	{
		throw std::invalid_argument("Non-existent vertex attribute cannot be unbound.");
	}
}

} // namespace gl
