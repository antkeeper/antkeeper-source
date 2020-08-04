/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_LOGGER_HPP
#define ANTKEEPER_LOGGER_HPP

#include <list>
#include <ostream>
#include <stack>
#include <string>

class logger
{
public:
	logger();
	~logger();

	/**
	 * Redirects log output to the specified output stream.
	 *
	 * @param stream Output stream to which log text will be written.
	 */
	void redirect(std::ostream* stream);
	/**
	 * Outputs text to the log.
	 */
	void log(const std::string& text);

	void warning(const std::string& text);
	void error(const std::string& text);
	void success(const std::string& text);

	void set_log_prefix(const std::string& prefix);
	void set_log_postfix(const std::string& postfix);
	void set_warning_prefix(const std::string& prefix);
	void set_warning_postfix(const std::string& postfix);
	void set_error_prefix(const std::string& prefix);
	void set_error_postfix(const std::string& postfix);
	void set_success_prefix(const std::string& prefix);
	void set_success_postfix(const std::string& postfix);
	
	/**
	 * Pushes a task onto the stack and outputs it to the log.
	 *
	 * @param description Task description.
	 */
	void push_task(const std::string& description);
	
	/**
	 * Pops a task off the stack and outputs its status to the log.
	 *
	 * @param status Exit status of the task. A value of `0` or `EXIT_SUCCESS` indicates the task exited successfully. A non-zero exit status indicates the task failed.
	 */
	void pop_task(int status);
	
	/**
	 * Sets the indent string which prefixes subtasks. This string will be repeated according to the level of indentation.
	 *
	 * @param indent Indent string.
	 */
	void set_indent(const std::string& indent);
	
	/**
	 * Enables or disables prefixing log messages with a timestamp.
	 *
	 * @param enabled `true` if timestamps should be enabled, `false` otherwise.
	 */
	void set_timestamp(bool enabled);

private:
	std::ostream* os;
	std::string log_prefix;
	std::string log_postfix;
	std::string warning_prefix;
	std::string warning_postfix;
	std::string error_prefix;
	std::string error_postfix;
	std::string success_prefix;
	std::string success_postfix;	
	std::stack<std::string> tasks;
	std::string indent;
	bool timestamp_enabled;
};

#endif // ANTKEEPER_LOGGER_HPP

