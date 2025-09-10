// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/debug/console-log.hpp>
#include <engine/debug/contract.hpp>
#include <engine/debug/logger.hpp>
#include <engine/debug/log.hpp>
#include <engine/math/functions.hpp>

#if defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif
#include <iostream>
#include <syncstream>
#include <filesystem>
#include <format>

namespace
{
	/// Maps log message severity to ANSI color codes.
	const char* console_log_colors[] =
	{
		"\33[37m",         // trace:   FG_WHITE
		"\33[34;1m",       // debug:   FG_BRIGHT_BLUE
		"\33[32;1m",       // info:    FG_BRIGHT_GREEN
		"\33[33m",         // warning: FG_YELLOW
		"\33[31m",         // error:   FG_RED
		"\33[37;\33[41;1m" // fatal:   FG_WHITE, BG_BRIGHT_RED
	};

	/// ANSI color reset code.
	const char* ansi_reset = "\33[0m";

	#if defined(_WIN32)
		
		/// Enables UTF-8 output.
		void enable_utf8()
		{
			SetConsoleOutputCP(CP_UTF8);
		}

		/// Enables VT100 virtual terminal sequences.
		void enable_vt100()
		{
			DWORD mode = 0;
			HANDLE std_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
			GetConsoleMode(std_output_handle, &mode);
			SetConsoleMode(std_output_handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		}

		/// Disables VT100 virtual terminal sequences.
		void disable_vt100()
		{
			DWORD mode = 0;
			HANDLE std_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
			GetConsoleMode(std_output_handle, &mode);
			SetConsoleMode(std_output_handle, mode & (~ENABLE_VIRTUAL_TERMINAL_PROCESSING));
		}

	#endif
}

namespace engine::debug
{
	console_log::console_log()
	{
		// Get current time zone
		m_time_zone = std::chrono::current_zone();

		// Subscribe to log messages from default logger
		m_message_logged_subscription = default_logger().message_logged_channel().subscribe
		(
			[this](const auto& event)
			{
				this->message_logged(event);
			}
		);

		// Enable UTF-8 output and VT100 sequences on Windows
		#if defined(_WIN32)
			enable_utf8();
			enable_vt100();
		#endif

		debug::postcondition(m_time_zone);
	}

	console_log::~console_log()
	{}

	void console_log::message_logged(const message_logged_event& event)
	{
		debug::precondition(m_time_zone);

		// Clamp severity index to valid range
		const auto severity_index = std::to_underlying(math::clamp(event.severity, log_message_severity::trace, log_message_severity::fatal));

		// Round time to the millisecond and convert to local time zone
		const std::chrono::zoned_time zoned_time{m_time_zone, std::chrono::floor<std::chrono::milliseconds>(event.time)};
	
		// Select and synchronize output stream based on severity
		std::osyncstream output_stream(event.severity >= log_message_severity::error ? std::cerr : std::cout);

		// Format and output log message
		output_stream << std::format
		(
			"[{:%T}] {}{:7}: {}:{}: {}\33[0m\n",
			zoned_time,
			console_log_colors[severity_index],
			log_message_severity_to_string(event.severity),
			std::filesystem::path(event.location.file_name()).filename().string(),
			event.location.line(),
			event.message
		);
	}
}
