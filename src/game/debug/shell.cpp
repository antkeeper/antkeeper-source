// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/debug/shell.hpp"
#include <iomanip>
#include <regex>
#include <sstream>
#include <engine/debug/log.hpp>
#include <stdexcept>

shell::shell():
	m_output{nullptr}
{
	m_output.clear();
	
	set_variable("status", std::to_string(m_status));
	
	set_command("and", std::bind_front(&shell::command_and, this));
	set_command("count", std::bind_front(&shell::command_count, this));
	set_command("echo", std::bind_front(&shell::command_echo, this));
	set_command("false", std::bind_front(&shell::command_false, this));
	set_command("help", std::bind_front(&shell::command_help, this));
	set_command("history", std::bind_front(&shell::command_history, this));
	set_command("not", std::bind_front(&shell::command_not, this));
	set_command("or", std::bind_front(&shell::command_or, this));
	set_command("prompt", std::bind_front(&shell::command_prompt, this));
	set_command("read", std::bind_front(&shell::command_read, this));
	set_command("return", std::bind_front(&shell::command_return, this));
	set_command("set", std::bind_front(&shell::command_set, this));
	set_command("true", std::bind_front(&shell::command_true, this));
}

int shell::interpret(const std::string& input)
{
	// Append input to history
	if (!input.empty())
	{
		m_history.emplace_back(input);
	}
	
	// Write input to output
	m_output << prompt() << input << '\n';
	
	// Split input into semicolon-delimited lines
	auto semicolon_lines = tokenize(input, ";\n");
	
	// For each semicolon-delimited line
	for (const auto& semicolon_line: semicolon_lines)
	{
		// Split semicolon-delimited line into pipe-delimited lines 
		auto pipe_lines = tokenize(semicolon_line, "|");
		
		// Init cin, cout, and cerr
		std::stringstream cin;
		std::stringstream cout;
		std::ostream& cerr = m_output;
		
		// For each pipe-delimited line
		for (auto& pipe_line: pipe_lines)
		{
			// Clear cin then swap cin and cout
			cin.str("");
			cin.clear();
			std::swap(cin, cout);
			
			// Expand shell variables
			pipe_line = expand(pipe_line);
			
			// Extract whitespace-delimited arguments
			auto arguments = tokenize(pipe_line, " \f\n\r\t\v");
			if (arguments.empty())
			{
				// No commands
				continue;
			}
			
			// Unescape arguments
			for (auto& argument: arguments)
			{
				argument = unescape(argument);
			}
			
			// Find command
			if (auto command_it = m_commands.find(arguments.front()); command_it != m_commands.end())
			{
				// Execute command
				m_status = command_it->second(arguments, cin, cout, cerr);
				set_variable("status", std::to_string(m_status));
			}
			else
			{
				// Command not found
				m_status = 404;
				set_variable("status", std::to_string(m_status));
				cerr << "unknown command: " << arguments.front() << '\n';
			}
		}
		
		// Write cout to output buffer
		m_output << cout.view();
	}
	
	// Flush output buffer
	m_output << std::flush;
	
	return m_status;
}

void shell::set_command(const std::string& name, command_function_type function)
{
	static const std::regex command_identifier_pattern("^[a-zA-Z_][a-zA-Z0-9_]*$");
	
	if (!std::regex_match(name, command_identifier_pattern))
	{
		throw std::invalid_argument("Invalid command name.");
	}
	
	if (!function)
	{
		throw std::invalid_argument("Invalid command function.");
	}
	
	m_commands[name] = function;
}

void shell::unset_command(const std::string& name)
{
	if (auto it = m_commands.find(name); it != m_commands.end())
	{
		m_commands.erase(it);
	}
}

void shell::set_variable(const std::string& name, std::span<const std::string> values)
{
	static const std::regex identifier_pattern("^[a-zA-Z_][a-zA-Z0-9_]*$");
	
	if (!std::regex_match(name, identifier_pattern))
	{
		throw std::invalid_argument("Invalid variable name.");
	}
	
	m_variables[name].assign(values.begin(), values.end());
}

void shell::set_variable(const std::string& name, const std::string& value)
{
	set_variable(name, {&value, 1});
}

int shell::unset_variable(const std::string& name)
{
	if (auto it = m_variables.find(name); it != m_variables.end())
	{
		m_variables.erase(it);
		return 0;
	}
	
	return 404;
}

std::string shell::prompt()
{
	if (auto it = m_commands.find("prompt"); it != m_commands.end())
	{
		std::istringstream cin;
		std::ostringstream cout;
		it->second({}, cin, cout, m_output);
		return cout.str();
	}
	
	return {};
}

