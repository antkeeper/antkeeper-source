// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_DEBUG_CLI_HPP
#define ANTKEEPER_DEBUG_CLI_HPP

#include <functional>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>

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
	 *
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
	 * @tparam T Argument type.
	 *
	 * @param stream String stream from which an argument should be parsed.
	 */
	template <class T>
	[[nodiscard]] static T parse(std::istringstream& stream);
	
	/**
	 * Wraps a function in an interpreter function that parses strings as arguments, then executes the wrapped function with the parsed arguments.
	 *
	 * @tparam T Function return type.
	 * @tparam Args Function argument types.
	 *
	 * @param function Function to wrap.
	 *
	 * @return Wrapped function.
	 */
	template <class T, class... Args>
	[[nodiscard]] static command_type wrap(std::function<T(Args...)> function);
	
	std::unordered_map<std::string, command_type> commands;
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
typename cli::command_type cli::wrap(std::function<T(Args...)> function)
{
	return std::bind
	(
		[function](const std::string& line) -> std::string
		{
			//constexpr std::size_t argument_count = sizeof...(Args);
			
			// Parse string into tuple of arguments
			std::istringstream istream(line);
			std::tuple<Args...> arguments{parse<Args>(istream)...};
			
			if constexpr(std::is_void_v<T>)
			{
				// Invoke function with parsed arguments
				std::apply(function, arguments);
				
				// Return empty string
				return std::string();
			}
			else
			{
				// Invoke function with parsed arguments and save the result
				T result = std::apply(function, arguments);
				
				// Return invocation result as a string
				std::ostringstream ostream;
				ostream << result;
				return ostream.str();
			}
		},
		std::placeholders::_1
	);
}

} // namespace debug

#endif // ANTKEEPER_DEBUG_CLI_HPP
