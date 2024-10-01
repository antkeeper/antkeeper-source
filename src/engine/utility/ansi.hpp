// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_UTILITY_ANSI_HPP
#define ANTKEEPER_UTILITY_ANSI_HPP

/// ANSI escape sequences.
/// @see https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
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
