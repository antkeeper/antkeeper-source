/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "resources/string-table.hpp"

void build_string_table_map(string_table_map* map, const string_table& table)
{
	map->clear();
	
	for (std::size_t i = 0; i < table.size(); ++i)
	{
		for (std::size_t j = 2; j < table[i].size(); ++j)
		{
			const std::string& string = table[i][j];
			(*map)[table[0][j]][table[i][0]] = string.empty() ? "# MISSING STRING #" : string;
		}
	}
}

string_table_index index_string_table(const string_table& table)
{
	string_table_index index;

	for (std::size_t i = 0; i < table.size(); ++i)
	{
		index[table[i][0]] = i;
	}
	
	return index;
}
