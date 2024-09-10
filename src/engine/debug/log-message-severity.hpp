// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_HPP
#define ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_HPP

namespace debug {

/// @name Debug logging
/// @{

/** Log message severity levels. */
enum class log_message_severity
{
	/** Trace message severity. */
	trace,
	
	/** Debug message severity. */
	debug,
	
	/** Info message severity. */
	info,
	
	/** Warning message severity. */
	warning,
	
	/** Error message severity. */
	error,
	
	/** Fatal error message severity. */
	fatal
};

/**
 * Returns a string representing a log message severity.
 *
 * @param severity Log message severity.
 * @return Log message severity string.
 */
[[nodiscard]] constexpr const char* log_message_severity_to_string(log_message_severity severity) noexcept
{
	switch (severity)
	{
		case log_message_severity::trace:
			return "trace";
		case log_message_severity::debug:
			return "debug";
		case log_message_severity::info:
			return "info";
		case log_message_severity::warning:
			return "warning";
		case log_message_severity::error:
			return "error";
		case log_message_severity::fatal:
			return "fatal";
		default:
			return "unknown";
	}
}

/// @}

} // namespace debug

#endif // ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_HPP
