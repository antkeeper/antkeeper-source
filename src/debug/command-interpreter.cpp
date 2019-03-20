/*
 * Copyright (C) 2017-2019  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "command-interpreter.hpp"
#include <sstream>

template<>
int ArgumentParser<int>::parse(const std::string& argument)
{
	return std::stoi(argument);
}

template<>
unsigned int ArgumentParser<unsigned int>::parse(const std::string& argument)
{
	return static_cast<unsigned int>(std::stoul(argument));
}

template<>
long ArgumentParser<long>::parse(const std::string& argument)
{
	return std::stol(argument);
}

template<>
unsigned long ArgumentParser<unsigned long>::parse(const std::string& argument)
{
	return std::stoul(argument);
}

template<>
float ArgumentParser<float>::parse(const std::string& argument)
{
	return std::stof(argument);
}

template<>
double ArgumentParser<double>::parse(const std::string& argument)
{
	return std::stod(argument);
}

template<>
std::string ArgumentParser<std::string>::parse(const std::string& argument)
{
	return argument;
}

void CommandInterpreter::set(const std::string& name, const std::string& value)
{
	variableMap[name] = value;
}

void CommandInterpreter::unset(const std::string& name)
{
	auto it = variableMap.find(name);
	if (it != variableMap.end())
	{
		variableMap.erase(it);
	}
}

const std::map<std::string, std::string>& CommandInterpreter::help() const
{
	return helpStrings;
}

const std::map<std::string, std::string>& CommandInterpreter::variables() const
{
	return variableMap;
}

std::tuple<std::string, std::vector<std::string>, std::function<void()>> CommandInterpreter::interpret(const std::string& line)
{
	// Split line into arguments
	std::vector<std::string> arguments;
	std::stringstream stream(line);
	std::string argument;
	while (std::getline(stream, argument, ' '))
	{
		arguments.push_back(argument);
	}

	if (arguments.empty())
	{
		return {std::string(), std::vector<std::string>(), nullptr};
	}

	// Perform variable substitution/expansion on '$' operators
	for (std::string& argument: arguments)
	{
		if (!argument.empty() && argument[0] == '$')
		{
			std::string variableName = argument.substr(1);
			std::string variableValue = variableMap[variableName];
			argument = variableValue;
		}
	}

	// Get command name
	std::string commandName = arguments[0];
	
	// Remove command name from arguments
	arguments.erase(arguments.begin());

	// Find command linker for this command
	auto linker = linkers.find(commandName);
	if (linker == linkers.end())
	{
		return {commandName, arguments, nullptr};
	}

	// Link command function and its arguments into a callable object
	std::function<void()> call = linker->second(arguments);

	return {commandName, arguments, call};
}

