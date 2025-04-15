// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import <physfs.h>;
import engine.resources.physfs_serialize_context;
import engine.resources.serialize_error;
import engine.utility.sized_types;

namespace engine::resources
{
	physfs_serialize_context::physfs_serialize_context(const std::filesystem::path& path)
	{
		// Open file for writing using PhysicsFS
		file = PHYSFS_openWrite(path.string().c_str());
		if (!file)
		{
			throw serialize_error(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}

		// Store file path
		m_path = path;

		// Set error flag if file not open.
		m_error = !file;
	}

	physfs_serialize_context::~physfs_serialize_context()
	{
		if (file)
		{
			PHYSFS_close(file);
		}
	}

	void physfs_serialize_context::open(const std::filesystem::path& path)
	{
		// Close file, if open
		if (file)
		{
			PHYSFS_close(file);
		}

		// Open file for writing using PhysicsFS
		file = PHYSFS_openWrite(path.string().c_str());
		if (!file)
		{
			throw serialize_error(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}

		// Store file path
		m_path = path;

		// Set error flag if file not open.
		m_error = !file;
	}

	void physfs_serialize_context::close() noexcept
	{
		if (file)
		{
			m_error = !PHYSFS_close(file);
			file = nullptr;
			m_path.clear();
		}
	}

	bool physfs_serialize_context::is_open() const noexcept
	{
		return file;
	}

	const std::filesystem::path& physfs_serialize_context::path() const noexcept
	{
		return m_path;
	}

	bool physfs_serialize_context::error() const noexcept
	{
		return m_error;
	}

	usize physfs_serialize_context::write8(const std::byte* data, usize count)
	{
		const PHYSFS_sint64 status = PHYSFS_writeBytes(file, data, count);

		if (status < 0)
		{
			m_error = true;
			throw serialize_error(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			//return 0;
		}

		if (static_cast<usize>(status) != count)
		{
			m_error = true;
			throw serialize_error(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			//return static_cast<usize>(count);
		}

		return count;
	}

	usize physfs_serialize_context::write16_le(const std::byte* data, usize count)
	{
		const PHYSFS_uint16* data16 = reinterpret_cast<const PHYSFS_uint16*>(data);

		for (usize i = 0; i < count; ++i)
		{
			if (!PHYSFS_writeULE16(file, *data16))
			{
				m_error = true;
				throw serialize_error(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
				//return i;
			}

			++data16;
		}

		return count;
	}

	usize physfs_serialize_context::write16_be(const std::byte* data, usize count)
	{
		const PHYSFS_uint16* data16 = reinterpret_cast<const PHYSFS_uint16*>(data);

		for (usize i = 0; i < count; ++i)
		{
			if (!PHYSFS_writeUBE16(file, *data16))
			{
				m_error = true;
				throw serialize_error(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
				//return i;
			}

			++data16;
		}

		return count;
	}

	usize physfs_serialize_context::write32_le(const std::byte* data, usize count)
	{
		const PHYSFS_uint32* data32 = reinterpret_cast<const PHYSFS_uint32*>(data);

		for (usize i = 0; i < count; ++i)
		{
			if (!PHYSFS_writeULE32(file, *data32))
			{
				m_error = true;
				throw serialize_error(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
				//return i;
			}

			++data32;
		}

		return count;
	}

	usize physfs_serialize_context::write32_be(const std::byte* data, usize count)
	{
		const PHYSFS_uint32* data32 = reinterpret_cast<const PHYSFS_uint32*>(data);

		for (usize i = 0; i < count; ++i)
		{
			if (!PHYSFS_writeUBE32(file, *data32))
			{
				m_error = true;
				throw serialize_error(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
				//return i;
			}

			++data32;
		}

		return count;
	}

	usize physfs_serialize_context::write64_le(const std::byte* data, usize count)
	{
		const PHYSFS_uint64* data64 = reinterpret_cast<const PHYSFS_uint64*>(data);

		for (usize i = 0; i < count; ++i)
		{
			if (!PHYSFS_writeULE64(file, *data64))
			{
				m_error = true;
				throw serialize_error(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
				//return i;
			}

			++data64;
		}

		return count;
	}

	usize physfs_serialize_context::write64_be(const std::byte* data, usize count)
	{
		const PHYSFS_uint64* data64 = reinterpret_cast<const PHYSFS_uint64*>(data);

		for (usize i = 0; i < count; ++i)
		{
			if (!PHYSFS_writeUBE64(file, *data64))
			{
				m_error = true;
				throw serialize_error(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
				//return i;
			}

			++data64;
		}

		return count;
	}
}
