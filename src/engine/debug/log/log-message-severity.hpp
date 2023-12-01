// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_HPP
#define ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_HPP

#include <cstdint>

namespace debug {

/// @name Debug logging
/// @{

/// Trace message severity level.
#define ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_TRACE   0

/// Debug message severity level.
#define ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_DEBUG   1

/// Info message severity level.
#define ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_INFO    2

/// Warning message severity level.
#define ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_WARNING 3

/// Error message severity level.
#define ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_ERROR   4

/// Fatal error message severity level.
#define ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_FATAL   5

/// Log message severity levels.
enum class log_message_severity: std::uint8_t
{
	/// Trace message severity.
	trace = ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_TRACE,
	
	/// Debug message severity.
	debug = ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_DEBUG,
	
	/// Info message severity.
	info = ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_INFO,
	
	/// Warning message severity.
	warning = ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_WARNING,
	
	/// Error message severity.
	error = ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_ERROR,
	
	/// Fatal error message severity.
	fatal = ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_FATAL,
};

/// @}

} // namespace debug

#endif // ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_HPP
