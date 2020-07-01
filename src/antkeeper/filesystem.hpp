/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_FILESYSTEM_HPP
#define ANTKEEPER_FILESYSTEM_HPP

#include <string>

/**
 * Returns the absolute path to the current executable.
 *
 * @return Path to the application's executable.
 */
std::string get_executable_path();

/**
 * Returns the absolute path to the directory containing application data.
 *
 * Windows: executable_directory
 * GNU/Linux: executable_directory/../share/applicationName/
 *
 * @param application_name Name of the application.
 * @return Path to the application's data directory.
 */
std::string get_data_path(const std::string& application_name);

/**
 * Returns the absolute path to the directory containing user-specific application data.
 *
 * Windows: %LOCALAPPDATA%\application_name\
 * GNU/Linux: $XDG_CONFIG_HOME/application_name/ or ~/.config/application_name/ if $XDG_CONFIG_HOME is not set.
 *
 * @param application_name Name of the application.
 * @return Path to the application's config directory.
 */
std::string get_config_path(const std::string& application_name);

/// Checks if a file or directory exists
bool path_exists(const std::string& path);

/// Creates a directory
bool create_directory(const std::string& path);

#endif // ANTKEEPER_FILESYSTEM_HPP

