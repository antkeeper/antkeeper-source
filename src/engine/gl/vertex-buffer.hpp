// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/gl/buffer-usage.hpp>
#include <engine/utility/sized-types.hpp>
#include <span>

namespace engine::gl
{
	/// Vertex buffer object (VBO).
	class vertex_buffer
	{
	public:
		/// Constructs a vertex buffer.
		/// @param usage Buffer usage hint.
		/// @param size Buffer size, in bytes.
		/// @param data Buffer data. If empty, buffer data will not be set.
		/// @exception std::out_of_range Vertex buffer construct operation exceeded data bounds.
		vertex_buffer(buffer_usage usage, usize size, std::span<const std::byte> data = {});

		/// Constructs a vertex buffer.
		/// @param usage Buffer usage hint.
		/// @param data Buffer data.
		inline explicit vertex_buffer(buffer_usage usage, std::span<const std::byte> data = {}):
			vertex_buffer(usage, data.size(), data)
		{
		}

		/// Constructs an empty vertex buffer.
		inline vertex_buffer():
			vertex_buffer(buffer_usage::static_draw, 0)
		{
		}

		/// Constructs a copy of a vertex buffer.
		/// @param buffer Buffer to copy.
		vertex_buffer(const vertex_buffer& buffer);

		/// Move-constructs a vertex buffer.
		/// @param buffer Buffer to move.
		vertex_buffer(vertex_buffer&& buffer) noexcept;

		/// Destroys a vertex buffer.
		~vertex_buffer();

		/// Copies another vertex buffer.
		/// @param buffer Buffer to copy.
		/// @return Reference to this vertex buffer.
		vertex_buffer& operator=(const vertex_buffer& buffer);

		/// Moves another vertex buffer into this buffer.
		/// @param buffer Buffer to move.
		/// @return Reference to this vertex buffer.
		vertex_buffer& operator=(vertex_buffer&& buffer) noexcept;

		/// Repurposes the vertex buffer, changing its usage hint, size, and updating its data.
		/// @param usage New buffer usage hint.
		/// @param size New buffer size, in bytes.
		/// @param data New buffer data. If empty, buffer data will not be updated.
		/// @exception std::out_of_range Vertex buffer resize operation exceeded data bounds.
		void repurpose(buffer_usage usage, usize size, std::span<const std::byte> data = {});

		/// Repurposes the vertex buffer, changing its usage hint and updating its data.
		/// @param usage New buffer usage hint.
		/// @param data New buffer data.
		inline void repurpose(buffer_usage usage, std::span<const std::byte> data)
		{
			repurpose(usage, data.size(), data);
		}

		/// Repurposes the vertex buffer, changing its usage hint.
		/// @param usage New buffer usage hint.
		inline void repurpose(buffer_usage usage)
		{
			repurpose(usage, m_size, {});
		}

		/// Resizes the vertex buffer.
		/// @param size New buffer size, in bytes.
		/// @param data New buffer data. If empty, buffer data will not be updated.
		/// @exception std::out_of_range Vertex buffer resize operation exceeded data bounds.
		inline void resize(usize size, std::span<const std::byte> data = {})
		{
			repurpose(m_usage, size, data);
		}

		/// Resizes the vertex buffer.
		/// @param data New buffer data.
		inline void resize(std::span<const std::byte> data)
		{
			repurpose(m_usage, data.size(), data);
		}

		/// Writes data into the vertex buffer.
		/// @param offset Offset into the buffer's data, in bytes, where writing will begin.
		/// @param data Data to write into the buffer.
		/// @exception std::out_of_range Vertex buffer write operation exceeded buffer bounds.
		void write(usize offset, std::span<const std::byte> data);

		/// Writes data into the vertex buffer.
		/// @param data Data to write into the buffer.
		/// @exception std::out_of_range Vertex buffer write operation exceeded buffer bounds.
		inline void write(std::span<const std::byte> data)
		{
			write(0, data);
		}

		/// Reads a subset of the buffer's data from the GL and returns it to the application.
		/// @param offset Offset into the buffer's data, in bytes, where reading will begin.
		/// @param data Data buffer where the read bytes will be stored.
		/// @exception std::out_of_range Vertex buffer read operation exceeded buffer bounds.
		void read(usize offset, std::span<std::byte> data) const;

		/// Reads the entire buffer's data from the GL and returns it to the application.
		/// @param data Data buffer where the read bytes will be stored.
		/// @exception std::out_of_range Vertex buffer read operation exceeded buffer bounds.
		inline void read(std::span<std::byte> data) const
		{
			read(0, data);
		}

		/// Copies a subset of another vertex buffer's data into this vertex buffer.
		/// @param read_buffer Buffer from which data will be read.
		/// @param copy_size Number of bytes to copy from the read buffer into this buffer.
		/// @param read_offset Offset into the read buffer's data, in bytes, where reading will begin.
		/// @param write_offset Offset into the this buffer's data, in bytes, where writing will begin.
		/// @exception std::out_of_range Vertex buffer copy operation exceeded read buffer bounds.
		/// @exception std::out_of_range Vertex buffer copy operation exceeded write buffer bounds.
		void copy(const vertex_buffer& read_buffer, usize copy_size, usize read_offset = 0, usize write_offset = 0);

		/// Returns the size of the buffer's data, in bytes.
		[[nodiscard]] inline constexpr usize size() const noexcept
		{
			return m_size;
		}

		/// Return's the buffer's usage hint.
		[[nodiscard]] inline constexpr buffer_usage usage() const noexcept
		{
			return m_usage;
		}

	private:
		friend class pipeline;

		unsigned int m_gl_named_buffer{0};
		buffer_usage m_usage{buffer_usage::static_draw};
		usize m_size{0};
	};
}
