// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/debug/cli.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <regex>

namespace debug {

cli::cli():
	m_output(&std::cout)
{}

int cli::interpret(const std::string& line)
{
	// Split line into semicolon-delimited commands
	auto pipes = split(line, '|');
	
	// Execute commands
	std::ostringstream oss;
	for (auto it1 = pipes.begin(); it1 != pipes.end(); ++it1)
	{
		auto& command = *it1;
		
		if (it1 != pipes.begin())
		{
			command += ' ' + oss.str();
			oss.str("");
			oss.clear();
		}
		
		// Expand and tokenize command line
		const auto arguments = tokenize(expand(command));
		if (arguments.empty())
		{
			continue;
		}
		
		if (auto it2 = m_commands.find(arguments.front()); it2 != m_commands.end())
		{
			std::ostream& redirected_output = (it1 == pipes.end() - 1) ? *m_output : oss;
			
			// Execute command
			if (int status = it2->second(arguments, redirected_output); status != 0)
			{
				// Exit failure
				return status;
			}
		}
		else
		{
			// Command not found
			return 404;
		}
	}
	
	// Exit success
	return 0;
}

std::vector<std::string> cli::split(const std::string& line, char delimiter) const
{
	std::vector<std::string> tokens;
	
	std::string token;
	bool quoted = false;
	for (char c: line)
	{
		if (c == delimiter && !quoted)
		{
			tokens.push_back(token);
			token.clear();
		}
		else
		{
			if (c == '"')
			{
				quoted = !quoted;
			}
			
			token += c;
		}
	}
	
	if (!token.empty())
	{
		tokens.push_back(token);
	}
	
	return tokens;
}

std::string cli::expand(const std::string& line) const
{
	static const std::regex variable_regex(R"(\$([a-zA-Z_]\w*))");
	
	std::string expanded_line = line;
	std::smatch match;
	while (std::regex_search(expanded_line, match, variable_regex))
	{
		if (match.size() == 2)
		{
			const std::string& variable_name = match[1].str();
			if (auto it = m_variables.find(variable_name); it != m_variables.end())
			{
				expanded_line.replace(match.position(), match.length(), it->second);
			}
			else
			{
				expanded_line.erase(match.position(), match.length());
			}
		}
	}
	
	return expanded_line;
}

std::vector<std::string> cli::tokenize(const std::string& line) const
{
	std::vector<std::string> tokens;
	if (!line.empty())
	{
		std::istringstream iss(line);
		std::string argument;
		while (iss >> std::quoted(argument))
		{
			tokens.push_back(argument);
		}
	}
	
	return tokens;
}

} // namespace debug
