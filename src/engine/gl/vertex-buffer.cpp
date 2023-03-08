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

#include <engine/gl/vertex-buffer.hpp>
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

vertex_buffer::vertex_buffer(buffer_usage usage, std::size_t size, std::span<const std::byte> data):
	m_usage{usage},
	m_size{size}
{
	if (!data.empty())
	{
		// Bounds check
		if (data.size() < size)
		{
			throw std::out_of_range("Vertex buffer creation operation exceeded data bounds.");
		}
	}
	
	const GLenum gl_usage = buffer_usage_lut[static_cast<std::size_t>(m_usage)];
	glGenBuffers(1, &gl_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, gl_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_size), data.empty() ? nullptr : data.data(), gl_usage);
}

vertex_buffer::vertex_buffer():
	vertex_buffer(buffer_usage::static_draw, 0)
{}

vertex_buffer::~vertex_buffer()
{
	glDeleteBuffers(1, &gl_buffer_id);
}

void vertex_buffer::repurpose(buffer_usage usage, std::size_t size, std::span<const std::byte> data)
{
	if (!data.empty())
	{
		// Bounds check
		if (data.size() < size)
		{
			throw std::out_of_range("Vertex buffer resize operation exceeded data bounds.");
		}
	}
	
	m_usage = usage;
	m_size = size;
	
	const GLenum gl_usage = buffer_usage_lut[static_cast<std::size_t>(m_usage)];
	glBindBuffer(GL_ARRAY_BUFFER, gl_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_size), data.empty() ? nullptr : data.data(), gl_usage);
}

void vertex_buffer::repurpose(buffer_usage usage, std::span<const std::byte> data)
{
	repurpose(usage, m_size, data);
}

void vertex_buffer::resize(std::size_t size, std::span<const std::byte> data)
{
	repurpose(m_usage, size, data);
}

void vertex_buffer::write(std::span<const std::byte> data, std::size_t offset)
{
	// Ignore empty write operations
	if (data.empty())
	{
		return;
	}
	
	// Bounds check
	if (offset + data.size() > m_size)
	{
		throw std::out_of_range("Vertex buffer write operation exceeded buffer bounds.");
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, gl_buffer_id);
	glBufferSubData(GL_ARRAY_BUFFER, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(data.size()), data.data());
}

void vertex_buffer::read(std::span<std::byte> data, std::size_t offset) const
{
	// Abort empty read operations
	if (data.empty())
	{
		return;
	}
	
	// Bounds check
	if (offset + data.size() > m_size)
	{
		throw std::out_of_range("Vertex buffer read operation exceeded buffer bounds.");
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, gl_buffer_id);
	glGetBufferSubData(GL_ARRAY_BUFFER, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(data.size()), data.data());
}

void vertex_buffer::copy(const vertex_buffer& read_buffer, std::size_t copy_size, std::size_t read_offset, std::size_t write_offset)
{
	// Bounds check
	if (read_offset + copy_size > read_buffer.m_size)
	{
		throw std::out_of_range("Vertex buffer copy operation exceeded read buffer bounds.");
	}
	if (write_offset + copy_size > m_size)
	{
		throw std::out_of_range("Vertex buffer copy operation exceeded write buffer bounds.");
	}
	
	glBindBuffer(GL_COPY_READ_BUFFER, read_buffer.gl_buffer_id);
	glBindBuffer(GL_COPY_WRITE_BUFFER, gl_buffer_id);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, static_cast<GLintptr>(read_offset), static_cast<GLintptr>(write_offset), static_cast<GLsizeiptr>(copy_size));
}

} // namespace gl
