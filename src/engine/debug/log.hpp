/*
 * Copyright (C) 2023  Christopher J. Howard
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

#ifndef ANTKEEPER_DEBUG_LOG_HPP
#define ANTKEEPER_DEBUG_LOG_HPP

#include <engine/config.hpp>
#include <engine/debug/log/message-severity.hpp>
#include <engine/debug/log/logger.hpp>
#include <source_location>
#include <string>
#include <format>

// Enable logging of messages of all severities by default.
#if !defined(ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY)
	#define ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY (ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_TRACE)
#endif

namespace debug {

/**
 * Debug message logging.
 */
namespace log {

/**
 * Returns the default logger.
 */
[[nodiscard]] logger& default_logger() noexcept;

/**
 * Self-formatting message that logs itself to the default logger on construction.
 *
 * @tparam Severity Message severity. A message will not log itself if @p Severity is less than the user-defined macro `ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY`.
 * @tparam Args Types of arguments to be formatted.
 */
template <message_severity Severity, class... Args>
struct message
{
	/**
	 * Formats and logs a message.
	 *
	 * Class template argument deduction (CTAD) is utilized to capture source location as a default argument following variadic format arguments.
	 *
	 * @param format Message format string.
	 * @param args Arguments to be formatted.
	 * @param location Source location from which the message was sent.
	 */
	message
	(
		[[maybe_unused]] std::string_view format,
		[[maybe_unused]] Args&&... args,
		[[maybe_unused]] std::source_location&& location = std::source_location::current()
	)
	{
		if constexpr (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= static_cast<std::underlying_type_t<message_severity>>(Severity))
		{
			default_logger().log(std::vformat(format, std::make_format_args(std::forward<Args>(args)...)), Severity, std::forward<std::source_location>(location));
		}
	}
};

// Use class template argument deduction (CTAD) to capture source location as a default argument following variadic format arguments.
template <message_severity Severity, class... Args>
message(std::string_view, Args&&...) -> message<Severity, Args...>;

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_TRACE)
	/**
	 * Formats and logs a trace message.
	 *
	 * @tparam Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using trace = message<message_severity::trace, Args...>;
#else
	// Disable trace message logging.
	template <class... Args>
	inline void trace([[maybe_unused]] Args&&...) noexcept {};
#endif

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_DEBUG)
	/**
	 * Formats and logs a debug message.
	 *
	 * @tparam Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using debug = message<message_severity::debug, Args...>;
#else
	// Disable debug message logging.
	template <class... Args>
	inline void debug([[maybe_unused]] Args&&...) noexcept {};
#endif

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_INFO)
	/**
	 * Formats and logs an info message.
	 *
	 * @tparam Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using info = message<message_severity::info, Args...>;
#else
	// Disable info message logging.
	template <class... Args>
	inline void info([[maybe_unused]] Args&&...) noexcept {};
#endif

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_WARNING)
	/**
	 * Formats and logs a warning message.
	 *
	 * @tparam Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using warning = message<message_severity::warning, Args...>;
#else
	// Disable warning message logging.
	template <class... Args>
	inline void warning([[maybe_unused]] Args&&...) noexcept {};
#endif

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_ERROR)
	/**
	 * Formats and logs an error message.
	 *
	 * @tparam Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using error = message<message_severity::error, Args...>;
#else
	// Disable error message logging.
	template <class... Args>
	inline void error([[maybe_unused]] Args&&...) noexcept {};
#endif

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_FATAL)
	/**
	 * Formats and logs a fatal error message.
	 *
	 * @tparam Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using fatal = message<message_severity::fatal, Args...>;
#else
	// Disable fatal error message logging.
	template <class... Args>
	inline void fatal([[maybe_unused]] Args&&...) noexcept {};
#endif

} // namespace log
} // namespace debug

#endif // ANTKEEPER_DEBUG_LOG_HPP
