// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/game.hpp"
#include <chrono>
#include <engine/config.hpp>
#include <engine/debug/log.hpp>
#include <engine/debug/console-log.hpp>
#include <engine/debug/file-log.hpp>
#include <engine/debug/crash-reporter.hpp>
#include <engine/utility/paths.hpp>
#include <set>

int main(int argc, char* argv[])
{
	// Open console log
	#if !defined(NDEBUG)
		debug::console_log console_log;
	#endif

	// Get time at which the application was launched
	const auto launch_time = std::chrono::system_clock::now();

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
	const std::string log_extension = ".tsv";
	
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

	// Open file log
	std::unique_ptr<debug::file_log> file_log;
	if (config::debug_log_archive_capacity && log_archive_exists)
	{
		const auto log_filename = std::format("{0}{1:%Y%m%d}T{1:%H%M%S}Z{2}", log_stem_prefix, std::chrono::floor<std::chrono::seconds>(launch_time), log_extension);
		file_log = std::make_unique<debug::file_log>(log_archive_path / log_filename);
	}
	
	// Start marker
	debug::log_debug("Hi! 🐜");

	// Log version string
	debug::log_info("{0} v{1}", config::application_name, config::application_version);

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
