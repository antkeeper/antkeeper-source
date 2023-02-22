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

#include <engine/debug/console.hpp>

#if defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif

namespace debug {
namespace console {

void enable_vt100()
{
	#if defined(_WIN32)
		DWORD mode = 0;
		HANDLE std_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleMode(std_output_handle, &mode);
		SetConsoleMode(std_output_handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	#endif
}

void disable_vt100()
{
	#if defined(_WIN32)
		DWORD mode = 0;
		HANDLE std_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleMode(std_output_handle, &mode);
		SetConsoleMode(std_output_handle, mode & (~ENABLE_VIRTUAL_TERMINAL_PROCESSING));
	#endif
}

void enable_utf8()
{
	#if defined(_WIN32)
		SetConsoleOutputCP(CP_UTF8);
	#endif
}

} // namespace console
} // namespace debug
