// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RESOURCES_SERIALIZE_CONTEXT_HPP
#define ANTKEEPER_RESOURCES_SERIALIZE_CONTEXT_HPP

#include <cstddef>
#include <bit>
#include <filesystem>

/// Provides access to a serialization state.
struct serialize_context
{
public:
	/// Destructs a serialize context.
	virtual ~serialize_context() = default;
	
	/// Returns the path associated with this serialize context.
	[[nodiscard]] virtual const std::filesystem::path& path() const noexcept = 0;
	
	/// Returns `true` if an error occured during a write operation or initialization, `false` otherwise.
	[[nodiscard]] virtual bool error() const noexcept = 0;
	
	/// Writes 8-bit (byte) data.
	/// @param data Pointer to data source.
	/// @param count Number of bytes to write.
	/// @return Number of bytes written.
	/// @throw serialize_error Write error.
	virtual std::size_t write8(const std::byte* data, std::size_t count) noexcept(false) = 0;
	
	/// Writes 16-bit (word) little-endian data.
	/// @param data Pointer to data source.
	/// @param count Number of words to write.
	/// @return Number of words written.
	/// @throw serialize_error Write error.
	virtual std::size_t write16_le(const std::byte* data, std::size_t count) noexcept(false) = 0;
	
	/// Writes 16-bit (word) big-endian data.
	/// @param data Pointer to data source.
	/// @param count Number of words to write.
	/// @return Number of words written.
	/// @throw serialize_error Write error.
	virtual std::size_t write16_be(const std::byte* data, std::size_t count) noexcept(false) = 0;
	
	/// Writes 16-bit (word) data.
	/// @tparam Endian Endianness of the write operation.
	/// @param data Pointer to data source.
	/// @param count Number of words to write.
	/// @return Number of words written.
	/// @throw serialize_error Write error.
	template <std::endian Endian>
	inline std::size_t write16(const std::byte* data, std::size_t count) noexcept(false)
	{
		if constexpr (Endian == std::endian::little)
		{
			return write16_le(data, count);
		}
		else
		{
			return write16_be(data, count);
		}
	}
	
	/// Writes 32-bit (double word) little-endian data.
	/// @param data Pointer to data source.
	/// @param count Number of double words to write.
	/// @return Number of double words written.
	/// @throw serialize_error Write error.
	virtual std::size_t write32_le(const std::byte* data, std::size_t count) noexcept(false) = 0;
	
	/// Writes 32-bit (double word) big-endian data.
	/// @param data Pointer to data source.
	/// @param count Number of double words to write.
	/// @return Number of double words written.
	/// @throw serialize_error Write error.
	virtual std::size_t write32_be(const std::byte* data, std::size_t count) noexcept(false) = 0;
	
	/// Writes 32-bit (double word) data.
	/// @tparam Endian Endianness of the write operation.
	/// @param data Pointer to data source.
	/// @param count Number of double words to write.
	/// @return Number of double words written.
	/// @throw serialize_error Write error.
	template <std::endian Endian>
	inline std::size_t write32(const std::byte* data, std::size_t count) noexcept(false)
	{
		if constexpr (Endian == std::endian::little)
		{
			return write32_le(data, count);
		}
		else
		{
			return write32_be(data, count);
		}
	}
	
	/// Writes 64-bit (quad word) little-endian data.
	/// @param data Pointer to data source.
	/// @param count Number of quad words to write.
	/// @return Number of quad words written.
	/// @throw serialize_error Write error.
	virtual std::size_t write64_le(const std::byte* data, std::size_t count) noexcept(false) = 0;
	
	/// Writes 64-bit (quad word) big-endian data.
	/// @param data Pointer to data source.
	/// @param count Number of quad words to write.
	/// @return Number of quad words written.
	/// @throw serialize_error Write error.
	virtual std::size_t write64_be(const std::byte* data, std::size_t count) noexcept(false) = 0;
	
	/// Writes 64-bit (quad word) data.
	/// @tparam Endian Endianness of the write operation.
	/// @param data Pointer to data source.
	/// @param count Number of quad words to write.
	/// @return Number of quad words written.
	/// @throw serialize_error Write error.
	template <std::endian Endian>
	inline std::size_t write64(const std::byte* data, std::size_t count) noexcept(false)
	{
		if constexpr (Endian == std::endian::little)
		{
			return write64_le(data, count);
		}
		else
		{
			return write64_be(data, count);
		}
	}
};

#endif // ANTKEEPER_RESOURCES_SERIALIZE_CONTEXT_HPP
