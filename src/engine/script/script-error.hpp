// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <exception>
#include <string>
#include <utility>

namespace engine
{
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
}
