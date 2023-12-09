// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/utility/dict.hpp>
#include <engine/resources/serializer.hpp>
#include <engine/resources/serialize-error.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/hash/fnv1a.hpp>
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
 * Serializes a dict with a 32-bit FNV-1a hash key.
 *
 * @param[in] dict Dict to serialize.
 * @param[in,out] ctx Serialize context.
 *
 * @throw serialize_error Write error.
 * @throw serialize_error Unsupported dict value type.
 */
template <>
void serializer<dict<hash::fnv1a32_t>>::serialize(const dict<hash::fnv1a32_t>& dict, serialize_context& ctx)
{
	// Map type indices to tuples containing a type hash and serialize function pointer
	static const std::unordered_map
	<
		std::type_index,
		std::tuple
		<
			hash::fnv1a32_t,
			void (*)(const std::any&, serialize_context&)
		>
	> type_map
	{
		{std::type_index(typeid(bool)), {"bool", &serialize_any<bool>}},
		{std::type_index(typeid(std::uint8_t)), {"uint8", &serialize_any<std::uint8_t>}},
		{std::type_index(typeid(std::uint16_t)), {"uint16", &serialize_any<std::uint16_t>}},
		{std::type_index(typeid(std::uint32_t)), {"uint32", &serialize_any<std::uint32_t>}},
		{std::type_index(typeid(std::uint64_t)), {"uint64", &serialize_any<std::uint64_t>}},
		{std::type_index(typeid(std::int8_t)), {"int8", &serialize_any<std::int8_t>}},
		{std::type_index(typeid(std::int16_t)), {"int16", &serialize_any<std::int16_t>}},
		{std::type_index(typeid(std::int32_t)), {"int32", &serialize_any<std::int32_t>}},
		{std::type_index(typeid(std::int64_t)), {"int64", &serialize_any<std::int64_t>}},
		{std::type_index(typeid(float)), {"float", &serialize_any<float>}},
		{std::type_index(typeid(double)), {"double", &serialize_any<double>}},
		{std::type_index(typeid(std::string)), {"string", &serialize_any<std::string>}},
		{std::type_index(typeid(std::u8string)), {"u8string", &serialize_any<std::u8string>}},
		{std::type_index(typeid(std::u16string)), {"u16string", &serialize_any<std::u16string>}},
		{std::type_index(typeid(std::u32string)), {"u32string", &serialize_any<std::u32string>}}
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
 * Deserializes a dict with a 32-bit FNV-1a hash key.
 *
 * @param[out] dict Dict to deserialize.
 * @param[in,out] ctx Deserialize context.
 *
 * @throw deserialize_error Read error.
 * @throw deserialize_error Unsupported dict value type.
 */
template <>
void deserializer<dict<hash::fnv1a32_t>>::deserialize(dict<hash::fnv1a32_t>& dict, deserialize_context& ctx)
{
	// Map type hashes to deserialize function pointers
	static const std::unordered_map
	<
		hash::fnv1a32_t,
		void (*)(std::any&, deserialize_context&)
	> type_map
	{
		{"bool", &deserialize_any<bool>},
		{"uint8", &deserialize_any<std::uint8_t>},
		{"uint16", &deserialize_any<std::uint16_t>},
		{"uint32", &deserialize_any<std::uint32_t>},
		{"uint64", &deserialize_any<std::uint64_t>},
		{"int8", &deserialize_any<std::int8_t>},
		{"int16", &deserialize_any<std::int16_t>},
		{"int32", &deserialize_any<std::int32_t>},
		{"int64", &deserialize_any<std::int64_t>},
		{"float", &deserialize_any<float>},
		{"double", &deserialize_any<double>},
		{"string", &deserialize_any<std::string>},
		{"u8string", &deserialize_any<std::u8string>},
		{"u16string", &deserialize_any<std::u16string>},
		{"u32string", &deserialize_any<std::u32string>}
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
			hash::fnv1a32_t key;
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

template <>
std::unique_ptr<dict<hash::fnv1a32_t>> resource_loader<dict<hash::fnv1a32_t>>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	auto resource = std::make_unique<dict<hash::fnv1a32_t>>();
	
	deserializer<dict<hash::fnv1a32_t>>().deserialize(*resource, ctx);
	
	return resource;
}
