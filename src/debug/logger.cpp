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

#include "logger.hpp"
#include <iostream>

logger::logger():
	os(&std::cout),
	log_prefix(std::string()),
	log_postfix(std::string()),
	warning_prefix(std::string("Warning: ")),
	warning_postfix(std::string()),
	error_prefix(std::string("Error: ")),
	error_postfix(std::string()),
	success_prefix(std::string("Success: ")),
	success_postfix(std::string()),
	next_task_id(0)
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
		for (const std::string& prefix: prefix_stack)
		{
			(*os) << prefix;
		}

		(*os) << (log_prefix + text + log_postfix);

		os->flush();
	}
}

void logger::warning(const std::string& text)
{
	log(warning_prefix + text + warning_postfix);
}

void logger::error(const std::string& text)
{
	log(error_prefix + text + error_postfix);
}

void logger::success(const std::string& text)
{
	log(success_prefix + text + success_postfix);
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

void logger::push_prefix(const std::string& prefix)
{
	prefix_stack.push_back(prefix);
}

void logger::pop_prefix()
{
	prefix_stack.pop_back();
}

int logger::open_task(const std::string& text)
{
	tasks[next_task_id] = text;
	
	std::string indent;
	for (std::size_t i = 0; i < tasks.size() - 1; ++i)
		indent += "    ";
	
	log(indent + text + "...\n");
	
	return next_task_id++;
}

bool logger::close_task(int id, int status)
{
	auto it = tasks.find(id);
	if (it == tasks.end())
		return false;
	
	std::string message = it->second + "... ";
	
	
	std::string indent;
	for (std::size_t i = 0; i < tasks.size() - 1; ++i)
		indent += "    ";
	
	if (status == EXIT_SUCCESS)
	{
		message += "success\n";
		log(indent + message);
	}
	else
	{
		message += "failed (" + std::to_string(status) + ")\n";
		log(indent + message);
	}
	
	tasks.erase(it);
}

