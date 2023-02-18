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

#include "i18n/string-map.hpp"
#include "utility/hash/fnv1a.hpp"
#include "resources/serializer.hpp"
#include "resources/serialize-error.hpp"
#include "resources/deserializer.hpp"
#include "resources/deserialize-error.hpp"
#include <algorithm>

/**
 * Serializes a string map.
 *
 * @param[in] map String map to serialize.
 * @param[in,out] ctx Serialize context.
 *
 * @throw serialize_error Write error.
 */
template <>
void serializer<i18n::string_map>::serialize(const i18n::string_map& map, serialize_context& ctx)
{
	// Write number of entries
	std::uint32_t size = static_cast<std::uint32_t>(map.size());
	ctx.write32<std::endian::big>(reinterpret_cast<const std::byte*>(&size), 1);
	
	// Write entries
	for (const auto& [key, value]: map)
	{
		// Write key
		ctx.write32<std::endian::big>(reinterpret_cast<const std::byte*>(&key), 1);
		
		// Write string length
		std::uint32_t length = static_cast<std::uint32_t>(value.length());
		ctx.write32<std::endian::big>(reinterpret_cast<const std::byte*>(&length), 1);
		
		// Write string
		ctx.write8(reinterpret_cast<const std::byte*>(value.data()), length);
	}
}

/**
 * Deserializes a string map.
 *
 * @param[out] map String map to deserialize.
 * @param[in,out] ctx Deserialize context.
 *
 * @throw deserialize_error Read error.
 */
template <>
void deserializer<i18n::string_map>::deserialize(i18n::string_map& map, deserialize_context& ctx)
{
	map.clear();
	
	// Read number of entries
	std::uint32_t size = 0;
	ctx.read32<std::endian::big>(reinterpret_cast<std::byte*>(&size), 1);
	
	// Read entries
	for (std::uint32_t i = 0; i < size; ++i)
	{
		// Read key
		std::uint32_t key = 0;
		ctx.read32<std::endian::big>(reinterpret_cast<std::byte*>(&key), 1);
		
		// Read string length
		std::uint32_t length = 0;
		ctx.read32<std::endian::big>(reinterpret_cast<std::byte*>(&length), 1);
		
		// Insert empty string into map
		auto [iterator, inserted] = map.emplace
		(
			std::piecewise_construct,
			std::forward_as_tuple(key),
			std::forward_as_tuple(static_cast<std::size_t>(length), '\0')
		);
		
		// Read string
		ctx.read8(reinterpret_cast<std::byte*>(iterator->second.data()), length);
	}
}
