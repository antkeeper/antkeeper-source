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

#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

/**
 * Parses an argument string into a typed value.
 */
template<class T>
class ArgumentParser
{
public:
	static T parse(const std::string& argument);
};

template<>
int ArgumentParser<int>::parse(const std::string& argument);
template<>
unsigned int ArgumentParser<unsigned int>::parse(const std::string& argument);
template<>
long ArgumentParser<long>::parse(const std::string& argument);
template<>
unsigned long ArgumentParser<unsigned long>::parse(const std::string& argument);
template<>
float ArgumentParser<float>::parse(const std::string& argument);
template<>
double ArgumentParser<double>::parse(const std::string& argument);
template<>
std::string ArgumentParser<std::string>::parse(const std::string& argument);

/**
 * Parses an argument vector of strings into a tuple of typed values.
 */
class ArgumentVectorParser
{
public:
	template<class... Args>
	static std::tuple<Args...> parse(const std::vector<std::string>& arguments)
	{
		if (arguments.size() != sizeof...(Args))
		{
			throw std::invalid_argument("Argument vector size doesn't match function parameter count.");
		}

		return parse<Args...>(arguments, std::make_index_sequence<sizeof...(Args)>{});
	}

private:
	template <class... Args, std::size_t... index>
	static std::tuple<Args...> parse(const std::vector<std::string>& arguments, std::index_sequence<index...>)
	{
		return {ArgumentParser<Args>::parse(arguments[index])...};
	}
};

class CommandLinker
{
public:
	/**
	 * Links a function and its arguments together into a single callable object.
	 */
	template<class... Args>
	static std::function<void()> link(const std::function<void(Args...)>& function, const std::vector<std::string>& arguments)
	{
		// Parse argument vectors and store in a tuple
		auto parsedArguments = ArgumentVectorParser::parse<Args...>(arguments);

		// Return callable object which will invoke the function with the parsed arguments
		return std::bind
		(
			[function, parsedArguments]()
			{
				std::apply(function, parsedArguments);
			}
		);
	}
};

/**
 * 
 */
class CommandInterpreter
{
public:
	/**
	 * Registers a command.
	 *
	 * @param name Name used to invoke the command.
	 * @param function Function associated with the command.
	 */
	template <class... Args>
	void registerCommand(const std::string& name, const std::function<void(Args...)>& function, const std::string& helpString = std::string());
	template <class... Args>
	void registerCommand(const std::string& name, void(*function)(Args...));

	/**
	 * Sets the value of an interpreter variable.
	 *
	 * @param name Interpreter variable name.
	 * @param value Interpreter variable value.
	 */
	void set(const std::string& name, const std::string& value);

	/**
	 * Unsets an interpreter variable.
	 *
	 * @param name Interpreter variable name.
	 */
	void unset(const std::string& name);


	/**
	 * Returns the help strings for all commands.
	 */
	const std::map<std::string, std::string>& help() const;

	/**
	 * Interprets a line of text as a function call, returning the interpreted command name, argument vector, and callable function object.
	 *
	 * @param line Line of text to be interpreted. Arguments are delimeted by spaces, with the first argument as the command name. Command names containing the '.' operator will have the post-dot string substituted for its console variable value, then the string will be transposed around the dot, and the dot will be replaced by a space, such that the command "object.setValue 10" would become "setValue x 10" if that console variable "object" was set to "x". Arguments beginning with substitution operator '$' will interpreted as variables and substituted with their values.
	 */
	std::tuple<std::string, std::vector<std::string>, std::function<void()>> interpret(const std::string& line);

private:
	template <class Function, class Linker>
	void addCommandLinker(const std::string& name, const Function& function, const Linker& linker);

	// A command name-keyed map of command linkers
	std::unordered_map<std::string, std::function<std::function<void()>(const std::vector<std::string>&)>> linkers;
	std::map<std::string, std::string> helpStrings;
	std::unordered_map<std::string, std::string> variables;
};

template <class... Args>
void CommandInterpreter::registerCommand(const std::string& name, const std::function<void(Args...)>& function, const std::string& helpString)
{
	addCommandLinker(name, function, CommandLinker::link<Args...>);
	helpStrings[name] = helpString;
}

template <class... Args>
void CommandInterpreter::registerCommand(const std::string& name, void(*function)(Args...))
{
	addCommandLinker(name, function, CommandLinker::link<Args...>);
}

template <class Function, class Linker>
void CommandInterpreter::addCommandLinker(const std::string& name, const Function& function, const Linker& linker)
{
	linkers[name] = std::bind(linker, function, std::placeholders::_1);
}

#endif // CONSOLE_HPP

