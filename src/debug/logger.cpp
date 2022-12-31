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

#include "logger.hpp"
#include "utility/timestamp.hpp"
#include <iostream>

#if defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif

namespace debug {

logger::logger():
	os(&std::cout),
	auto_newline(true),
	timestamp_enabled(true),
	indent("|   "),
	log_prefix(std::string()),
	log_postfix(std::string()),
	warning_prefix(std::string()),
	warning_postfix(std::string()),
	error_prefix(std::string()),
	error_postfix(std::string()),
	success_prefix(std::string()),
	success_postfix(std::string())
{}

logger::~logger()
{}

void logger::redirect(std::ostream* stream)
{
	os = stream;
}

void logger::log(const std::string& text)
{
	if (os)
	{
		std::string message = "";
		
		// Prepend timestamp
		if (timestamp_enabled)
		{
			message += timestamp();
			message += ": ";
		}
		
		// Prepend indentation
		for (std::size_t i = 0; i < tasks.size(); ++i)
			message += indent;
		
		// Append text
		message += (log_prefix + text + log_postfix);
		
		// Append newline
		if (auto_newline)
			message += "\n";
		
		// Add message to log history
		const std::lock_guard<std::mutex> history_lock(history_mutex);
		history += message;
		
		// Output message
		(*os) << message;
		
		// Flush output stream
		os->flush();
	}
}

void logger::warning(const std::string& text)
{
	#if defined(_WIN32)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
	#endif
	
	log(warning_prefix + text + warning_postfix);
	
	#if defined(_WIN32)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	#endif
}

void logger::error(const std::string& text)
{
	#if defined(_WIN32)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	#endif
	
	log(error_prefix + text + error_postfix);
	
	#if defined(_WIN32)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	#endif
}

void logger::success(const std::string& text)
{
	/*
	#if defined(_WIN32)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	#endif
	*/
	
	log(success_prefix + text + success_postfix);
	
	/*
	#if defined(_WIN32)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	#endif
	*/
}

void logger::set_auto_newline(bool enabled)
{
	auto_newline = enabled;
}

void logger::set_timestamp(bool enabled)
{
	timestamp_enabled = enabled;
}

void logger::set_indent(const std::string& indent)
{
	this->indent = indent;
}

void logger::set_log_prefix(const std::string& prefix)
{
	log_prefix = prefix;
}

void logger::set_log_postfix(const std::string& postfix)
{
	log_postfix = postfix;
}

void logger::set_warning_prefix(const std::string& prefix)
{
	warning_prefix = prefix;
}

void logger::set_warning_postfix(const std::string& postfix)
{
	warning_postfix = postfix;
}

void logger::set_error_prefix(const std::string& prefix)
{
	error_prefix = prefix;
}

void logger::set_error_postfix(const std::string& postfix)
{
	error_postfix = postfix;
}

void logger::set_success_prefix(const std::string& prefix)
{
	success_prefix = prefix;
}

void logger::set_success_postfix(const std::string& postfix)
{
	success_postfix = postfix;
}

void logger::push_task(const std::string& description)
{
	std::string message = description + " {";
	if (!auto_newline)
		message += "\n";
	
	log(message);
	
	tasks.push(description);
}

void logger::pop_task(int status, std::string error)
{
	if (tasks.empty())
	{
		return;
	}
	
	std::string message = "} => ";
	
	tasks.pop();
	
	if (status == EXIT_SUCCESS)
	{
		message += "success";
		if (!auto_newline)
			message += "\n";
		
		success(message);
	}
	else
	{
		message += "failure";
		if (!error.empty())
			message += " (" + error + ")";
		if (!auto_newline)
			message += "\n";
		
		this->error(message);
	}
}

} // namespace debug
