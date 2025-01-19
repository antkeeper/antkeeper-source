// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SCRIPT_ERROR_HPP
#define ANTKEEPER_SCRIPT_ERROR_HPP

#include <exception>
#include <string>
#include <utility>

/// Base class for errors thrown by script contexts.
class script_error: public std::exception
{
public:
	script_error(std::string message, int code):
		m_message(std::move(message)), m_error_code(code)
	{}

	[[nodiscard]] inline constexpr const char* what() const noexcept override
	{
		return m_message.c_str();
	}

	[[nodiscard]] inline constexpr int code() const noexcept
	{
		return m_error_code;
	}

private:
	std::string m_message;
	int m_error_code{};
};

#endif // ANTKEEPER_SCRIPT_ERROR_HPP
