// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_HPP
#define ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_HPP

namespace debug {

/// @name Logging
/// @{

/// Log message severity levels.
enum class log_message_severity
{
	/// Trace message severity.
	trace,
	
	/// Debug message severity.
	debug,
	
	/// Info message severity.
	info,
	
	/// Warning message severity.
	warning,
	
	/// Error message severity.
	error,
	
	/// Fatal error message severity.
	fatal
};

/// Returns a string representing a log message severity.
/// @param severity Log message severity.
/// @return Log message severity string.
[[nodiscard]] const char* log_message_severity_to_string(log_message_severity severity) noexcept;

/// @}

} // namespace debug

#endif // ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_HPP
