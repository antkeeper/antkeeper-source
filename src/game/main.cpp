// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/game.hpp"
#include <chrono>
#include <engine/config.hpp>
#include <engine/debug/console.hpp>
#include <engine/debug/log.hpp>
#include <engine/debug/crash-reporter.hpp>
#include <engine/utility/ansi.hpp>
#include <engine/utility/paths.hpp>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <syncstream>

int main(int argc, char* argv[])
{
	// Get time at which the application was launched
	const auto launch_time = std::chrono::system_clock::now();

	#if !defined(NDEBUG)

		// Enable console UTF-8 output and VT100 sequences (for colored text)
		debug::console::enable_utf8();
		debug::console::enable_vt100();

		// Set up logging to cout/cerr
		auto log_to_cout_subscription = debug::default_logger().message_logged_channel().subscribe
		(
			[&launch_time](const auto& event)
			{
				static const std::string colors[] =
				{
					std::format("{}", ansi::fg_white),
					std::format("{}", ansi::fg_bright_blue),
					std::format("{}", ansi::fg_bright_green),
					std::format("{}", ansi::fg_yellow),
					std::format("{}", ansi::fg_red),
					std::format("{}{}", ansi::fg_white, ansi::bg_bright_red)
				};

				auto& output_stream = std::to_underlying(event.severity) >= std::to_underlying(debug::log_message_severity::error) ? std::cerr : std::cout;
				
				std::osyncstream(output_stream) << std::format
				(
					"[{:%T}] {}{:7}: {}:{}: {}{}\n",
					std::chrono::floor<std::chrono::milliseconds>(event.time - launch_time),
					colors[static_cast<int>(event.severity)],
					debug::log_message_severity_to_string(event.severity),
					std::filesystem::path(event.location.file_name()).filename().string(),
					event.location.line(),
					event.message,
					ansi::reset
				);
			}
		);

	#endif

	const auto& shared_config_directory = shared_config_directory_path();

	// Set up crash reporting
	debug::crash_reporter crash_reporter;
	crash_reporter.set_report_directory_path(shared_config_directory / config::application_name / "crash-reports");
	crash_reporter.set_report_prefix(std::string(config::application_slug) + "-crash-");
	debug::set_crash_reporter(&crash_reporter);

	// Determine path to log archive
	const std::filesystem::path log_archive_path = shared_config_directory / config::application_name / "logs";
	
	// Determine log file prefix
	const std::string log_stem_prefix = std::format("{}-log-", config::application_slug);
	const std::string log_extension = ".txt";
	
	// Set up log archive
	bool log_archive_exists = false;
	try
	{
		// Create log archive if it doesn't exist
		if (std::filesystem::create_directories(log_archive_path))
		{
			debug::log_debug("Created log archive \"{}\"", log_archive_path.string());
		}
		
		log_archive_exists = true;
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		debug::log_error("Failed to create log archive at \"{}\": {}", log_archive_path.string(), e.what());
	}
	
	// Set up logging to file
	std::shared_ptr<event::subscription> log_to_file_subscription;
	std::filesystem::path log_filepath;
	if (config::debug_log_archive_capacity && log_archive_exists)
	{
		// Determine log filename
		const std::string log_filename = std::format("{0}{1:%Y%m%d}T{1:%H%M%S}Z{2}", log_stem_prefix, std::chrono::floor<std::chrono::seconds>(launch_time), log_extension);
		
		// Open log file
		log_filepath = log_archive_path / log_filename;
		const std::string log_filepath_string = log_filepath.string();
		auto log_filestream = std::make_shared<std::ofstream>(log_filepath);
		
		if (log_filestream->is_open())
		{
			debug::log_debug("Opened log file \"{}\"", log_filepath_string);
			
			// Write log file header
			(*log_filestream) << "time\tseverity\tfile\tline\tthread\tmessage";
			
			if (log_filestream->good())
			{
				// Subscribe log to file function to message logged events
				log_to_file_subscription = debug::default_logger().message_logged_channel().subscribe
				(
					[&launch_time, log_filestream](const auto& event)
					{
						std::osyncstream(*log_filestream) << std::format
						(
							"\n{:%T}\t{}\t{}\t{}\t{}\t{}",
							std::chrono::floor<std::chrono::milliseconds>(event.time - launch_time),
							debug::log_message_severity_to_string(event.severity),
							std::filesystem::path(event.location.file_name()).filename().string(),
							event.location.line(),
							event.thread_id,
							event.message
						);
					}
				);
			}
			else
			{
				debug::log_error("Failed to write to log file \"{}\"", log_filepath_string);
			}
		}
		else
		{
			debug::log_error("Failed to open log file \"{}\"", log_filepath_string);
		}
	}
	
	// Start marker
	debug::log_debug("Hi! 🐜");

	// Log version and launch time
	debug::log_info("{0} version: {1}; launch time: {2:%Y%m%d}T{2:%H%M%S}Z", config::application_name, config::application_version, std::chrono::floor<std::chrono::milliseconds>(launch_time));

	// Launch game
	game(argc, argv).execute();

	// Clean log archive
	if (log_archive_exists)
	{
		try
		{
			// Detect and sort archived logs
			std::set<std::filesystem::path> log_archive;
			for (const auto& entry: std::filesystem::directory_iterator{log_archive_path})
			{
				if (entry.is_regular_file() &&
					entry.path().extension() == log_extension &&
					entry.path().stem().string().starts_with(log_stem_prefix))
				{
					log_archive.emplace(entry.path());
				}
			}
			
			debug::log_debug("Detected {} archived log{} at \"{}\"", log_archive.size(), log_archive.size() != 1 ? "s" : "", log_archive_path.string());
			
			// Delete expired logs
			if (!log_archive.empty())
			{
				for (std::size_t i = log_archive.size(); i > config::debug_log_archive_capacity; --i)
				{
					if (std::filesystem::remove(*log_archive.begin()))
					{
						debug::log_debug("Deleted expired log file \"{}\"", log_archive.begin()->string());
					}

					log_archive.erase(log_archive.begin());
				}
			}
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			debug::log_error("A filesystem error occured while cleaning the log archive \"{}\": {}", log_archive_path.string(), e.what());
		}
	}
	
	// Clean exit marker
	debug::log_debug("Bye! 🐜");
	
	return EXIT_SUCCESS;
}
