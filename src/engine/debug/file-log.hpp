// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_DEBUG_FILE_LOG_HPP
#define ANTKEEPER_DEBUG_FILE_LOG_HPP

#include <engine/debug/log-events.hpp>
#include <engine/event/subscription.hpp>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <memory>

namespace debug {

/// @name Logging
/// @{

/**
 * Logs messages to a file.
 */
class file_log
{
public:
	/**
	 * Opens a file log.
	 *
	 * @param path Path to the log file.
	 * 
	 * @exception std::runtime_error Failed to open log file.
	 * @exception std::runtime_error Failed to write to log file.
	 * @exception std::runtime_error Failed to get current time zone.
	 */
	file_log(const std::filesystem::path& path);

	/// Closes a file log.
	~file_log();

private:
	void message_logged(const message_logged_event& event);

	file_log(const file_log&) = delete;
	file_log(file_log&&) = delete;
	file_log& operator=(const file_log&) = delete;
	file_log& operator=(file_log&&) = delete;

	std::ofstream m_output_stream;
	const std::chrono::time_zone* m_time_zone{};
	std::shared_ptr<event::subscription> m_message_logged_subscription;
};

/// @}

} // namespace debug

#endif // ANTKEEPER_DEBUG_FILE_LOG_HPP
