// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/utility/json.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/deserializer.hpp>

template <>
void deserializer<json>::deserialize(::json& json, deserialize_context& ctx)
{
	// Read file into buffer
	std::string file_buffer(ctx.size(), '\0');
	ctx.read8(reinterpret_cast<std::byte*>(file_buffer.data()), ctx.size());
	
	// Parse JSON from file buffer
	json = ::json::parse(file_buffer, nullptr, true, true);
}

template <>
std::unique_ptr<json> resource_loader<json>::load([[maybe_unused]] ::resource_manager& resource_manager, deserialize_context& ctx)
{
	auto resource = std::make_unique<json>();
	
	deserializer<json>().deserialize(*resource, ctx);
	
	return resource;
}
