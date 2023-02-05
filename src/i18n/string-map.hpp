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

#ifndef ANTKEEPER_I18N_STRING_MAP_HPP
#define ANTKEEPER_I18N_STRING_MAP_HPP

#include "i18n/string-table.hpp"
#include <string>
#include <unordered_map>

namespace i18n {

/**
 * Maps 32-bit keys to strings.
 */
typedef std::unordered_map<std::uint32_t, std::string> string_map;

/**
 * Builds a string map from a string table. Keys are generated with the 32-bit FNV-1a hash function.
 *
 * @param[in] table String table from which the string map will be built.
 * @param[in] key_column Column containing key strings.
 * @param[in] value_column Column containing value strings.
 * @param[out] String map to build.
 */
void build_string_map(const string_table& table, std::size_t key_column, std::size_t value_column, string_map& map);

} // namespace i18n

#endif // ANTKEEPER_I18N_STRING_MAP_HPP
