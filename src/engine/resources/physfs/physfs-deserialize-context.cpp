// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/resources/physfs/physfs-deserialize-context.hpp>
#include <engine/resources/deserialize-error.hpp>

physfs_deserialize_context::physfs_deserialize_context(const std::filesystem::path& path)
{
	// Open file for reading using PhysicsFS
	m_file = PHYSFS_openRead(path.string().c_str());
	if (!m_file)
	{
		throw deserialize_error(PHYSFS_getLastError());
	}
	
	// Store file path
	m_path = path;
	
	// Set EOF and error flags if file not open.
	m_eof = !m_file;
	m_error = !m_file;
}

physfs_deserialize_context::~physfs_deserialize_context()
{
	if (m_file)
	{
		PHYSFS_close(m_file);
	}
}

void physfs_deserialize_context::open(const std::filesystem::path& path)
{
	// Close file, if open
	if (m_file)
	{
		PHYSFS_close(m_file);
	}
	
	// Open file for reading using PhysicsFS
	m_file = PHYSFS_openRead(path.string().c_str());
	if (!m_file)
	{
		throw deserialize_error(PHYSFS_getLastError());
	}
	
	// Store file path
	m_path = path;
	
	// Set EOF and error flags if file not open.
	m_eof = !m_file;
	m_error = !m_file;
}

void physfs_deserialize_context::close() noexcept
{
	if (m_file)
	{
		m_error = !PHYSFS_close(m_file);
		m_file = nullptr;
		m_path.clear();
		m_eof = true;
	}
}

bool physfs_deserialize_context::is_open() const noexcept
{
	return m_file;
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
	const PHYSFS_sint64 length = PHYSFS_fileLength(m_file);
	if (length >= 0)
	{
		return static_cast<std::size_t>(length);
	}
	
	return 0;
};

std::size_t physfs_deserialize_context::tell() const
{
	const PHYSFS_sint64 offset = PHYSFS_tell(m_file);
	if (offset < 0)
	{
		// m_error = true;
		throw deserialize_error(PHYSFS_getLastError());
	}
	
	return static_cast<std::size_t>(offset);
}

void physfs_deserialize_context::seek(std::size_t offset)
{
	if (!PHYSFS_seek(m_file, static_cast<PHYSFS_uint64>(offset)))
	{
		m_error = true;
		throw deserialize_error(PHYSFS_getLastError());
	}
	
	m_eof = (PHYSFS_eof(m_file) != 0);
}

std::size_t physfs_deserialize_context::read8(std::byte* data, std::size_t count)
{
	const PHYSFS_sint64 status = PHYSFS_readBytes(m_file, data, count);
	
	if (status != count)
	{
		if (status < 0 || !PHYSFS_eof(m_file))
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
		if (!PHYSFS_readULE16(m_file, data16))
		{
			m_error = true;
			m_eof = (PHYSFS_eof(m_file) != 0);
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
		if (!PHYSFS_readUBE16(m_file, data16))
		{
			m_error = true;
			m_eof = (PHYSFS_eof(m_file) != 0);
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
		if (!PHYSFS_readULE32(m_file, data32))
		{
			m_error = true;
			m_eof = (PHYSFS_eof(m_file) != 0);
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
		if (!PHYSFS_readUBE32(m_file, data32))
		{
			m_error = true;
			m_eof = (PHYSFS_eof(m_file) != 0);
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
		if (!PHYSFS_readULE64(m_file, data64))
		{
			m_error = true;
			m_eof = (PHYSFS_eof(m_file) != 0);
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
		if (!PHYSFS_readUBE64(m_file, data64))
		{
			m_error = true;
			m_eof = (PHYSFS_eof(m_file) != 0);
			throw deserialize_error(PHYSFS_getLastError());
		}
		
		++data64;
	}
	
	return count;
}
