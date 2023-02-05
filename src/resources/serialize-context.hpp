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

#ifndef ANTKEEPER_RESOURCES_SERIALIZE_CONTEXT_HPP
#define ANTKEEPER_RESOURCES_SERIALIZE_CONTEXT_HPP

#include <cstddef>
#include <bit>

/**
 * Provides access to a serialization state.
 */
struct serialize_context
{
public:
	static inline constexpr std::endian endian = std::endian::little;
	
	/**
	 * Writes 8-bit (byte) data.
	 *
	 * @param data Pointer to data source.
	 * @param count Number of bytes to write.
	 *
	 * @return Number of bytes written.
	 *
	 * @throw serialize_error Write error.
	 */
	std::size_t write8(const std::byte* data, std::size_t count) noexcept(false);
	
	/**
	 * Writes 16-bit (word) data.
	 *
	 * @param data Pointer to data source.
	 * @param count Number of words to write.
	 *
	 * @return Number of words written.
	 *
	 * @throw serialize_error Write error.
	 */
	std::size_t write16(const std::byte* data, std::size_t count) noexcept(false);
	
	/**
	 * Writes 32-bit (double word) data.
	 *
	 * @param data Pointer to data source.
	 * @param count Number of double words to write.
	 *
	 * @return Number of double words written.
	 *
	 * @throw serialize_error Write error.
	 */
	std::size_t write32(const std::byte* data, std::size_t count) noexcept(false);
	
	/**
	 * Writes 64-bit (quad word) data.
	 *
	 * @param data Pointer to data source.
	 * @param count Number of quad words to write.
	 *
	 * @return Number of quad words written.
	 *
	 * @throw serialize_error Write error.
	 */
	std::size_t write64(const std::byte* data, std::size_t count) noexcept(false);
	
	/**
	 * Returns `true` if an error occured during a write operation, `false` otherwise.
	 */
	inline bool error() const noexcept
	{
		return m_error;
	}
	
private:
	template <class T>
	friend class resource_loader;
	
	serialize_context(void* handle);
	
	void* handle;
	bool m_error;
};

#endif // ANTKEEPER_RESOURCES_SERIALIZE_CONTEXT_HPP
