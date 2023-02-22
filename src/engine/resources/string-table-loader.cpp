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

#include <engine/resources/resource-loader.hpp>
#include <engine/i18n/string-table.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <physfs.h>

template <>
i18n::string_table* resource_loader<i18n::string_table>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	i18n::string_table* table = new i18n::string_table();
	
	i18n::string_table_row row;
	std::string entry;
	
	for (;;)
	{
		char c;
		const PHYSFS_sint64 status = PHYSFS_readBytes(file, &c, 1);
		
		if (status == 1)
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
				table->push_back(row);
				row.clear();
			}
			else if (c != '\r')
			{
				entry.push_back(c);
			}
		}
		else
		{
			if (PHYSFS_eof(file))
			{
				if (!entry.empty())
				{
					row.push_back(entry);
				}
				if (!row.empty())
				{
					table->push_back(row);
				}
				break;
			}
			else
			{
				throw deserialize_error(PHYSFS_getLastError());
			}
		}
	}

	return table;
}