std::string shell::expand(const std::string& line)
{
	static const std::regex identifier_pattern("\\$([a-zA-Z_][a-zA-Z0-9_]*)(?:\\[([0-9]+)\\])?");
	
	std::string expanded_line = line;
	std::smatch match;
	while (std::regex_search(expanded_line, match, identifier_pattern))
	{
		auto name_string = match[1].str();
		auto index_string = match[2].str();
		if (auto it = m_variables.find(name_string); it != m_variables.end())
		{
			const auto& elements = it->second;
			
			// Join variable elements with a space
			std::ostringstream element_list;
			if (!elements.empty())
			{
				if (!index_string.empty())
				{
					int index = std::stoi(index_string);
					if (index >= 0 && index < elements.size())
					{
						element_list << elements[index];
					}
				}
				else
				{
					element_list << elements.front();
					for (auto element_it = elements.begin() + 1; element_it != elements.end(); ++element_it)
					{
						element_list << ' ' << *element_it;
					}
				}
			}
			
			// Replace identifier with element list
			expanded_line.replace(match.position(), match.length(), element_list.str());
		}
		else
		{
			// Variable not found, erase identifier
			expanded_line.erase(match.position(), match.length());
		}
	}
	
	return expanded_line;
}

std::vector<std::string> shell::tokenize(const std::string& string, const std::string& delimiters) const
{
	std::vector<std::string> tokens;
	bool quoted = false;
	bool escape = false;
	std::string token;
	
	for (char c: string)
	{
		if (delimiters.contains(c) && !quoted && !escape)
		{
			if (!token.empty())
			{
				tokens.push_back(token);
				token.clear();
			}
		}
		else if (c == '"' && !escape)
		{
			quoted = !quoted;
		}
		else
		{
			escape = (c == '\\' && !escape);
			token += c;
		}
	}
	
	if (!token.empty())
	{
		tokens.push_back(token);
	}
	
	return tokens;
}

std::string shell::unescape(const std::string& text) const
{
	static const std::unordered_map<char, char> escape_map =
	{
		{'\'', '\''},
		{'\"', '\"'},
		{'\?', '\?'},
		{'\\', '\\'},
		{'a', '\a'},
		{'b', '\b'},
		{'f', '\f'},
		{'n', '\n'},
		{'r', '\r'},
		{'t', '\t'},
		{'v', '\v'}
	};
	
	std::string result;
	result.reserve(text.size());
	
	for (auto it = text.cbegin(); it != text.cend(); ++it)
	{
		if (*it == '\\' && std::next(it) != text.cend())
		{
			if (auto escape_it = escape_map.find(*(it + 1)); escape_it != escape_map.end())
			{
				result += escape_it->second;
				++it;
			}
			else
			{
				result += *(it + 1);
				++it;
			}
		}
		else
		{
			result += *it;
		}
	}
	
	return result;
}

int shell::command_and(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, std::ostream& cerr)
{
	if (m_status)
	{
		return m_status;
	}
	
	if (arguments.size() == 1)
	{
		return 2;
	}
	
	if (auto it = m_commands.find(arguments[1]); it != m_commands.end())
	{
		return it->second(arguments.subspan(1), cin, cout, cerr);
	}
	
	cerr << std::format("and: unknown command \"{}\"\n", arguments[1]);
	return 404;
}

int shell::command_count(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, [[maybe_unused]] std::ostream& cerr)
{
	// Count arguments
	auto count = arguments.size() - 1;
	
	// Count stdin lines
	std::string line;
	while (std::getline(cin, line))
	{
		++count;
	}
	
	// Print count
	cout << count << '\n';
	
	// Return error on count 0
	return !count;
}

int shell::command_echo(std::span<const std::string> arguments, [[maybe_unused]] std::istream& cin, std::ostream& cout, [[maybe_unused]] std::ostream& cerr)
{
	if (arguments.size() > 1)
	{
		cout << arguments[1];
		for (std::size_t i = 2; i < arguments.size(); ++i)
		{
			cout << ' ' << arguments[i];
		}
	}
	cout << '\n';
	
	return 0;
}

int shell::command_false([[maybe_unused]] std::span<const std::string> arguments, [[maybe_unused]] std::istream& cin, [[maybe_unused]] std::ostream& cout, [[maybe_unused]] std::ostream& cerr)
{
	return 1;
}
 
