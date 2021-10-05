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

#ifndef ANTKEEPER_TYPE_UNICODE_CONVERT_HPP
#define ANTKEEPER_TYPE_UNICODE_CONVERT_HPP

#include <string>

namespace type {
namespace unicode {

/**
 * Converts a UTF-8 string to UTF-32.
 *
 * @param u8 UTF-8 string.
 * @return UTF-32 string.
 */
std::u32string u32(const std::string& u8);

/**
 * Converts a UTF-32 string to UTF-8.
 *
 * @param u32 UTF-32 string.
 * @return UTF-8 string.
 */
std::string u32(const std::u32string& u32);

} // namespace unicode
} // namespace type

#endif // ANTKEEPER_TYPE_UNICODE_CONVERT_HPP
