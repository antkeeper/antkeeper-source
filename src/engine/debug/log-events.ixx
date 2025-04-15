// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.debug.log_events;
import engine.debug.log_message_severity;
export import <chrono>;
export import <source_location>;
export import <string>;
export import <thread>;

export namespace engine::debug
{
	class logger;

	/// @name Logging
	/// @{

	/// Event generated when a message has been logged.
	struct message_logged_event
	{
		/// Logger which received the message.
		debug::logger* logger{};
	
		/// Time at which the message was sent.
		std::chrono::time_point<std::chrono::system_clock> time{};
	
		/// ID of the thread from which the message was sent.
		std::thread::id thread_id{};
	
		/// Source location from which the message was sent.
		std::source_location location{};
	
		/// Severity of the message.
		log_message_severity severity{};
	
		/// Message contents.
		std::string message;
	};

	/// @}
}
