// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/game.hpp"
#include <SDL2/SDL.h>
#include <chrono>
#include <engine/config.hpp>
#include <engine/debug/console.hpp>
#include <engine/debug/log.hpp>
#include <engine/utility/ansi.hpp>
#include <engine/utility/paths.hpp>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <syncstream>

int main(int argc, char* argv[])
{
	// Get time at which the application launched 
	const auto launch_time = std::chrono::system_clock::now();
	
	// Enable console UTF-8 output and VT100 sequences (for colored text)
	debug::console::enable_utf8();
	debug::console::enable_vt100();
	
	// Subscribe log to cout function to message logged events
	auto log_to_cout_subscription = debug::default_logger().get_message_logged_channel().subscribe
	(
		[&launch_time](const auto& event)
		{
			static const char* severities[] =
			{
				"trace",
				"debug",
				"info",
				"warning",
				"error",
				"fatal"
			};
			
			static const std::string colors[] =
			{
				std::format("{}", ansi::fg_white),
				std::format("{}", ansi::fg_bright_blue),
				std::format("{}", ansi::fg_bright_green),
				std::format("{}", ansi::fg_yellow),
				std::format("{}", ansi::fg_red),
				std::format("{}{}", ansi::fg_white, ansi::bg_bright_red)
			};
			
			std::osyncstream((static_cast<int>(event.severity) > 3) ? std::cerr : std::cout) << std::format
			(
				"[{:8.03f}] {}{}: {}:{}:{}: {}{}\n",
				std::chrono::duration<float>(event.time - launch_time).count(),
				colors[static_cast<int>(event.severity)],
				//severities[static_cast<int>(event.severity)],
				static_cast<int>(event.severity),
				std::filesystem::path(event.location.file_name()).filename().string(),
				event.location.line(),
				event.location.column(),
				event.message,
				ansi::reset
			);
		}
	);
	
	// Determine path to log archive
	const std::filesystem::path log_archive_path = get_shared_config_path() / config::application_name / "logs";
	
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
		else
		{
			// Clean pre-existing log archive
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
					for (std::size_t i = log_archive.size() + 1; i > config::debug_log_archive_capacity; --i)
					{
						std::filesystem::remove(*log_archive.begin());
						debug::log_debug("Deleted expired log file \"{}\"", log_archive.begin()->string());
						log_archive.erase(log_archive.begin());
					}
				}
			}
			catch (const std::filesystem::filesystem_error& e)
			{
				debug::log_error("An error occured while cleaning the log archive \"{}\": {}", log_archive_path.string(), e.what());
			}
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
		const auto time = std::chrono::floor<std::chrono::seconds>(launch_time);
		const std::string log_filename = std::format("{0}{1:%Y%m%d}T{1:%H%M%S}Z{2}", log_stem_prefix, time, log_extension);
		
		// Open log file
		log_filepath = log_archive_path / log_filename;
		const std::string log_filepath_string = log_filepath.string();
		auto log_filestream = std::make_shared<std::ofstream>(log_filepath);
		
		if (log_filestream->is_open())
		{
			debug::log_debug("Opened log file \"{}\"", log_filepath_string);
			
			// Write log file header
			(*log_filestream) << "time\tfile\tline\tcolumn\tseverity\tmessage";
			
			if (log_filestream->good())
			{
				// Subscribe log to file function to message logged events
				log_to_file_subscription = debug::default_logger().get_message_logged_channel().subscribe
				(
					[&launch_time, log_filestream](const auto& event)
					{
						std::osyncstream(*log_filestream) << std::format
						(
							"\n{:.03f}\t{}\t{}\t{}\t{}\t{}",
							std::chrono::duration<float>(event.time - launch_time).count(),
							std::filesystem::path(event.location.file_name()).filename().string(),
							event.location.line(),
							event.location.column(),
							static_cast<int>(event.severity),
							event.message
						);
					}
				);
				
				// Unsubscribe log to cout function from message logged events on release builds
				#if defined(NDEBUG)
					log_to_cout_subscription->unsubscribe();
				#endif
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
	
	// Log application name and version string, followed by launch time
	debug::log_info("{0} {1}; {2:%Y%m%d}T{2:%H%M%S}Z", config::application_name, config::application_version_string, std::chrono::floor<std::chrono::milliseconds>(launch_time));
	
	// Start marker
	debug::log_debug("Hi! 🐜");
	
	// #if defined(NDEBUG)
	try
	{
		game(argc, argv).execute();
	}
	catch (const std::exception& e)
	{
		debug::log_fatal("Unhandled exception: {}", e.what());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", std::format("Unhandled exception: {}", e.what()).c_str(), nullptr);
		
		return EXIT_FAILURE;
	}
	// #else
		// game(argc, argv).execute();
	// #endif
	
	// Clean exit marker
	debug::log_debug("Bye! 🐜");
	
	return EXIT_SUCCESS;
}
