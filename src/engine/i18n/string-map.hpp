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

#include <string>
#include <unordered_map>
#include <engine/utility/hash/fnv1a.hpp>

namespace i18n {

/**
 * Maps 32-bit keys to strings.
 */
typedef std::unordered_map<hash::fnv1a32_t, std::string> string_map;

} // namespace i18n

#endif // ANTKEEPER_I18N_STRING_MAP_HPP
