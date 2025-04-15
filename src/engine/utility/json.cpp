// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <nlohmann/json.hpp>
import engine.utility.json;
import engine.resources.serializer;
import engine.resources.deserializer;
import engine.resources.resource_loader;

namespace engine::resources
{
	template <>
	void serializer<json>::serialize(const json& element, serialize_context& ctx)
	{
		// Dump JSON to string
		auto dump = element.dump(1, '\t', false);

		// Write string
		ctx.write8(reinterpret_cast<std::byte*>(dump.data()), dump.length());
	}

	template <>
	void deserializer<json>::deserialize(json& element, deserialize_context& ctx)
	{
		// Read file into buffer
		std::string file_buffer(ctx.size(), '\0');
		ctx.read8(reinterpret_cast<std::byte*>(file_buffer.data()), ctx.size());

		// Parse JSON from file buffer
		element = nlohmann::json::parse(file_buffer, nullptr, true, true);
	}

	template <>
	std::unique_ptr<json> resource_loader<json>::load(resource_manager&, std::shared_ptr<deserialize_context> ctx)
	{
		auto resource = std::make_unique<json>();

		deserializer<json>().deserialize(*resource, *ctx);

		return resource;
	}
}
