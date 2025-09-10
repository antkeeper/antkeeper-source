// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <nlohmann/json.hpp>
#include <engine/utility/json.hpp>
#include <engine/i18n/string-map.hpp>
#include <engine/hash/fnv.hpp>
#include <engine/resources/serializer.hpp>
#include <engine/resources/serialize-error.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/utility/sized-types.hpp>
#include <utility>

namespace engine::resources
{
	/// Serializes a string map.
	/// @param[in] map String map to serialize.
	/// @param[in,out] ctx Serialize context.
	/// @throw serialize_error Write error.
	template <>
	void serializer<i18n::string_map>::serialize(const i18n::string_map& map, serialize_context& ctx)
	{
		// Write number of entries
		u32 size = static_cast<u32>(map.size());
		ctx.write32<std::endian::little>(reinterpret_cast<const std::byte*>(&size), 1);

		// Write entries
		for (const auto& [key, value] : map)
		{
			// Write key
			ctx.write32<std::endian::little>(reinterpret_cast<const std::byte*>(&key), 1);

			// Write string length
			u32 length = static_cast<u32>(value.length());
			ctx.write32<std::endian::little>(reinterpret_cast<const std::byte*>(&length), 1);

			// Write string
			ctx.write8(reinterpret_cast<const std::byte*>(value.data()), length);
		}
	}

	/// Deserializes a string map.
	/// @param[out] map String map to deserialize.
	/// @param[in,out] ctx Deserialize context.
	/// @throw deserialize_error Read error.
	template <>
	void deserializer<i18n::string_map>::deserialize(i18n::string_map& map, deserialize_context& ctx)
	{
		map.clear();

		if (ctx.path().extension() == ".json")
		{
			// Read file into buffer
			std::string file_buffer(ctx.size(), '\0');
			ctx.read8(reinterpret_cast<std::byte*>(file_buffer.data()), ctx.size());

			// Parse JSON from file buffer
			const auto json = nlohmann::json::parse(file_buffer, nullptr, true, true);

			// Map key hashes to string values
			for (const auto& element : json.items())
			{
				if (!element.value().is_string() || element.value().get_ref<const nlohmann::json::string_t&>().empty())
				{
					map[hash::fnv1a32<char>(element.key())] = '$' + element.key();
				}
				else
				{
					map[hash::fnv1a32<char>(element.key())] = element.value();
				}
			}
		}
		else
		{
			// Read number of entries
			u32 size = 0;
			ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&size), 1);

			// Read entries
			for (u32 i = 0; i < size; ++i)
			{
				// Read key
				hash::fnv32_t key;
				ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&key), 1);

				// Read string length
				u32 length = 0;
				ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&length), 1);

				// Insert empty string into map
				auto [iterator, inserted] = map.emplace
				(
					std::piecewise_construct,
					std::forward_as_tuple(key),
					std::forward_as_tuple(static_cast<usize>(length), '\0')
				);

				// Read string
				ctx.read8(reinterpret_cast<std::byte*>(iterator->second.data()), length);
			}
		}
	}

	template <>
	std::unique_ptr<i18n::string_map> resource_loader<i18n::string_map>::load(resource_manager&, std::shared_ptr<deserialize_context> ctx)
	{
		auto resource = std::make_unique<i18n::string_map>();

		deserializer<i18n::string_map>().deserialize(*resource, *ctx);

		return resource;
	}
}
