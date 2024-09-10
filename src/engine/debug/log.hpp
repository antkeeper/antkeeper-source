// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_DEBUG_LOG_HPP
#define ANTKEEPER_DEBUG_LOG_HPP

#include <engine/debug/log-message-severity.hpp>
#include <engine/debug/logger.hpp>
#include <format>
#include <source_location>
#include <string>
#include <utility>

namespace debug {

/// @name Debug logging
/// @{

#if !defined(NDEBUG)
	/** Minimum log message severity for debug builds. */
	inline constexpr auto min_log_message_severity = log_message_severity::trace;
#else
	/** Minimum log message severity for non-debug builds. */
	inline constexpr auto min_log_message_severity = log_message_severity::info;
#endif

/**
 * Log message that formats and logs itself to the default logger on construction.
 *
 * @tparam Severity Message severity. A message will not be logged if @p Severity is less than min_log_message_severity.
 * @tparam Args Types of arguments to be formatted.
 */
template <log_message_severity Severity, class... Args>
struct log_message
{
	/**
	 * Formats and logs a message.
	 *
	 * @param format Message format string.
	 * @param args Arguments to be formatted.
	 * @param location Source location from which the message was logged.
	 */
	constexpr log_message
	(
		[[maybe_unused]] std::format_string<Args...> format,
		[[maybe_unused]] Args&&... args,
		[[maybe_unused]] std::source_location&& location = std::source_location::current()
	)
	{
		if constexpr (std::to_underlying(Severity) >= std::to_underlying(min_log_message_severity))
		{
			default_logger().log(std::format(std::move(format), std::forward<Args>(args)...), Severity, std::move(location));
		}
	}
};

// Enable capture of source location as a default argument following variadic format arguments using CTAD.
template <log_message_severity Severity, class... Args>
log_message(std::format_string<Args...>, Args&&...) -> log_message<Severity, Args...>;

/**
 * Formats and logs a trace message.
 *
 * @tparam Args Types of arguments to be formatted.
 */
template <class... Args>
using log_trace = log_message<log_message_severity::trace, Args...>;

/**
 * Formats and logs a debug message.
 *
 * @tparam Args Types of arguments to be formatted.
 */
template <class... Args>
using log_debug = log_message<log_message_severity::debug, Args...>;

/**
 * Formats and logs an info message.
 *
 * @tparam Args Types of arguments to be formatted.
 */
template <class... Args>
using log_info = log_message<log_message_severity::info, Args...>;

/**
 * Formats and logs a warning message.
 *
 * @tparam Args Types of arguments to be formatted.
 */
template <class... Args>
using log_warning = log_message<log_message_severity::warning, Args...>;

/**
 * Formats and logs an error message.
 *
 * @tparam Args Types of arguments to be formatted.
 */
template <class... Args>
using log_error = log_message<log_message_severity::error, Args...>;

/**
 * Formats and logs a fatal error message.
 *
 * @tparam Args Types of arguments to be formatted.
 */
template <class... Args>
using log_fatal = log_message<log_message_severity::fatal, Args...>;

/// @}

} // namespace debug

#endif // ANTKEEPER_DEBUG_LOG_HPP
