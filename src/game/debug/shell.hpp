// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SHELL_HPP
#define ANTKEEPER_SHELL_HPP

#include <functional>
#include <istream>
#include <ostream>
#include <span>
#include <string>
#include <map>
#include <vector>

/// Simple command-line interpreter.
class shell
{
public:
	/// Command function type.
	using command_function_type = std::function<int(std::span<const std::string>, std::istream&, std::ostream&, std::ostream&)>;
	
	/// Constructs a shell.
	shell();
	
	/// Interprets a command line.
	/// @param input Command line(s) to interpret.
	/// @return Exit status of the command.
	int interpret(const std::string& input);
	
	/// Returns the command history.
	[[nodiscard]] inline auto& get_history() noexcept
	{
		return m_history;
	}
	
	/// @copydoc get_history()
	[[nodiscard]] inline const auto& get_history() const noexcept
	{
		return m_history;
	}
	
	/// Registers a command with the shell.
	/// @param name Command name. Must be a standard identifier.
	/// @param function Command function.
	/// @exception std::invalid_argument Invalid command name.
	/// @exception std::invalid_argument Invalid command function.
	void set_command(const std::string& name, command_function_type function);
	
	/// Unregisters a command with the shell.
	/// @param name Command name.
	void unset_command(const std::string& name);
	
	/// Returns the command registry.
	[[nodiscard]] inline const auto& get_commands() const noexcept
	{
		return m_commands;
	}
	
	/// Sets a shell variable.
	/// @param name Name of the variable to set. Must be a standard identifier.
	/// @param values Variable value array.
	/// @exception std::invalid_argument Invalid variable name.
	void set_variable(const std::string& name, std::span<const std::string> values = {});
	
	/// Sets a shell variable.
	/// @param name Name of the variable to set. Must be a standard identifier.
	/// @param value Variable value.
	/// @exception std::invalid_argument Invalid variable name.
	void set_variable(const std::string& name, const std::string& value);
	
	/// Unsets a shell variable.
	/// @param name Name of the variable to unset.
	/// @return `0` on success, non-zero otherwise.
	int unset_variable(const std::string& name);
	
	/// Returns the variable registry.
	[[nodiscard]] inline const auto& get_variables() const noexcept
	{
		return m_variables;
	}
	
	/// Returns the prompt string.
	[[nodiscard]] std::string prompt();
	
	/// Returns the output stream.
	[[nodiscard]] inline auto& get_output() noexcept
	{
		return m_output;
	}

	/// @copydoc get_output()
	[[nodiscard]] inline const auto& get_output() const noexcept
	{
		return m_output;
	}

	/// Returns the exit status of the last command.
	[[nodiscard]] inline int get_status() const noexcept
	{
		return m_status;
	}
	
private:
	/// Performs variable expansion on a command line.
	/// @param line Command line to variable-expand.
	/// @return Variable-expanded command line.
	[[nodiscard]] std::string expand(const std::string& line);
	
	/// Tokenizes a string.
	/// @param string String to tokenize.
	/// @param delimiters Token delimiters.
	/// @return Command line tokens.
	[[nodiscard]] std::vector<std::string> tokenize(const std::string& string, const std::string& delimiters) const;
	
	/// Replaces escape sequences with the characters they represent.
	/// @param text Text to unescape.
	/// @return Unescaped text.
	[[nodiscard]] std::string unescape(const std::string& text) const;
	
	/// @name Built-in commands
	/// @{
	
	/// Executes a command if the previous command succeeded.
	int command_and(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, std::ostream& cerr);
	
	/// Prints the number of lines in `stdin` plus the number of arguments.
	int command_count(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, std::ostream& cerr);
	
	/// Prints arguments.
	int command_echo(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, std::ostream& cerr);
	
	/// Returns exit status 1.
	int command_false(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, std::ostream& cerr);
	
	/// Prints a list of commands and their usage.
	int command_help(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, std::ostream& cerr);
	
	/// Prints command line history.
	int command_history(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, std::ostream& cerr);
	
	/// Negates the exit status of a command.
	int command_not(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, std::ostream& cerr);
	
	/// Executes a command if the previous command failed.
	int command_or(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, std::ostream& cerr);
	
	/// Prints prompt text.
	int command_prompt(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, std::ostream& cerr);
	
	/// Reads a line of input into a variable.
	int command_read(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, std::ostream& cerr);
	
	/// Returns a status code.
	int command_return(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, std::ostream& cerr);
	
	/// Sets a shell variable or prints all shell variables and their values.
	int command_set(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, std::ostream& cerr);
	
	/// Returns exit status 0.
	int command_true(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, std::ostream& cerr);
	
	/// @}
	
	std::vector<std::string> m_history;
	std::map<std::string, command_function_type> m_commands;
	std::map<std::string, std::vector<std::string>> m_variables;
	std::ostream m_output;
	int m_status{};
};

#endif // ANTKEEPER_SHELL_HPP
