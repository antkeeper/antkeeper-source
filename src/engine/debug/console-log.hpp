// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_DEBUG_CONSOLE_LOG_HPP
#define ANTKEEPER_DEBUG_CONSOLE_LOG_HPP

#include <engine/debug/log-events.hpp>
#include <engine/event/subscription.hpp>
#include <chrono>
#include <memory>

namespace debug {

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

} // namespace debug

#endif // ANTKEEPER_DEBUG_CONSOLE_LOG_HPP
