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

#ifndef ANTKEEPER_UTILITY_ANSI_HPP
#define ANTKEEPER_UTILITY_ANSI_HPP

/**
 * ANSI escape sequences.
 *
 * @see https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
 */
namespace ansi {

const char* reset             = "\33[0m";
const char* bold              = "\33[1m";
const char* underline         = "\33[4m";
const char* invert            = "\33[7m";
const char* no_bold           = "\33[22m";
const char* no_underline      = "\33[24m";
const char* no_invert         = "\33[24m";
const char* fg_black          = "\33[30m";
const char* fg_red            = "\33[31m";
const char* fg_green          = "\33[32m";
const char* fg_yellow         = "\33[33m";
const char* fg_blue           = "\33[34m";
const char* fg_magenta        = "\33[35m";
const char* fg_cyan           = "\33[36m";
const char* fg_white          = "\33[37m";
const char* fg_extended       = "\33[38m";
const char* fg_reset          = "\33[39m";
const char* bg_black          = "\33[40m";
const char* bg_red            = "\33[41m";
const char* bg_green          = "\33[42m";
const char* bg_yellow         = "\33[43m";
const char* bg_blue           = "\33[44m";
const char* bg_magenta        = "\33[45m";
const char* bg_cyan           = "\33[46m";
const char* bg_white          = "\33[47m";
const char* bg_extended       = "\33[48m";
const char* bg_reset          = "\33[49m";
const char* fg_bright_black   = "\33[30;1m";
const char* fg_bright_red     = "\33[31;1m";
const char* fg_bright_green   = "\33[32;1m";
const char* fg_bright_yellow  = "\33[33;1m";
const char* fg_bright_blue    = "\33[34;1m";
const char* fg_bright_magenta = "\33[35;1m";
const char* fg_bright_cyan    = "\33[36;1m";
const char* fg_bright_white   = "\33[37;1m";
const char* bg_bright_black   = "\33[40;1m";
const char* bg_bright_red     = "\33[41;1m";
const char* bg_bright_green   = "\33[42;1m";
const char* bg_bright_yellow  = "\33[43;1m";
const char* bg_bright_blue    = "\33[44;1m";
const char* bg_bright_magenta = "\33[45;1m";
const char* bg_bright_cyan    = "\33[46;1m";
const char* bg_bright_white   = "\33[47;1m";

} // namespace ansi

#endif // ANTKEEPER_UTILITY_ANSI_HPP
