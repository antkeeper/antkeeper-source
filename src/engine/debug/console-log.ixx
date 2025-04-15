// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.debug.console_log;
import engine.debug.log_events;
import engine.event.subscription;
import <chrono>;
import <memory>;

export namespace engine::debug
{
	/// @name Logging
	/// @{

	/// Logs messages to the console.
	class console_log
	{
	public:
		/// Opens a console log.
		/// @exception std::runtime_error Failed to get current time zone.
		console_log();

		/// Closes a console log.
		~console_log();

	private:
		/// Logs a message to the console.
		void message_logged(const message_logged_event& event);

		console_log(const console_log&) = delete;
		console_log(console_log&&) = delete;
		console_log& operator=(const console_log&) = delete;
		console_log& operator=(console_log&&) = delete;

		const std::chrono::time_zone* m_time_zone{};
		std::shared_ptr<event::subscription> m_message_logged_subscription;
	};

	/// @}
}
