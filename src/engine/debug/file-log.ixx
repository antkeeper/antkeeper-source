// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.debug.file_log;
import engine.debug.log_events;
import engine.event.subscription;
export import <filesystem>;
import <chrono>;
import <fstream>;
import <memory>;

export namespace engine::debug
{
	/// @name Logging
	/// @{

	/// Logs messages to a file.
	class file_log
	{
	public:
		/// Opens a file log.
		/// @param path Path to the log file.
		/// @exception std::runtime_error Failed to open log file.
		/// @exception std::runtime_error Failed to write to log file.
		/// @exception std::runtime_error Failed to get current time zone.
		explicit file_log(const std::filesystem::path& path);

		/// Closes a file log.
		~file_log();

	private:
		/// Logs a message to a file.
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
}
