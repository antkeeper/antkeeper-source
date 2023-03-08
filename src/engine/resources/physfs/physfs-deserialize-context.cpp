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

#include <engine/resources/physfs/physfs-deserialize-context.hpp>
#include <engine/resources/deserialize-error.hpp>

physfs_deserialize_context::physfs_deserialize_context(const std::filesystem::path& path)
{
	// Open file for reading using PhysicsFS
	file = PHYSFS_openRead(path.string().c_str());
	if (!file)
	{
		throw deserialize_error(PHYSFS_getLastError());
	}
	
	// Store file path
	m_path = path;
	
	// Set EOF and error flags if file not open.
	m_eof = !file;
	m_error = !file;
}

physfs_deserialize_context::~physfs_deserialize_context()
{
	if (file)
	{
		PHYSFS_close(file);
	}
}

void physfs_deserialize_context::open(const std::filesystem::path& path)
{
	// Close file, if open
	if (file)
	{
		PHYSFS_close(file);
	}
	
	// Open file for reading using PhysicsFS
	file = PHYSFS_openRead(path.string().c_str());
	if (!file)
	{
		throw deserialize_error(PHYSFS_getLastError());
	}
	
	// Store file path
	m_path = path;
	
	// Set EOF and error flags if file not open.
	m_eof = !file;
	m_error = !file;
}

void physfs_deserialize_context::close() noexcept
{
	if (file)
	{
		m_error = !PHYSFS_close(file);
		file = nullptr;
		m_path.clear();
		m_eof = true;
	}
}

bool physfs_deserialize_context::is_open() const noexcept
{
	return file;
}

const std::filesystem::path& physfs_deserialize_context::path() const noexcept
{
	return m_path;
}

bool physfs_deserialize_context::error() const noexcept
{
	return m_error;
}

bool physfs_deserialize_context::eof() const noexcept
{
	return m_eof;
}

std::size_t physfs_deserialize_context::size() const noexcept
{
	PHYSFS_sint64 length = PHYSFS_fileLength(file);
	if (length >= 0)
	{
		return static_cast<std::size_t>(length);
	}
	
	return 0;
};

std::size_t physfs_deserialize_context::tell() const
{
	PHYSFS_sint64 offset = PHYSFS_fileLength(file);
	if (offset < 0)
	{
		//m_error = true;
		throw deserialize_error(PHYSFS_getLastError());
	}
	
	return static_cast<std::size_t>(offset);
}

void physfs_deserialize_context::seek(std::size_t offset)
{
	if (!PHYSFS_seek(file, static_cast<PHYSFS_uint64>(offset)))
	{
		m_error = true;
		throw deserialize_error(PHYSFS_getLastError());
	}
	
	m_eof = (PHYSFS_eof(file) != 0);
}

std::size_t physfs_deserialize_context::read8(std::byte* data, std::size_t count)
{
	const PHYSFS_sint64 status = PHYSFS_readBytes(file, data, count);
	
	if (status != count)
	{
		if (status < 0 || !PHYSFS_eof(file))
		{
			m_error = true;
			throw deserialize_error(PHYSFS_getLastError());
		}
		
		m_eof = true;
		
		return static_cast<std::size_t>(status);
	}
	else
	{
		return count;
	}
}

std::size_t physfs_deserialize_context::read16_le(std::byte* data, std::size_t count)
{
	PHYSFS_uint16* data16 = reinterpret_cast<PHYSFS_uint16*>(data);
	
	for (std::size_t i = 0; i < count; ++i)
	{
		if (!PHYSFS_readULE16(file, data16))
		{
			m_error = true;
			m_eof = (PHYSFS_eof(file) != 0);
			throw deserialize_error(PHYSFS_getLastError());
		}
		
		++data16;
	}
	
	return count;
}

std::size_t physfs_deserialize_context::read16_be(std::byte* data, std::size_t count)
{
	PHYSFS_uint16* data16 = reinterpret_cast<PHYSFS_uint16*>(data);
	
	for (std::size_t i = 0; i < count; ++i)
	{
		if (!PHYSFS_readUBE16(file, data16))
		{
			m_error = true;
			m_eof = (PHYSFS_eof(file) != 0);
			throw deserialize_error(PHYSFS_getLastError());
		}
		
		++data16;
	}
	
	return count;
}

std::size_t physfs_deserialize_context::read32_le(std::byte* data, std::size_t count)
{
	PHYSFS_uint32* data32 = reinterpret_cast<PHYSFS_uint32*>(data);
	
	for (std::size_t i = 0; i < count; ++i)
	{
		if (!PHYSFS_readULE32(file, data32))
		{
			m_error = true;
			m_eof = (PHYSFS_eof(file) != 0);
			throw deserialize_error(PHYSFS_getLastError());
		}
		
		++data32;
	}
	
	return count;
}

std::size_t physfs_deserialize_context::read32_be(std::byte* data, std::size_t count)
{
	PHYSFS_uint32* data32 = reinterpret_cast<PHYSFS_uint32*>(data);
	
	for (std::size_t i = 0; i < count; ++i)
	{
		if (!PHYSFS_readUBE32(file, data32))
		{
			m_error = true;
			m_eof = (PHYSFS_eof(file) != 0);
			throw deserialize_error(PHYSFS_getLastError());
		}
		
		++data32;
	}
	
	return count;
}

std::size_t physfs_deserialize_context::read64_le(std::byte* data, std::size_t count)
{
	PHYSFS_uint64* data64 = reinterpret_cast<PHYSFS_uint64*>(data);
	
	for (std::size_t i = 0; i < count; ++i)
	{
		if (!PHYSFS_readULE64(file, data64))
		{
			m_error = true;
			m_eof = (PHYSFS_eof(file) != 0);
			throw deserialize_error(PHYSFS_getLastError());
		}
		
		++data64;
	}
	
	return count;
}

std::size_t physfs_deserialize_context::read64_be(std::byte* data, std::size_t count)
{
	PHYSFS_uint64* data64 = reinterpret_cast<PHYSFS_uint64*>(data);
	
	for (std::size_t i = 0; i < count; ++i)
	{
		if (!PHYSFS_readUBE64(file, data64))
		{
			m_error = true;
			m_eof = (PHYSFS_eof(file) != 0);
			throw deserialize_error(PHYSFS_getLastError());
		}
		
		++data64;
	}
	
	return count;
}
