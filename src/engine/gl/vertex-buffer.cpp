// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <glad/gl.h>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/utility/sized-types.hpp>
#include <stdexcept>
#include <utility>

namespace engine::gl::opengl
{
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
}

namespace engine::gl
{
	vertex_buffer::vertex_buffer(buffer_usage usage, usize size, std::span<const std::byte> data):
		m_usage{usage},
		m_size{size}
	{
		if (!data.empty())
		{
			// Bounds check
			if (data.size() < size)
			{
				throw std::out_of_range("Vertex buffer construct operation exceeded data bounds.");
			}
		}

		const GLenum gl_usage = opengl::buffer_usage_lut[std::to_underlying(m_usage)];
		glCreateBuffers(1, &m_gl_named_buffer);
		glNamedBufferData(m_gl_named_buffer, static_cast<GLsizeiptr>(m_size), data.empty() ? nullptr : data.data(), gl_usage);
	}

	vertex_buffer::vertex_buffer(const vertex_buffer& buffer):
		vertex_buffer(buffer.m_usage, buffer.m_size)
	{
		copy(buffer, m_size);
	}

	vertex_buffer::vertex_buffer(vertex_buffer&& buffer):
		m_gl_named_buffer{std::exchange(buffer.m_gl_named_buffer, 0)},
		m_usage{std::move(buffer.m_usage)},
		m_size{std::exchange(buffer.m_size, 0)}
	{
	}

	vertex_buffer::~vertex_buffer()
	{
		if (m_gl_named_buffer)
		{
			glDeleteBuffers(1, &m_gl_named_buffer);
		}
	}

	vertex_buffer& vertex_buffer::operator=(const vertex_buffer& buffer)
	{
		repurpose(buffer.m_usage, buffer.m_size);
		copy(buffer, m_size);

		return *this;
	}

	vertex_buffer& vertex_buffer::operator=(vertex_buffer&& buffer)
	{
		if (m_gl_named_buffer)
		{
			glDeleteBuffers(1, &m_gl_named_buffer);
		}

		m_gl_named_buffer = std::exchange(buffer.m_gl_named_buffer, 0);
		m_usage = std::move(buffer.m_usage);
		m_size = std::exchange(buffer.m_size, 0);

		return *this;
	}

	void vertex_buffer::repurpose(buffer_usage usage, usize size, std::span<const std::byte> data)
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

		const auto gl_usage = opengl::buffer_usage_lut[std::to_underlying(m_usage)];
		glNamedBufferData(m_gl_named_buffer, static_cast<GLsizeiptr>(m_size), data.empty() ? nullptr : data.data(), gl_usage);
	}

	void vertex_buffer::write(usize offset, std::span<const std::byte> data)
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

		glNamedBufferSubData(m_gl_named_buffer, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(data.size()), data.data());
	}

	void vertex_buffer::read(usize offset, std::span<std::byte> data) const
	{
		// Ignore empty read operations
		if (data.empty())
		{
			return;
		}

		// Bounds check
		if (offset + data.size() > m_size)
		{
			throw std::out_of_range("Vertex buffer read operation exceeded buffer bounds.");
		}

		glGetNamedBufferSubData(m_gl_named_buffer, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(data.size()), data.data());
	}

	void vertex_buffer::copy(const vertex_buffer& read_buffer, usize copy_size, usize read_offset, usize write_offset)
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

		glCopyNamedBufferSubData(read_buffer.m_gl_named_buffer, m_gl_named_buffer, static_cast<GLintptr>(read_offset), static_cast<GLintptr>(write_offset), static_cast<GLsizeiptr>(copy_size));
	}
}
