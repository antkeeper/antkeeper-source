/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_STRING_TABLE_HPP
#define ANTKEEPER_STRING_TABLE_HPP

#include <string>
#include <unordered_map>
#include <vector>

/**
 * A single row in a string table.
 */
typedef std::vector<std::string> string_table_row;

/**
 * A table of strings.
 */
typedef std::vector<string_table_row> string_table;

/**
 * An index for finding elements in a string table.
 */
typedef std::unordered_map<std::string, std::size_t> string_table_index;

typedef std::unordered_map<std::string, std::unordered_map<std::string, std::string>> string_table_map;

void build_string_table_map(string_table_map* map, const string_table& table);

/**
 * Creates an index for a string table using strings in the first column as keys.
 *
 * @param table Table for which an index will be created.
 */
string_table_index index_string_table(const string_table& table);

#endif // ANTKEEPER_STRING_TABLE_HPP

