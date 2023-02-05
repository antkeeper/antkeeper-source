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
#include <algorithm>

namespace i18n {

void build_string_map(const string_table& table, std::size_t key_column, std::size_t value_column, string_map& map)
{
	map.clear();
	
	const std::size_t max_column = std::max<std::size_t>(key_column, value_column);
	
	for (const auto& row: table)
	{
		if (row.size() <= max_column)
		{
			continue;
		}
		
		const auto& value_string = row[value_column];
		if (value_string.empty())
		{
			continue;
		}
		
		const auto& key_string = row[key_column];		
		const std::uint32_t key_hash = hash::fnv1a32(key_string.c_str(), key_string.length());	
		map[key_hash] = row[value_column];
	}
}

} // namespace i18n
