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

#ifndef ANTKEEPER_RESOURCES_DESERIALIZE_CONTEXT_HPP
#define ANTKEEPER_RESOURCES_DESERIALIZE_CONTEXT_HPP

#include <cstddef>
#include <bit>

/**
 * Provides access to a deserialization state.
 */
struct deserialize_context
{
public:
	/**
	 * Reads 8-bit (byte) data.
	 *
	 * @param data Pointer to data source.
	 * @param count Number of bytes to read.
	 *
	 * @return Number of bytes read.
	 *
	 * @throw deserialize_error Read error.
	 */
	std::size_t read8(std::byte* data, std::size_t count) noexcept(false);
	
	/**
	 * Reads 16-bit (word) data.
	 *
	 * @tparam Endian Endianness of the read operation.
	 *
	 * @param data Pointer to data destination.
	 * @param count Number of words to read.
	 *
	 * @return Number of words read.
	 *
	 * @throw deserialize_error Read error.
	 */
	template <std::endian Endian = std::endian::native>
	inline std::size_t read16(std::byte* data, std::size_t count) noexcept(false)
	{
		if constexpr (Endian == std::endian::little)
		{
			return read16_le(data, count);
		}
		else
		{
			return read16_be(data, count);
		}
	}
	
	/**
	 * Reads 32-bit (double word) data.
	 *
	 * @tparam Endian Endianness of the read operation.
	 *
	 * @param data Pointer to data destination.
	 * @param count Number of double words to read.
	 *
	 * @return Number of double words read.
	 *
	 * @throw deserialize_error Read error.
	 */
	template <std::endian Endian = std::endian::native>
	inline std::size_t read32(std::byte* data, std::size_t count) noexcept(false)
	{
		if constexpr (Endian == std::endian::little)
		{
			return read32_le(data, count);
		}
		else
		{
			return read32_be(data, count);
		}
	}
	
	/**
	 * Reads 64-bit (quad word) data.
	 *
	 * @tparam Endian Endianness of the read operation.
	 *
	 * @param data Pointer to data destination.
	 * @param count Number of quad words to read.
	 *
	 * @return Number of quad words read.
	 *
	 * @throw deserialize_error Read error.
	 */
	template <std::endian Endian = std::endian::native>
	inline std::size_t read64(std::byte* data, std::size_t count) noexcept(false)
	{
		if constexpr (Endian == std::endian::little)
		{
			return read64_le(data, count);
		}
		else
		{
			return read64_be(data, count);
		}
	}
	
	/**
	 * Returns `true` if the end of a file was reached.
	 */
	inline bool eof() const noexcept
	{
		return m_eof;
	}
	
	/**
	 * Returns `true` if an error occured during a read operation, `false` otherwise.
	 */
	inline bool error() const noexcept
	{
		return m_error;
	}
	
private:
	template <class T>
	friend class resource_loader;
	
	deserialize_context(void* handle);
	std::size_t read16_le(std::byte* data, std::size_t count) noexcept(false);
	std::size_t read16_be(std::byte* data, std::size_t count) noexcept(false);
	std::size_t read32_le(std::byte* data, std::size_t count) noexcept(false);
	std::size_t read32_be(std::byte* data, std::size_t count) noexcept(false);
	std::size_t read64_le(std::byte* data, std::size_t count) noexcept(false);
	std::size_t read64_be(std::byte* data, std::size_t count) noexcept(false);
	
	void* handle;
	bool m_eof;
	bool m_error;
};

#endif // ANTKEEPER_RESOURCES_DESERIALIZE_CONTEXT_HPP
