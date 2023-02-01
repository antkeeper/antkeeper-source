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

#ifndef ANTKEEPER_DEBUG_LOG_EVENT_HPP
#define ANTKEEPER_DEBUG_LOG_EVENT_HPP

#include "debug/log/message-severity.hpp"
#include <chrono>
#include <source_location>
#include <string>
#include <thread>

namespace debug {
namespace log {

class logger;

/**
 * Debug logging events.
 */
namespace event {

/**
 * Event generated when a message has been logged.
 */
struct message_logged
{
	/// Logger which received the message.
	log::logger* logger;
	
	/// Time at which the message was sent.
	std::chrono::time_point<std::chrono::system_clock> time;
	
	/// ID of the thread from which the message was sent.
	std::thread::id thread_id;
	
	/// Source location from which the message was sent.
	std::source_location location;
	
	/// Severity of the message.
	message_severity severity;
	
	/// Message contents.
	std::string message;
};

} // namespace event
} // namespace log
} // namespace debug

#endif // ANTKEEPER_DEBUG_LOG_EVENT_HPP
