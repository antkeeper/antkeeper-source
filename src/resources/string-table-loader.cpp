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

#include "resource-loader.hpp"
#include "string-table.hpp"
#include <physfs.h>

static string_table_row parse_row(const std::string& line)
{
	std::vector<std::string> row;
	std::string column;
	bool quoted = false;
	bool escape = false;

	for (char c: line)
	{
		if (escape)
		{
			switch (c)
			{
				case 'n':
					column.push_back('\n');
					break;

				case 't':
					column.push_back('\t');
					break;

				default:
					column.push_back(c);
					break;
			}

			escape = false;
		}
		else
		{
			switch (c)
			{
				case '\\':
					escape = true;
					break;

				case ',':
					if (quoted)
					{
						column.push_back(c);
					}
					else
					{
						row.push_back(column);
						column.clear();
					}
					break;

				case '"':
					if (!quoted)
					{
						quoted = true;
					}
					else
					{
						quoted = false;
					}
					break;

				default:
					column.push_back(c);
					break;
			}
		}
	}

	row.push_back(column);

	return row;
}

template <>
string_table* resource_loader<string_table>::load(resource_manager* resource_manager, PHYSFS_File* file)
{
	string_table* table = new string_table();
	std::string line;

	while (!PHYSFS_eof(file))
	{
		physfs_getline(file, line);
		table->push_back(parse_row(line));
	}

	return table;
}

template <>
void resource_loader<string_table>::save(resource_manager* resource_manager, PHYSFS_File* file, const string_table* table)
{
	const char* delimeter = ",";
	const char* newline = "\n";
	
	for (std::size_t i = 0; i < table->size(); ++i)
	{
		const string_table_row& row = (*table)[i];

		for (std::size_t j = 0; j < row.size(); ++j)
		{
			const std::string& column = row[j];
			
			PHYSFS_writeBytes(file, column.data(), column.length());

			if (j < row.size() - 1)
			{
				PHYSFS_writeBytes(file, delimeter, 1);
			}
		}

		if (i < table->size() - 1)
		{
			PHYSFS_writeBytes(file, newline, 1);
		}
	}
}
