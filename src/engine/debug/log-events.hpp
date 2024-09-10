// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_DEBUG_LOG_EVENTS_HPP
#define ANTKEEPER_DEBUG_LOG_EVENTS_HPP

#include <engine/debug/log-message-severity.hpp>
#include <chrono>
#include <source_location>
#include <string>
#include <thread>

namespace debug {

class logger;

/// @name Debug logging
/// @{

/**
 * Event generated when a message has been logged.
 */
struct message_logged_event
{
	/** Logger which received the message. */
	debug::logger* logger;
	
	/** Time at which the message was sent. */
	std::chrono::time_point<std::chrono::system_clock> time;
	
	/** ID of the thread from which the message was sent. */
	std::thread::id thread_id;
	
	/** Source location from which the message was sent. */
	std::source_location location;
	
	/** Severity of the message. */
	log_message_severity severity;
	
	/** Message contents. */
	std::string message;
};

/// @}

} // namespace debug

#endif // ANTKEEPER_DEBUG_LOG_EVENTS_HPP
