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

#ifndef ANTKEEPER_GL_VERTEX_BUFFER_HPP
#define ANTKEEPER_GL_VERTEX_BUFFER_HPP

#include <cstddef>
#include <cstdint>
#include "gl/buffer-usage.hpp"

namespace gl {

class vertex_array;

/**
 * Vertex buffer object (VBO).
 */
class vertex_buffer
{
public:
	/**
	 * Creates a vertex buffer, settings its size, uploading its data, and setting its usage hint.
	 *
	 * @param size Size of the buffer's data, in bytes.
	 * @param data Pointer to data that will be copied into the buffer, or `nullptr` if no data is to be copied.
	 * @param usage Buffer usage hint.
	 */
	explicit vertex_buffer(std::size_t size, const void* data = nullptr, buffer_usage usage = buffer_usage::static_draw);
	
	/// Creates an empty vertex buffer.
	vertex_buffer();
	
	/// Destroys a vertex buffer.
	~vertex_buffer();

	vertex_buffer(const vertex_buffer&) = delete;
	vertex_buffer& operator=(const vertex_buffer&) = delete;
	
	/**
	 * Repurposes a vertex buffer, changing its usage hint, its size, and replacing its data.
	 *
	 * @param usage New usage hint for the buffer.
	 * @param size New size of the buffer's data, in bytes.
	 * @param data Pointer to data that will be copied into the buffer, or `nullptr` if no data is to be copied.
	 */
	void repurpose(buffer_usage usage, std::size_t size, const void* data = nullptr);
	
	/**
	 * Resizes the vertex buffer.
	 *
	 * @param size New size of the buffer's data, in bytes.
	 * @param data Pointer to data that will be copied into the buffer, or `nullptr` if no data is to be copied.
	 */
	void resize(std::size_t size, const void* data = nullptr);
	
	/**
	 * Writes data into the vertex buffer.
	 *
	 * @param offset Offset into the buffer's data, in bytes, where writing will begin.
	 * @param size Size of the write operation, in bytes.
	 * @param data Pointer to the data that will be written.
	 *
	 * @except std::out_of_range Vertex buffer write operation exceeded buffer bounds.
	 */
	void write(std::size_t offset, std::size_t size, const void* data);
	
	/**
	 * Reads a subset of the buffer's data from the GL and returns it to the application.
	 *
	 * @param offset Offset into the buffer's data, in bytes, where reading will begin.
	 * @param size Size of the data to read, in bytes.
	 * @param data Pointer to where the read bytes will be stored.
	 *
	 * @except std::out_of_range Vertex buffer read operation exceeded buffer bounds.
	 */
	void read(std::size_t offset, std::size_t size, void* data) const;
	
	/// Returns the size of the buffer's data, in bytes.
	std::size_t get_size() const;
	
	/// Return's the buffer's usage hint.
	buffer_usage get_usage() const;

private:
	friend class vertex_array;
	
	std::uint_fast32_t gl_buffer_id;
	std::size_t size;
	buffer_usage usage;
};

inline std::size_t vertex_buffer::get_size() const
{
	return size;
}

inline buffer_usage vertex_buffer::get_usage() const
{
	return usage;
}

} // namespace gl

#endif // ANTKEEPER_GL_VERTEX_BUFFER_HPP
