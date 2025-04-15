// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.debug.logger;
export import engine.debug.log_events;
export import engine.debug.log_message_severity;
import engine.event.publisher;
export import <string>;
import <source_location>;

export namespace engine::debug
{
	/// @name Logging
	/// @{

	/// Generates an event each time a message is logged.
	class logger
	{
	public:
		/// Logs a message.
		/// @param message Message contents.
		/// @param severity Message severity.
		/// @param location Source location from which the message was sent.
		void log
		(
			log_message_severity severity,
			std::string&& message,
			std::source_location&& location = std::source_location::current()
		);
	
		/// Returns the channel through which message logged events are published.
		[[nodiscard]] inline auto& message_logged_channel() noexcept
		{
			return m_message_logged_publisher.channel();
		}
	
	private:
		event::publisher<message_logged_event> m_message_logged_publisher;
	};

	/// Returns the default logger.
	[[nodiscard]] logger& default_logger() noexcept;

	/// @}
}
