// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_DEBUG_LOG_HPP
#define ANTKEEPER_DEBUG_LOG_HPP

#include <engine/debug/log/log-message-severity.hpp>
#include <engine/debug/log/logger.hpp>
#include <source_location>
#include <string>
#include <format>

// Disable trace message logging on release builds
#if defined(NDEBUG)
	#define ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY 1
#else
	#define ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY 0
#endif

namespace debug {

/// @name Debug logging
/// @{

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
template <log_message_severity Severity, class... Args>
struct log_message
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
	log_message
	(
		[[maybe_unused]] std::format_string<Args...> format,
		[[maybe_unused]] Args&&... args,
		[[maybe_unused]] std::source_location&& location = std::source_location::current()
	)
	{
		if constexpr (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= static_cast<std::underlying_type_t<log_message_severity>>(Severity))
		{
			default_logger().log(std::format(format, std::forward<Args>(args)...), Severity, std::forward<std::source_location>(location));
		}
	}
};

// Use class template argument deduction (CTAD) to capture source location as a default argument following variadic format arguments.
template <log_message_severity Severity, class... Args>
log_message(std::format_string<Args...>, Args&&...) -> log_message<Severity, Args...>;

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_TRACE)
	/**
	 * Formats and logs a trace message.
	 *
	 * @tparam Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using log_trace = log_message<log_message_severity::trace, Args...>;
#else
	// Disable trace message logging.
	template <class... Args>
	inline void log_trace([[maybe_unused]] Args&&...) noexcept {};
#endif

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_DEBUG)
	/**
	 * Formats and logs a debug message.
	 *
	 * @tparam Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using log_debug = log_message<log_message_severity::debug, Args...>;
#else
	// Disable debug message logging.
	template <class... Args>
	inline void log_debug([[maybe_unused]] Args&&...) noexcept {};
#endif

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_INFO)
	/**
	 * Formats and logs an info message.
	 *
	 * @tparam Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using log_info = log_message<log_message_severity::info, Args...>;
#else
	// Disable info message logging.
	template <class... Args>
	inline void log_info([[maybe_unused]] Args&&...) noexcept {};
#endif

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_WARNING)
	/**
	 * Formats and logs a warning message.
	 *
	 * @tparam Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using log_warning = log_message<log_message_severity::warning, Args...>;
#else
	// Disable warning message logging.
	template <class... Args>
	inline void log_warning([[maybe_unused]] Args&&...) noexcept {};
#endif

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_ERROR)
	/**
	 * Formats and logs an error message.
	 *
	 * @tparam Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using log_error = log_message<log_message_severity::error, Args...>;
#else
	// Disable error message logging.
	template <class... Args>
	inline void log_error([[maybe_unused]] Args&&...) noexcept {};
#endif

#if (ANTKEEPER_DEBUG_LOG_MIN_MESSAGE_SEVERITY <= ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_FATAL)
	/**
	 * Formats and logs a fatal error message.
	 *
	 * @tparam Args Types of arguments to be formatted.
	 */
	template <class... Args>
	using log_fatal = log_message<log_message_severity::fatal, Args...>;
#else
	// Disable fatal error message logging.
	template <class... Args>
	inline void log_fatal([[maybe_unused]] Args&&...) noexcept {};
#endif

/// @}

} // namespace debug

#endif // ANTKEEPER_DEBUG_LOG_HPP
