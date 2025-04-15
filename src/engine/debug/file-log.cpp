// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <syncstream>
import engine.debug.file_log;
import engine.debug.contract;
import engine.debug.logger;
import <filesystem>;
import <format>;
import <stdexcept>;

namespace engine::debug
{
	file_log::file_log(const std::filesystem::path& path)
	{
		// Open log file
		m_output_stream.open(path.string());
		if (!m_output_stream.is_open())
		{
			throw std::runtime_error(std::format("Failed to open log file \"{}\"", path.string()));
		}

		// Write log file header
		m_output_stream << "time\tseverity\tfile\tline\tthread\tmessage";
		if (!m_output_stream.good())
		{
			throw std::runtime_error(std::format("Failed to write to log file \"{}\"", path.string()));
		}

		// Get current time zone
		m_time_zone = std::chrono::current_zone();

		// Subscribe to log messages from default logger
		m_message_logged_subscription = default_logger().message_logged_channel().subscribe
		(
			[this](const auto& event)
			{
				this->message_logged(event);
			}
		);

		debug::postcondition(m_time_zone);
	}

	file_log::~file_log()
	{}

	void file_log::message_logged(const message_logged_event& event)
	{
		debug::precondition(m_time_zone);

		// Round time to the millisecond and convert to local time zone
		const std::chrono::zoned_time zoned_time{m_time_zone, std::chrono::floor<std::chrono::milliseconds>(event.time)};

		// Format and output log message
		std::osyncstream(m_output_stream) << std::format
		(
			"\n{:%FT%T%Ez}\t{}\t{}\t{}\t{}\t{}",
			zoned_time,
			log_message_severity_to_string(event.severity),
			std::filesystem::path(event.location.file_name()).filename().string(),
			event.location.line(),
			event.thread_id,
			event.message
		);
	}
}
