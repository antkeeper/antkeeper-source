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
std::unique_ptr<json> resource_loader<json>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	auto resource = std::make_unique<json>();
	
	deserializer<json>().deserialize(*resource, ctx);
	
	return resource;
}
