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

#include <engine/i18n/string-table.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/resources/resource-loader.hpp>

/**
 * Deserializes a string table.
 *
 * @param[out] file Text file to serialize.
 * @param[in,out] ctx Deserialize context.
 *
 * @throw deserialize_error Read error.
 */
template <>
void deserializer<i18n::string_table>::deserialize(i18n::string_table& table, deserialize_context& ctx)
{
	table.rows.clear();
	
	std::vector<std::string> row;
	std::string entry;
	
	char c;
	while (ctx.read8(reinterpret_cast<std::byte*>(&c), 1) == 1)
	{
		if (c == '\t')
		{
			row.push_back(entry);
			entry.clear();
		}
		else if (c == '\n')
		{
			row.push_back(entry);
			entry.clear();
			table.rows.push_back(row);
			row.clear();
		}
		else if (c != '\r')
		{
			entry.push_back(c);
		}
	}
	
	if (!entry.empty())
	{
		row.push_back(entry);
	}
	
	if (!row.empty())
	{
		table.rows.push_back(row);
	}
}

template <>
std::unique_ptr<i18n::string_table> resource_loader<i18n::string_table>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	auto resource = std::make_unique<i18n::string_table>();
	
	deserializer<i18n::string_table>().deserialize(*resource, ctx);
	
	return resource;
}
