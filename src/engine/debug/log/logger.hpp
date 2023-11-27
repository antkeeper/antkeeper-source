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

#ifndef ANTKEEPER_DEBUG_LOGGER_HPP
#define ANTKEEPER_DEBUG_LOGGER_HPP

#include <engine/debug/log/log-message-severity.hpp>
#include <engine/debug/log/log-events.hpp>
#include <engine/event/publisher.hpp>
#include <source_location>
#include <string>

namespace debug {

/// @name Debug logging
/// @{

/**
 * Generates an event each time a message logged.
 */
class logger
{
public:
	/**
	 * Logs a message.
	 *
	 * @param message Message contents.
	 * @param severity Message severity.
	 * @param location Source location from which the message was sent.
	 */
	void log
	(
		std::string&& message,
		log_message_severity severity = log_message_severity::info,
		std::source_location&& location = std::source_location::current()
	);
	
	/// Returns the channel through which message logged events are published.
	[[nodiscard]] inline ::event::channel<message_logged_event>& get_message_logged_channel() noexcept
	{
		return m_message_logged_publisher.channel();
	}
	
private:
	::event::publisher<message_logged_event> m_message_logged_publisher;
};

/// @}

} // namespace debug

#endif // ANTKEEPER_DEBUG_LOGGER_HPP
