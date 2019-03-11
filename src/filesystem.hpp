/*
 * Copyright (C) 2017-2019  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>

/**
 * Returns the absolute path to the current executable.
 *
 * @return Path to the application's executable.
 */
std::string getExecutablePath();

/**
 * Returns the absolute path to the directory containing application data.
 *
 * Windows: executableDirectory
 * GNU/Linux: executableDirectory/../share/applicationName/
 *
 * @param applicationName Name of the application.
 * @return Path to the application's data directory.
 */
std::string getDataPath(const std::string& applicationName);

/**
 * Returns the absolute path to the directory containing user-specific application data.
 *
 * Windows: %LOCALAPPDATA%\applicationName\
 * GNU/Linux: $XDG_CONFIG_HOME/applicationName/ or ~/.config/applicationName/ if $XDG_CONFIG_HOME is not set.
 *
 * @param applicationName Name of the application.
 * @return Path to the application's config directory.
 */
std::string getConfigPath(const std::string& applicationName);

/// Checks if a file or directory exists
bool pathExists(const std::string& path);

/// Creates a directory
bool createDirectory(const std::string& path);

#endif // FILESYSTEM_HPP

