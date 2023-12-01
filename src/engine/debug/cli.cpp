// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/debug/cli.hpp>

namespace debug {

std::string cli::interpret(const std::string& line) const
{
	std::istringstream stream(line);
	std::string command_name;
	stream >> command_name;
	
	if (auto it = commands.find(command_name); it != commands.end())
	{
		return it->second(line.substr(command_name.length() + 1));
	}
	
	return std::string();
}

void cli::unregister_command(const std::string& name)
{
	if (auto it = commands.find(name); it != commands.end())
		commands.erase(it);
}

} // namespace debug
