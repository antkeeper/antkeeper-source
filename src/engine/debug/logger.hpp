// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_DEBUG_LOGGER_HPP
#define ANTKEEPER_DEBUG_LOGGER_HPP

#include <engine/debug/log-message-severity.hpp>
#include <engine/debug/log-events.hpp>
#include <engine/event/publisher.hpp>
#include <source_location>
#include <string>

namespace debug {

/// @name Logging
/// @{

/**
 * Generates an event each time a message is logged.
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
		log_message_severity severity,
		std::source_location&& location = std::source_location::current()
	);
	
	/** Returns the channel through which message logged events are published. */
	[[nodiscard]] inline auto& message_logged_channel() noexcept
	{
		return m_message_logged_publisher.channel();
	}
	
private:
	event::publisher<message_logged_event> m_message_logged_publisher;
};

/**
 * Returns the default logger.
 */
[[nodiscard]] logger& default_logger() noexcept;

/// @}

} // namespace debug

#endif // ANTKEEPER_DEBUG_LOGGER_HPP
