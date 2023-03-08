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

#include <engine/utility/text-file.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/resources/resource-loader.hpp>

/**
 * Deserializes a text file.
 *
 * @param[out] file Text file to deserialize.
 * @param[in,out] ctx Deserialize context.
 *
 * @throw deserialize_error Read error.
 */
template <>
void deserializer<text_file>::deserialize(text_file& file, deserialize_context& ctx)
{
	file.lines.resize(1);
	std::string* line = &file.lines.front();
	line->clear();
	
	char c;
	while (ctx.read8(reinterpret_cast<std::byte*>(&c), 1) == 1)
	{
		if (c == '\n')
		{
			line = &file.lines.emplace_back(std::string());
		}
		else if (c != '\r')
		{
			line->append(1, c);
		}
	}
	
	if (line->empty())
	{
		file.lines.pop_back();
	}
}

template <>
std::unique_ptr<text_file> resource_loader<text_file>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	auto resource = std::make_unique<text_file>();
	
	deserializer<text_file>().deserialize(*resource, ctx);
	
	return resource;
}
