/*
 * Copyright (C) 2017-2019  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "resource-loader.hpp"
#include "string-table.hpp"

static StringTableRow parseRow(const std::string& line)
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
StringTable* ResourceLoader<StringTable>::load(ResourceManager* resourceManager, std::istream* is)
{
	StringTable* table = new StringTable();
	std::string line;

	while (!is->eof())
	{
		std::getline(*is, line);
		if (is->bad() || is->fail())
		{
			break;
		}

		table->push_back(parseRow(line));
	}

	return table;
}

template <>
void ResourceLoader<StringTable>::save(ResourceManager* resourceManager, std::ostream* os, const StringTable* table)
{
	for (std::size_t i = 0; i < table->size(); ++i)
	{
		const StringTableRow& row = (*table)[i];

		for (std::size_t j = 0; j < row.size(); ++j)
		{
			const std::string& column = row[j];

			(*os) << column;

			if (j < row.size() - 1)
			{
				(*os) << ",";
			}
		}

		if (i < table->size() - 1)
		{
			(*os) << std::endl;
		}
	}
}

