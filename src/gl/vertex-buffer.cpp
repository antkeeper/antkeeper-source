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

#include "gl/vertex-buffer.hpp"
#include <stdexcept>
#include <glad/glad.h>

namespace gl {

static constexpr GLenum buffer_usage_lut[] =
{
	GL_STREAM_DRAW,
	GL_STREAM_READ,
	GL_STREAM_COPY,
	GL_STATIC_DRAW,
	GL_STATIC_READ,
	GL_STATIC_COPY,
	GL_DYNAMIC_DRAW,
	GL_DYNAMIC_READ,
	GL_DYNAMIC_COPY
};

vertex_buffer::vertex_buffer(std::size_t size, const void* data, buffer_usage usage):
	gl_buffer_id(0),
	size(size),
	usage(usage)
{
	GLenum gl_usage = buffer_usage_lut[static_cast<std::size_t>(usage)];
	glGenBuffers(1, &gl_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, gl_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, gl_usage);
}

vertex_buffer::vertex_buffer():
	vertex_buffer(0, nullptr, buffer_usage::static_draw)
{}

vertex_buffer::~vertex_buffer()
{
	glDeleteBuffers(1, &gl_buffer_id);
}

void vertex_buffer::repurpose(buffer_usage usage, std::size_t size, const void* data)
{
	this->size = size;
	this->usage = usage;
	
	GLenum gl_usage = buffer_usage_lut[static_cast<std::size_t>(usage)];
	glBindBuffer(GL_ARRAY_BUFFER, gl_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, gl_usage);
}

void vertex_buffer::resize(std::size_t size, const void* data)
{
	repurpose(usage, size, data);
}

void vertex_buffer::write(std::size_t offset, std::size_t size, const void* data)
{
	// Abort empty write operations
	if (!size)
		return;
	
	// Bounds check
	if (offset + size > this->size)
		throw std::out_of_range("Vertex buffer write operation exceeded buffer bounds.");
	
	glBindBuffer(GL_ARRAY_BUFFER, gl_buffer_id);
	glBufferSubData(GL_ARRAY_BUFFER, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);
}

void vertex_buffer::read(std::size_t offset, std::size_t size, void* data) const
{
	// Abort empty read operations
	if (!size)
		return;
	
	// Bounds check
	if (offset + size > this->size)
		throw std::out_of_range("Vertex buffer read operation exceeded buffer bounds.");
	
	glBindBuffer(GL_ARRAY_BUFFER, gl_buffer_id);
	glGetBufferSubData(GL_ARRAY_BUFFER, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);
}

} // namespace gl
