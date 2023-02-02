/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.hpp"
#include "debug/console.hpp"
#include "debug/log.hpp"
#include "game/context.hpp"
#include "game/state/boot.hpp"
#include "utility/ansi.hpp"
#include "utility/paths.hpp"
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <syncstream>

int main(int argc, char* argv[])
{
	// Enable VT100 sequences in console for colored text
	debug::console::enable_vt100();
	
	// Subscribe log to cout function to message logged events
	auto log_to_cout_subscription = debug::log::default_logger().get_message_logged_channel().subscribe
	(
		[&](const auto& event)
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
			
			std::osyncstream(std::cout) << std::format
			(
				"{}:{}:{}: {}{}: {}{}\n",
				std::filesystem::path(event.location.file_name()).filename().string(),
				event.location.line(),
				event.location.column(),
				colors[static_cast<int>(event.severity)],
				severities[static_cast<int>(event.severity)],
				event.message,
				ansi::reset
			);
		}
	);
	
	// Determine path to log archive
	const std::filesystem::path log_archive_path = get_config_path(config::application_name) / "logs";
	
	// Set up log archive
	bool log_archive_exists = false;
	try
	{
		// Create log archive if it doesn't exist
		if (std::filesystem::create_directories(log_archive_path))
		{
			debug::log::debug("Created log archive \"{}\"", log_archive_path.string());
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
						entry.path().extension() == ".log")
					{
						log_archive.emplace(entry.path());
					}
				}
				
				debug::log::debug("Detected {} archived log{} at \"{}\"", log_archive.size(), log_archive.size() != 1 ? "s" : "", log_archive_path.string());
				
				// Delete expired logs
				if (!log_archive.empty())
				{
					for (std::size_t i = log_archive.size() + 1; i > config::debug_log_archive_capacity; --i)
					{
						std::filesystem::remove(*log_archive.begin());
						debug::log::debug("Deleted expired log file \"{}\"", log_archive.begin()->string());
						log_archive.erase(log_archive.begin());
					}
				}
			}
			catch (const std::filesystem::filesystem_error& e)
			{
				debug::log::error("An error occured while cleaning the log archive \"{}\": {}", log_archive_path.string(), e.what());
			}
		}
		
		log_archive_exists = true;
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		debug::log::error("Failed to create log archive at \"{}\": {}", log_archive_path.string(), e.what());
	}
	
	// Set up logging to file
	std::shared_ptr<event::subscription> log_to_file_subscription;
	if (config::debug_log_archive_capacity && log_archive_exists)
	{
		// Determine log filename
		const auto time = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
		const std::string log_filename = std::format("{0}-{1:%Y%m%d}T{1:%H%M%S}Z.log", config::application_name, time);
		
		// Open log file
		std::filesystem::path log_filepath = log_archive_path / log_filename;
		const std::string log_filepath_string = log_filepath.string();
		auto log_filestream = std::make_shared<std::ofstream>(log_filepath);
		
		if (log_filestream->is_open())
		{
			debug::log::debug("Opened log file \"{}\"", log_filepath_string);
			
			// Write log file header
			(*log_filestream) << "time\tfile\tline\tcolumn\tseverity\tmessage";
			
			if (log_filestream->good())
			{
				// Subscribe log to file function to message logged events
				log_to_file_subscription = debug::log::default_logger().get_message_logged_channel().subscribe
				(
					[log_filestream](const auto& event)
					{
						std::osyncstream(*log_filestream) << std::format
						(
							"\n{0:%Y%m%d}T{0:%H%M%S}Z\t{1}\t{2}\t{3}\t{4}\t{5}",
							std::chrono::floor<std::chrono::milliseconds>(event.time),
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
				debug::log::error("Failed to write to log file \"{}\"", log_filepath_string);
			}
		}
		else
		{
			debug::log::error("Failed to open log file \"{}\"", log_filepath_string);
		}
	}
	
	// Log application name and version string
	debug::log::info("{} v{}", config::application_name, config::application_version_string);
	
	try
	{
		// Allocate game context
		game::context ctx;
		
		// Enter boot state
		ctx.state_machine.emplace(new game::state::boot(ctx, argc, argv));
	}
	catch (const std::exception& e)
	{
		debug::log::fatal("Unhandled exception: {}", e.what());
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
