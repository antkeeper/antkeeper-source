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

#ifndef STRING_TABLE_HPP
#define STRING_TABLE_HPP

#include <string>
#include <unordered_map>
#include <vector>

/**
 * A single row in a string table.
 */
typedef std::vector<std::string> StringTableRow;

/**
 * A table of strings.
 */
typedef std::vector<StringTableRow> StringTable;

/**
 * An index for finding elements in a string table.
 */
typedef std::unordered_map<std::string, std::size_t> StringTableIndex;

/**
 * Creates an index for a string table using strings in the first column as keys.
 *
 * @param table Table for which an index will be created.
 */
StringTableIndex createIndex(const StringTable& table);

#endif // STRING_TABLE_HPP

