// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RESOURCES_DESERIALIZE_CONTEXT_HPP
#define ANTKEEPER_RESOURCES_DESERIALIZE_CONTEXT_HPP

#include <cstddef>
#include <bit>
#include <filesystem>

/// Provides access to a deserialization state.
struct deserialize_context
{
public:
	/// Destructs a deserialize context.
	virtual ~deserialize_context() = default;
	
	/// Returns the path associated with this deserialize context.
	[[nodiscard]] virtual const std::filesystem::path& path() const noexcept = 0;
	
	/// Returns `true` if an error occured during a read operation or initialization, `false` otherwise.
	[[nodiscard]] virtual bool error() const noexcept = 0;
	
	/// Returns `true` if the end of a file was reached.
	[[nodiscard]] virtual bool eof() const noexcept = 0;
	
	/// Returns the size of the file, in bytes.
	[[nodiscard]] virtual std::size_t size() const noexcept = 0;
	
	/// Returns the offsets from the start of the file to the current position, in bytes.
	/// @throw deserialize_error Tell error.
	[[nodiscard]] virtual std::size_t tell() const = 0;
	
	/// Seeks to a position in the file.
	/// @param offset Offset from the start of the file, in bytes.
	/// @throw deserialize_error Seek error.
	virtual void seek(std::size_t offset) = 0;
	
	/// Reads 8-bit (byte) data.
	/// @param[out] data Pointer to data destination.
	/// @param[in] count Number of bytes to read.
	/// @return Number of bytes read.
	/// @throw deserialize_error Read error.
	virtual std::size_t read8(std::byte* data, std::size_t count) noexcept(false) = 0;
	
	/// Reads 16-bit (word) little-endian data.
	/// @param[out] data Pointer to data destination.
	/// @param[in] count Number of words to read.
	/// @return Number of words read.
	/// @throw deserialize_error Read error.
	virtual std::size_t read16_le(std::byte* data, std::size_t count) noexcept(false) = 0;
	
	/// Reads 16-bit (word) big-endian data.
	/// @param[out] data Pointer to data destination.
	/// @param[in] count Number of words to read.
	/// @return Number of words read.
	/// @throw deserialize_error Read error.
	virtual std::size_t read16_be(std::byte* data, std::size_t count) noexcept(false) = 0;
	
	/// Reads 16-bit (word) data.
	/// @tparam Endian Endianness of the read operation.
	/// @param[out] data Pointer to data destination.
	/// @param[in] count Number of words to read.
	/// @return Number of words read.
	/// @throw deserialize_error Read error.
	template <std::endian Endian>
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
	
	/// Reads 32-bit (double word) little-endian data.
	/// @param[out] data Pointer to data destination.
	/// @param[in] count Number of double words to read.
	/// @return Number of double words read.
	/// @throw deserialize_error Read error.
	virtual std::size_t read32_le(std::byte* data, std::size_t count) noexcept(false) = 0;
	
	/// Reads 32-bit (double word) big-endian data.
	/// @param[out] data Pointer to data destination.
	/// @param[in] count Number of double words to read.
	/// @return Number of double words read.
	/// @throw deserialize_error Read error.
	virtual std::size_t read32_be(std::byte* data, std::size_t count) noexcept(false) = 0;
	
	/// Reads 32-bit (double word) data.
	/// @tparam Endian Endianness of the read operation.
	/// @param[out] data Pointer to data destination.
	/// @param[in] count Number of double words to read.
	/// @return Number of double words read.
	/// @throw deserialize_error Read error.
	template <std::endian Endian>
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
	
	/// Reads 64-bit (quad word) little-endian data.
	/// @param[out] data Pointer to data destination.
	/// @param[in] count Number of quad words to read.
	/// @return Number of quad words read.
	/// @throw deserialize_error Read error.
	virtual std::size_t read64_le(std::byte* data, std::size_t count) noexcept(false) = 0;
	
	/// Reads 64-bit (quad word) big-endian data.
	/// @param[out] data Pointer to data destination.
	/// @param[in] count Number of quad words to read.
	/// @return Number of quad words read.
	/// @throw deserialize_error Read error.
	virtual std::size_t read64_be(std::byte* data, std::size_t count) noexcept(false) = 0;
	
	/// Reads 64-bit (quad word) data.
	/// @tparam Endian Endianness of the read operation.
	/// @param[out] data Pointer to data destination.
	/// @param[in] count Number of quad words to read.
	/// @return Number of quad words read.
	/// @throw deserialize_error Read error.
	template <std::endian Endian>
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
};

#endif // ANTKEEPER_RESOURCES_DESERIALIZE_CONTEXT_HPP