int shell::command_help(std::span<const std::string> arguments, [[maybe_unused]] std::istream& cin, std::ostream& cout, [[maybe_unused]] std::ostream& cerr)
{
	if (arguments.size() != 1)
	{
		return 1;
	}
	
	for (const auto& [name, function]: m_commands)
	{
		cout << name << '\n';
	}
	
	return 0;
}

int shell::command_history(std::span<const std::string> arguments, [[maybe_unused]] std::istream& cin, std::ostream& cout, [[maybe_unused]] std::ostream& cerr)
{
	if (arguments.size() == 1)
	{
		for (const auto& line: m_history)
		{
			cout << line << '\n';
		}
		
		return 0;
	}
	
	return 1;
}

int shell::command_not(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, std::ostream& cerr)
{
	if (arguments.size() == 1)
	{
		return 2;
	}
	
	if (auto it = m_commands.find(arguments[1]); it != m_commands.end())
	{
		return !it->second(arguments.subspan(1), cin, cout, cerr);
	}
	
	cerr << std::format("not: unknown command \"{}\"\n", arguments[1]);
	return 404;
}

int shell::command_or(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, std::ostream& cerr)
{
	if (!m_status)
	{
		return m_status;
	}
	
	if (arguments.size() == 1)
	{
		return 2;
	}
	
	if (auto it = m_commands.find(arguments[1]); it != m_commands.end())
	{
		return it->second(arguments.subspan(1), cin, cout, cerr);
	}
	
	cerr << std::format("or: unknown command \"{}\"\n", arguments[1]);
	return 404;
}

int shell::command_prompt([[maybe_unused]] std::span<const std::string> arguments, [[maybe_unused]] std::istream& cin, std::ostream& cout, [[maybe_unused]] std::ostream& cerr)
{
	if (m_status)
	{
		cout << std::format("[{}]> ", m_status);
	}
	else
	{
		cout << "> ";
	}
	
	return 0;
}

int shell::command_read(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, [[maybe_unused]] std::ostream& cerr)
{
	static const std::string whitespace = " \f\n\r\t\v";
	
	if (arguments.size() == 1)
	{
		// Write cin to cout
		std::string line;
		while (std::getline(cin, line))
		{
			cout << line << '\n';
		}
	}
	else if (arguments.size() == 2)
	{
		// Get single line from cin
		std::string line;
		std::getline(cin, line);
		
		// Tokenize line
		auto tokens = tokenize(line, whitespace);
		
		// Set variable
		m_variables[arguments[1]].assign(tokens.begin(), tokens.end());
	}
	else
	{
		return 1;
	}
	
	return 0;
}

int shell::command_return(std::span<const std::string> arguments, [[maybe_unused]] std::istream& cin, [[maybe_unused]] std::ostream& cout, std::ostream& cerr)
{
	if (arguments.size() == 1)
	{
		return m_status;
	}
	else if (arguments.size() > 2)
	{
		cerr << std::format("return: too many arguments\n");
		return 2;
	}
	
	if (int status; std::istringstream(arguments[1]) >> status)
	{
		return status;
	}
	
	cerr << std::format("return: invalid integer \"{}\"\n", arguments[1]);
	return 2;
}

int shell::command_set(std::span<const std::string> arguments, [[maybe_unused]] std::istream& cin, std::ostream& cout, std::ostream& cerr)
{
	static const std::regex identifier_pattern("^[a-zA-Z_][a-zA-Z0-9_]*$");
	
	// Print all shell variables
	if (arguments.size() == 1)
	{
		for (const auto& [name, elements]: m_variables)
		{
			// Print variable name
			cout << name;
			
			// Print variable elements
			if (elements.size() == 1)
			{
				cout << ' ' << elements.front();
			}
			else if (elements.size() > 1)
			{
				for (const auto& element: elements)
				{
					cout << " \"" << element << "\"";
				}
			}
			
			cout << '\n';
		}
		
		return 0;
	}
	
	// Validate identifier
	if (!std::regex_match(arguments[1], identifier_pattern))
	{
		cerr << std::format("set: invalid variable name \"{}\"\n", arguments[1]);
		return 1;
	}
	
	// Set or clear variable
	if (arguments.size() == 2)
	{
		m_variables[arguments[1]].clear();
	}
	else
	{
		m_variables[arguments[1]].assign(arguments.begin() + 2, arguments.end());
	}
	
	return 0;
}

int shell::command_true([[maybe_unused]] std::span<const std::string> arguments, [[maybe_unused]] std::istream& cin, [[maybe_unused]] std::ostream& cout, [[maybe_unused]] std::ostream& cerr)
{
	return 0;
}
