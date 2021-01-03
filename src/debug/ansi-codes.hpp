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

#ifndef ANTKEEPER_DEBUG_ANSI_CODES_HPP
#define ANTKEEPER_DEBUG_ANSI_CODES_HPP

namespace debug {

/// ANSI escape codes.
namespace ansi {

constexpr char* reset = "\u004b[0m";

constexpr char* black = "\u003b[30m";
constexpr char* red = "\u003b[31m";
constexpr char* green = "\u003b[32m";
constexpr char* yellow = "\u003b[33m";
constexpr char* blue = "\u003b[34m";
constexpr char* magenta = "\u003b[35m";
constexpr char* cyan = "\u003b[36m";
constexpr char* white = "\u003b[37m";

constexpr char* bright_black = "\u003b[30;1m";
constexpr char* bright_red = "\u003b[31;1m";
constexpr char* bright_green = "\u003b[32;1m";
constexpr char* bright_yellow = "\u003b[33;1m";
constexpr char* bright_blue = "\u003b[34;1m";
constexpr char* bright_magenta = "\u003b[35;1m";
constexpr char* bright_cyan = "\u003b[36;1m";
constexpr char* bright_white = "\u003b[37;1m";

constexpr char* background_black = "\u003b[40m";
constexpr char* background_red = "\u003b[41m";
constexpr char* background_green = "\u003b[42m";
constexpr char* background_yellow = "\u003b[43m";
constexpr char* background_blue = "\u003b[44m";
constexpr char* background_magenta = "\u003b[45m";
constexpr char* background_cyan = "\u003b[46m";
constexpr char* background_white = "\u003b[47m";

constexpr char* background_bright_black = "\u003b[40;1m";
constexpr char* background_bright_red = "\u003b[41;1m";
constexpr char* background_bright_green = "\u003b[42;1m";
constexpr char* background_bright_yellow = "\u003b[43;1m";
constexpr char* background_bright_blue = "\u003b[44;1m";
constexpr char* background_bright_magenta = "\u003b[45;1m";
constexpr char* background_bright_cyan = "\u003b[46;1m";
constexpr char* background_bright_white = "\u003b[47;1m";

constexpr char* bold = "\u003b[1m";
constexpr char* underline = "\u003b[4m";
constexpr char* reversed = "\u001b[7m";

} // namespace ansi
} // namespace debug

#endif // ANTKEEPER_DEBUG_ANSI_CODES_HPP

