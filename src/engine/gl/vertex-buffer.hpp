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

#ifndef ANTKEEPER_GL_VERTEX_BUFFER_HPP
#define ANTKEEPER_GL_VERTEX_BUFFER_HPP

#include <cstddef>
#include <engine/gl/buffer-usage.hpp>
#include <span>

namespace gl {

class vertex_array;

/**
 * Vertex buffer object (VBO).
 */
class vertex_buffer
{
public:
	/**
	 * Constructs a vertex buffer.
	 *
	 * @param usage Buffer usage hint.
	 * @param size Buffer size, in bytes.
	 * @param data Buffer data. If empty, buffer data will not be set.
	 *
	 * @except std::out_of_range Vertex buffer creation operation exceeded data bounds.
	 */
	vertex_buffer(buffer_usage usage, std::size_t size, std::span<const std::byte> data = {});
	
	/**
	 * Constructs an empty vertex buffer.
	 */
	vertex_buffer();
	
	/// Destroys a vertex buffer.
	~vertex_buffer();
	
	vertex_buffer(const vertex_buffer&) = delete;
	vertex_buffer(vertex_buffer&&) = delete;
	vertex_buffer& operator=(const vertex_buffer&) = delete;
	vertex_buffer& operator=(vertex_buffer&&) = delete;
	
	/**
	 * Repurposes the vertex buffer, changing its usage hint, size, and updating its data.
	 *
	 * @param usage New buffer usage hint.
	 * @param size New buffer size, in bytes.
	 * @param data New buffer data. If empty, buffer data will not be updated.
	 *
	 * @except std::out_of_range Vertex buffer resize operation exceeded data bounds.
	 */
	/// @{
	void repurpose(buffer_usage usage, std::size_t size, std::span<const std::byte> data = {});
	void repurpose(buffer_usage usage, std::span<const std::byte> data = {});
	/// @}
	
	/**
	 * Resizes the vertex buffer.
	 *
	 * @param size New buffer size, in bytes.
	 * @param data New buffer data. If empty, buffer data will not be updated.
	 *
	 * @except std::out_of_range Vertex buffer resize operation exceeded data bounds.
	 */
	void resize(std::size_t size, std::span<const std::byte> data = {});
	
	/**
	 * Writes data into the vertex buffer.
	 *
	 * @param offset Offset into the buffer's data, in bytes, where writing will begin.
	 * @param data Data to write into the buffer.
	 *
	 * @except std::out_of_range Vertex buffer write operation exceeded buffer bounds.
	 */
	void write(std::span<const std::byte> data, std::size_t offset = 0);
	
	/**
	 * Reads a subset of the buffer's data from the GL and returns it to the application.
	 *
	 * @param offset Offset into the buffer's data, in bytes, where reading will begin.
	 * @param data Data buffer where the read bytes will be stored.
	 *
	 * @except std::out_of_range Vertex buffer read operation exceeded buffer bounds.
	 */
	void read(std::span<std::byte> data, std::size_t offset = 0) const;
	
	/**
	 * Copies a subset of another vertex buffer's data into this vertex buffer.
	 *
	 * @param read_buffer Buffer from which data will be read.
	 * @param copy_size Number of bytes to copy from the read buffer into this buffer.
	 * @param read_offset Offset into the read buffer's data, in bytes, where reading will begin.
	 * @param write_offset Offset into the this buffer's data, in bytes, where writing will begin.
	 *
	 * @except std::out_of_range Vertex buffer copy operation exceeded read buffer bounds.
	 * @except std::out_of_range Vertex buffer copy operation exceeded write buffer bounds.
	 */
	void copy(const vertex_buffer& read_buffer, std::size_t copy_size, std::size_t read_offset = 0, std::size_t write_offset = 0);
	
	/// Returns the size of the buffer's data, in bytes.
	[[nodiscard]] inline std::size_t size() const noexcept
	{
		return m_size;
	}
	
	/// Return's the buffer's usage hint.
	[[nodiscard]] inline buffer_usage usage() const noexcept
	{
		return m_usage;
	}

private:
	friend class vertex_array;
	
	unsigned int gl_buffer_id{0};
	buffer_usage m_usage{buffer_usage::static_draw};
	std::size_t m_size{0};
};

} // namespace gl

#endif // ANTKEEPER_GL_VERTEX_BUFFER_HPP
