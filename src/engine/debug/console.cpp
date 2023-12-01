// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
