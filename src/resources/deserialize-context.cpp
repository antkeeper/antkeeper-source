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

#include "resources/deserialize-context.hpp"
#include "resources/deserialize-error.hpp"
#include <physfs.h>

deserialize_context::deserialize_context(void* handle):
	handle(handle),
	m_eof(false),
	m_error(false)
{}

std::size_t deserialize_context::read8(std::byte* data, std::size_t count)
{
	const PHYSFS_sint64 status = PHYSFS_readBytes(reinterpret_cast<PHYSFS_File*>(handle), data, count);
	
	if (status < 0)
	{
		m_error = true;
		throw deserialize_error(PHYSFS_getLastError());
		//return 0;
	}
	
	if (status != count)
	{
		m_eof = true;
		m_error = true;
		throw deserialize_error(PHYSFS_getLastError());
		//return static_cast<std::size_t>(count);
	}
	
	return count;
}

std::size_t deserialize_context::read16_le(std::byte* data, std::size_t count)
{
	PHYSFS_File* file = reinterpret_cast<PHYSFS_File*>(handle);
	PHYSFS_uint16* data16 = reinterpret_cast<PHYSFS_uint16*>(data);
	
	for (std::size_t i = 0; i < count; ++i)
	{
		if (!PHYSFS_readULE16(file, data16))
		{
			m_eof = (PHYSFS_eof(file) != 0);
			m_error = true;
			throw deserialize_error(PHYSFS_getLastError());
			//return i;
		}
		
		++data16;
	}
	
	return count;
}

std::size_t deserialize_context::read16_be(std::byte* data, std::size_t count)
{
	PHYSFS_File* file = reinterpret_cast<PHYSFS_File*>(handle);
	PHYSFS_uint16* data16 = reinterpret_cast<PHYSFS_uint16*>(data);
	
	for (std::size_t i = 0; i < count; ++i)
	{
		if (!PHYSFS_readUBE16(file, data16))
		{
			m_eof = (PHYSFS_eof(file) != 0);
			m_error = true;
			throw deserialize_error(PHYSFS_getLastError());
			//return i;
		}
		
		++data16;
	}
	
	return count;
}

std::size_t deserialize_context::read32_le(std::byte* data, std::size_t count)
{
	PHYSFS_File* file = reinterpret_cast<PHYSFS_File*>(handle);
	PHYSFS_uint32* data32 = reinterpret_cast<PHYSFS_uint32*>(data);
	
	for (std::size_t i = 0; i < count; ++i)
	{
		if (!PHYSFS_readULE32(file, data32))
		{
			m_eof = (PHYSFS_eof(file) != 0);
			m_error = true;
			throw deserialize_error(PHYSFS_getLastError());
			//return i;
		}
		
		++data32;
	}
	
	return count;
}

std::size_t deserialize_context::read32_be(std::byte* data, std::size_t count)
{
	PHYSFS_File* file = reinterpret_cast<PHYSFS_File*>(handle);
	PHYSFS_uint32* data32 = reinterpret_cast<PHYSFS_uint32*>(data);
	
	for (std::size_t i = 0; i < count; ++i)
	{
		if (!PHYSFS_readUBE32(file, data32))
		{
			m_eof = (PHYSFS_eof(file) != 0);
			m_error = true;
			throw deserialize_error(PHYSFS_getLastError());
			//return i;
		}
		
		++data32;
	}
	
	return count;
}

std::size_t deserialize_context::read64_le(std::byte* data, std::size_t count)
{
	PHYSFS_File* file = reinterpret_cast<PHYSFS_File*>(handle);
	PHYSFS_uint64* data64 = reinterpret_cast<PHYSFS_uint64*>(data);
	
	for (std::size_t i = 0; i < count; ++i)
	{
		if (!PHYSFS_readULE64(file, data64))
		{
			m_eof = (PHYSFS_eof(file) != 0);
			m_error = true;
			throw deserialize_error(PHYSFS_getLastError());
			//return i;
		}
		
		++data64;
	}
	
	return count;
}

std::size_t deserialize_context::read64_be(std::byte* data, std::size_t count)
{
	PHYSFS_File* file = reinterpret_cast<PHYSFS_File*>(handle);
	PHYSFS_uint64* data64 = reinterpret_cast<PHYSFS_uint64*>(data);
	
	for (std::size_t i = 0; i < count; ++i)
	{
		if (!PHYSFS_readUBE64(file, data64))
		{
			m_eof = (PHYSFS_eof(file) != 0);
			m_error = true;
			throw deserialize_error(PHYSFS_getLastError());
			//return i;
		}
		
		++data64;
	}
	
	return count;
}