/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_DEBUG_CLI_HPP
#define ANTKEEPER_DEBUG_CLI_HPP

#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <tuple>

namespace debug {

/**
 * Minimal command-line interpreter.
 */
class cli
{
public:
	/**
	 * Interprets a command line as a function invocation.
	 *
	 * @param line Command line.
	 * @return Stringified return value of the command function.
	 */
	std::string interpret(const std::string& line) const;

	/**
	 * Registers a command with the CLI.
	 *
	 * @tparam T Command function return type.
	 * @tparam Args Command function argument types.
	 * @param name Command name.
	 * @param function Command function.
	 */
	/// @{
	template <class T, class... Args>
	void register_command(const std::string& name, const std::function<T(Args...)>& function);
	template <class T, class... Args>
	void register_command(const std::string& name, T (*function)(Args...));
	/// @}

	/**
	 * Unregisters a command from the CLI.
	 *
	 * @param name Command name.
	 */
	void unregister_command(const std::string& name);

private:
	/// String-wrapped function object
	typedef std::function<std::string(const std::string&)> command_type;
	
	/**
	 * Parses a single argument from a string stream.
	 *
	 * @param stream String stream from which an argument should be parsed.
	 */
	template <class T>
	static T parse(std::istringstream& stream);

	/**
	 * Wraps a function in an interpreter function that parses strings as arguments then executes the wrapped function with the parsed arguments.
	 *
	 * @param function Function to wrap.
	 * @return Wrapped function.
	 */
	template <class T, class... Args>
	static command_type wrap(const std::function<T(Args...)>& function);

	std::map<std::string, command_type> commands;
};

template <class T, class... Args>
void cli::register_command(const std::string& name, const std::function<T(Args...)>& function)
{
	commands[name] = wrap(function);
}

template <class T, class... Args>
void cli::register_command(const std::string& name, T (*function)(Args...))
{
	commands[name] = wrap(std::function(function));
}

template <class T>
T cli::parse(std::istringstream& stream)
{
	T value;
	stream >> value;
	return value;
}

template <class T, class... Args>
typename cli::command_type cli::wrap(const std::function<T(Args...)>& function)
{
	return std::bind(
		[function](const std::string& line) -> std::string
		{
			//std::size_t argument_count = sizeof...(Args);

			// Parse string into tuple of arguments
			std::istringstream istream(line);
			std::tuple<Args...> arguments{parse<Args>(istream)...};

			// Invoke function with arguments and save the result
			T result = std::apply(function, arguments);

			// Return function result as string
			std::ostringstream ostream;
			ostream << result;
			return ostream.str();
		},
		std::placeholders::_1);
}

} // namespace debug

#endif // ANTKEEPER_DEBUG_CLI_HPP
