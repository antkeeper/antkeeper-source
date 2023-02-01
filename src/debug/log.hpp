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

#include "config.hpp"
#include "debug/log/message-severity.hpp"
#include "debug/log/logger.hpp"
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
	 * @param Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using trace = message<message_severity::trace, Args...>;
#else
	// Disable trace message logging.
	inline void trace([[maybe_unused]] ...) noexcept {};
#endif

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_DEBUG)
	/**
	 * Formats and logs a debug message.
	 *
	 * @param Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using debug = message<message_severity::debug, Args...>;
#else
	// Disable debug message logging.
	inline void debug([[maybe_unused]] ...) noexcept {};
#endif

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_INFO)
	/**
	 * Formats and logs an info message.
	 *
	 * @param Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using info = message<message_severity::info, Args...>;
#else
	// Disable info message logging.
	inline void info([[maybe_unused]] ...) noexcept {};
#endif

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_WARNING)
	/**
	 * Formats and logs a warning message.
	 *
	 * @param Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using warning = message<message_severity::warning, Args...>;
#else
	// Disable warning message logging.
	inline void warning([[maybe_unused]] ...) noexcept {};
#endif

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_ERROR)
	/**
	 * Formats and logs an error message.
	 *
	 * @param Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using error = message<message_severity::error, Args...>;
#else
	// Disable error message logging.
	inline void error([[maybe_unused]] ...) noexcept {};
#endif

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_FATAL)
	/**
	 * Formats and logs a fatal error message.
	 *
	 * @param Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using fatal = message<message_severity::fatal, Args...>;
#else
	// Disable fatal error message logging.
	inline void fatal([[maybe_unused]] ...) noexcept {};
#endif

/**
 * Pushes a task onto the default logger's task stack and writes its description the log.
 *
 * @param description Task description.
 * @param location Source location from which the message was sent.
 */
void push_task
(
	const std::string& description,
	std::source_location&& location = std::source_location::current()
);

/**
 * Pops a task off the default logger's task stack and writes its status to the log.
 *
 * @param status Exit status of the task. A value of `0` or `EXIT_SUCCESS` indicates the task exited successfully. A non-zero exit status indicates the task failed.
 * @param description Error code description.
 */
void pop_task
(
	int status,
	const std::string& description = std::string(),
	std::source_location&& location = std::source_location::current()
);

} // namespace log
} // namespace debug

#endif // ANTKEEPER_DEBUG_LOG_HPP
