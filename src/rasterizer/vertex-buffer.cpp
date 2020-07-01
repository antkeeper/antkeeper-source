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

#include "rasterizer/vertex-buffer.hpp"
#include <glad/glad.h>

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
	glBufferData(GL_ARRAY_BUFFER, size, data, gl_usage);
}

vertex_buffer::vertex_buffer():
	vertex_buffer(0, nullptr, buffer_usage::static_draw)
{}

vertex_buffer::~vertex_buffer()
{
	glDeleteBuffers(1, &gl_buffer_id);
}

void vertex_buffer::repurpose(std::size_t size, const void* data, buffer_usage usage)
{
	this->size = size;
	this->usage = usage;

	GLenum gl_usage = buffer_usage_lut[static_cast<std::size_t>(usage)];

	glBindBuffer(GL_ARRAY_BUFFER, gl_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, size, data, gl_usage);
}

void vertex_buffer::resize(std::size_t size, const void* data)
{
	repurpose(size, data, usage);
}

void vertex_buffer::update(int offset, std::size_t size, const void* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, gl_buffer_id);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

