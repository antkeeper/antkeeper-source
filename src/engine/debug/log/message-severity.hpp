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

#ifndef ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_HPP
#define ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_HPP

#include <cstdint>

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

namespace debug {
namespace log {

/// Log message severity levels.
enum class message_severity: std::uint8_t
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

} // namespace log
} // namespace debug

#endif // ANTKEEPER_DEBUG_LOG_MESSAGE_SEVERITY_HPP
