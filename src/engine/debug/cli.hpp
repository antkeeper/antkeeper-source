// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_DEBUG_CLI_HPP
#define ANTKEEPER_DEBUG_CLI_HPP

#include <functional>
#include <ostream>
#include <span>
#include <string>
#include <unordered_map>

namespace debug {

/**
 * Minimal command-line interpreter.
 */
class cli
{
public:
	/// Command function type.
	using command_type = std::function<int(std::span<const std::string>, std::ostream&)>;
	
	/// Constructs a CLI.
	cli();
	
	/**
	 * Interprets a command line.
	 * 
	 * @param line Command line to interpret.
	 * 
	 * @return Exit status of the command.
	 */
	virtual int interpret(const std::string& line);
	
	/// Returns the command registry.
	/// @{
	[[nodiscard]] inline const auto& commands() const noexcept
	{
		return m_commands;
	}
	[[nodiscard]] inline auto& commands() noexcept
	{
		return m_commands;
	}
	/// @}
	
	/// Returns the variable registry.
	/// @{
	[[nodiscard]] inline const auto& variables() const noexcept
	{
		return m_variables;
	}
	[[nodiscard]] inline auto& variables() noexcept
	{
		return m_variables;
	}
	/// @}
	
	/// Returns the output stream.
	/// @{
	[[nodiscard]] inline const auto& output() const noexcept
	{
		return m_output;
	}
	[[nodiscard]] inline auto& output() noexcept
	{
		return m_output;
	}
	/// @}
	
	/**
	 * Splits a command line into multiple commands.
	 *
	 * @param line Command line to split.
	 * @param delimiter Command delimiter.
	 *
	 * @return Commands.
	 */
	[[nodiscard]] std::vector<std::string> split(const std::string& line, char delimiter) const;
	
	/**
	 * Performs variable expansion on a command line.
	 *
	 * @param line Command line to variable-expand.
	 *
	 * @return Variable-expanded command line.
	 */
	[[nodiscard]] std::string expand(const std::string& line) const;
	
	/**
	 * Tokenizes a command line.
	 *
	 * @param line Command line to tokenize.
	 *
	 * @return Command line tokens.
	 */
	[[nodiscard]] std::vector<std::string> tokenize(const std::string& line) const;
	
private:
	std::unordered_map<std::string, command_type> m_commands;
	std::unordered_map<std::string, std::string> m_variables;
	std::ostream* m_output;
};

} // namespace debug

#endif // ANTKEEPER_DEBUG_CLI_HPP
