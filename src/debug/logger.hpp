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

	void push_prefix(const std::string& prefix);
	void pop_prefix();

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
	std::list<std::string> prefix_stack;
};

#endif // ANTKEEPER_LOGGER_HPP

