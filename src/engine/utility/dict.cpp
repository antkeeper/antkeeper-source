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

#include <engine/utility/dict.hpp>
#include <engine/resources/serializer.hpp>
#include <engine/resources/serialize-error.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <engine/utility/hash/fnv1a.hpp>
#include <cstdint>
#include <string>
#include <tuple>
#include <typeindex>
#include <unordered_map>

using namespace hash::literals;

template <class T>
static void serialize_any(const std::any& any, serialize_context& ctx)
{
	serializer<T>().serialize(std::any_cast<T>(any), ctx);
}

template <class T>
static void deserialize_any(std::any& any, deserialize_context& ctx)
{
	T value;
	deserializer<T>().deserialize(value, ctx);
	any = std::move(value);
}

/**
 * Serializes a dict with an unsigned 32-bit key.
 *
 * @param[in] dict Dict to serialize.
 * @param[in,out] ctx Serialize context.
 *
 * @throw serialize_error Write error.
 * @throw serialize_error Unsupported dict value type.
 */
template <>
void serializer<dict<std::uint32_t>>::serialize(const dict<std::uint32_t>& dict, serialize_context& ctx)
{
	// Map type indices to tuples containing a type hash and serialize function pointer
	static const std::unordered_map
	<
		std::type_index,
		std::tuple
		<
			std::uint32_t,
			void (*)(const std::any&, serialize_context&)
		>
	> type_map
	{
		{std::type_index(typeid(bool)), {"bool"_fnv1a32, &serialize_any<bool>}},
		{std::type_index(typeid(std::uint8_t)), {"uint8"_fnv1a32, &serialize_any<std::uint8_t>}},
		{std::type_index(typeid(std::uint16_t)), {"uint16"_fnv1a32, &serialize_any<std::uint16_t>}},
		{std::type_index(typeid(std::uint32_t)), {"uint32"_fnv1a32, &serialize_any<std::uint32_t>}},
		{std::type_index(typeid(std::uint64_t)), {"uint64"_fnv1a32, &serialize_any<std::uint64_t>}},
		{std::type_index(typeid(std::int8_t)), {"int8"_fnv1a32, &serialize_any<std::int8_t>}},
		{std::type_index(typeid(std::int16_t)), {"int16"_fnv1a32, &serialize_any<std::int16_t>}},
		{std::type_index(typeid(std::int32_t)), {"int32"_fnv1a32, &serialize_any<std::int32_t>}},
		{std::type_index(typeid(std::int64_t)), {"int64"_fnv1a32, &serialize_any<std::int64_t>}},
		{std::type_index(typeid(float)), {"float"_fnv1a32, &serialize_any<float>}},
		{std::type_index(typeid(double)), {"double"_fnv1a32, &serialize_any<double>}},
		{std::type_index(typeid(std::string)), {"string"_fnv1a32, &serialize_any<std::string>}},
		{std::type_index(typeid(std::u8string)), {"u8string"_fnv1a32, &serialize_any<std::u8string>}},
		{std::type_index(typeid(std::u16string)), {"u16string"_fnv1a32, &serialize_any<std::u16string>}},
		{std::type_index(typeid(std::u32string)), {"u32string"_fnv1a32, &serialize_any<std::u32string>}}
	};
	
	// Write dict size
	std::uint64_t size = static_cast<std::uint64_t>(dict.size());
	ctx.write64<std::endian::big>(reinterpret_cast<const std::byte*>(&size), 1);
	
	// Write dict entries
	for (const auto& [key, value]: dict)
	{
		if (auto i = type_map.find(value.type()); i != type_map.end())
		{
			const auto& [type_hash, type_serializer] = i->second;
			
			// Write entry type hash and key
			ctx.write32<std::endian::big>(reinterpret_cast<const std::byte*>(&type_hash), 1);
			ctx.write32<std::endian::big>(reinterpret_cast<const std::byte*>(&key), 1);
			
			// Serialize entry value
			type_serializer(value, ctx);
		}
		else
		{
			throw serialize_error("Unsupported dict value type");
		}
	}
}

/**
 * Deserializes a dict with an unsigned 32-bit key.
 *
 * @param[out] dict Dict to serialize.
 * @param[in,out] ctx Deserialize context.
 *
 * @throw deserialize_error Read error.
 * @throw deserialize_error Unsupported dict value type.
 */
template <>
void deserializer<dict<std::uint32_t>>::deserialize(dict<std::uint32_t>& dict, deserialize_context& ctx)
{
	// Map type hashes to deserialize function pointers
	static const std::unordered_map
	<
		std::uint32_t,
		void (*)(std::any&, deserialize_context&)
	> type_map
	{
		{"bool"_fnv1a32, &deserialize_any<bool>},
		{"uint8"_fnv1a32, &deserialize_any<std::uint8_t>},
		{"uint16"_fnv1a32, &deserialize_any<std::uint16_t>},
		{"uint32"_fnv1a32, &deserialize_any<std::uint32_t>},
		{"uint64"_fnv1a32, &deserialize_any<std::uint64_t>},
		{"int8"_fnv1a32, &deserialize_any<std::int8_t>},
		{"int16"_fnv1a32, &deserialize_any<std::int16_t>},
		{"int32"_fnv1a32, &deserialize_any<std::int32_t>},
		{"int64"_fnv1a32, &deserialize_any<std::int64_t>},
		{"float"_fnv1a32, &deserialize_any<float>},
		{"double"_fnv1a32, &deserialize_any<double>},
		{"string"_fnv1a32, &deserialize_any<std::string>},
		{"u8string"_fnv1a32, &deserialize_any<std::u8string>},
		{"u16string"_fnv1a32, &deserialize_any<std::u16string>},
		{"u32string"_fnv1a32, &deserialize_any<std::u32string>}
	};
	
	dict.clear();
	
	// Read dict size
	std::uint64_t size = 0;
	ctx.read64<std::endian::big>(reinterpret_cast<std::byte*>(&size), 1);
	
	// Read dict entries
	for (std::size_t i = 0; i < size; ++i)
	{
		// Read entry type hash
		std::uint32_t type_hash = 0;
		ctx.read32<std::endian::big>(reinterpret_cast<std::byte*>(&type_hash), 1);
		
		if (auto i = type_map.find(type_hash); i != type_map.end())
		{
			// Read entry key
			std::uint32_t key = 0;
			ctx.read32<std::endian::big>(reinterpret_cast<std::byte*>(&key), 1);
			
			// Deserialize entry value
			i->second(dict[key], ctx);
		}
		else
		{
			throw deserialize_error("Unsupported dict value type");
		}
	}
}
