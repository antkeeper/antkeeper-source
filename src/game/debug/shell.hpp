// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SHELL_HPP
#define ANTKEEPER_SHELL_HPP

#include <engine/script/script-context.hpp>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

class game;

/// Simple command-line interpreter.
class shell
{
public:
	/// Constructs a shell.
	/// @param ctx Game context.
	shell(game* ctx);

	/// Destructs a shell.
	~shell();

	shell(const shell&) = delete;
	shell(shell&&) = delete;
	shell& operator=(const shell&) = delete;
	shell& operator=(shell&&) = delete;
	
	/// Interprets a command line.
	/// @param input Command line(s) to interpret.
	/// @return Exit status of the command.
	int interpret(const std::string_view& input);
	
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
	game* m_context{};
	std::vector<std::string> m_history;
	std::ostream m_output;
	int m_status{};
};

#endif // ANTKEEPER_SHELL_HPP
