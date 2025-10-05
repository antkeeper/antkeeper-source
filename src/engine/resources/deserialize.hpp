// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/resources/deserialize-error.hpp>
#include <engine/utility/sized-types.hpp>
#include <bit>
#include <cstddef>
#include <cstring>
#include <span>

namespace engine::resources
{
	template <std::endian Endian, class T>
	void deserialize(std::span<const std::byte>& data, T& value)
	{
		if (data.size() < sizeof(T))
		{
			throw deserialize_error("Read out of range.");
		}

		std::memcpy(&value, data.data(), sizeof(T));

		if constexpr (Endian != std::endian::native)
		{
			if constexpr (sizeof(T) == sizeof(u16))
			{
				value = std::bit_cast<T>(std::byteswap(std::bit_cast<u16>(value)));
			}
			else if constexpr (sizeof(T) == sizeof(u32))
			{
				value = std::bit_cast<T>(std::byteswap(std::bit_cast<u32>(value)));
			}
			else if constexpr (sizeof(T) == sizeof(u64))
			{
				value = std::bit_cast<T>(std::byteswap(std::bit_cast<u64>(value)));
			}
		}

		data = data.subspan(sizeof(T));
	}

	template <class T>
	inline void deserialize_le(std::span<const std::byte>& data, T& value)
	{
		deserialize<std::endian::little>(data, value);
	}

	template <class T>
	inline void deserialize_be(std::span<const std::byte>& data, T& value)
	{
		deserialize<std::endian::big>(data, value);
	}
}
