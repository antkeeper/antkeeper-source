// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.debug.logger;
import <chrono>;
import <thread>;
import <utility>;

namespace engine::debug
{
	void logger::log(log_message_severity severity, std::string&& message, std::source_location&& location)
	{
		// Generate message logged event
		m_message_logged_publisher.publish
		(
			{
				this,
				std::chrono::system_clock::now(),
				std::this_thread::get_id(),
				std::move(location),
				severity,
				std::move(message)
			}
		);
	}

	logger& default_logger() noexcept
	{
		static logger instance;
		return instance;
	}
}
