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

#ifndef ANTKEEPER_UTILITY_PATHS_HPP
#define ANTKEEPER_UTILITY_PATHS_HPP

#include <filesystem>

/**
 * Returns the absolute path to the current executable.
 *
 * @return Path to the application's executable.
 */
[[nodiscard]] std::filesystem::path get_executable_path();

/**
 * Returns the absolute path to the directory containing application data.
 *
 * Windows: ` get_executable_path()`
 * GNU/Linux: ` get_executable_path()/../share/`
 *
 * @return Path to the application's data directory.
 */
[[nodiscard]] std::filesystem::path get_executable_data_path();

/**
 * Returns the absolute path to the directory containing user-specific, device-specific application data.
 *
 * Windows: `%LOCALAPPDATA%`
 * GNU/Linux: `$XDG_CONFIG_HOME` or `~/.config/` if `$XDG_CONFIG_HOME` is not set.
 *
 * @return Path to the local config directory.
 */
[[nodiscard]] std::filesystem::path get_local_config_path();

/**
 * Returns the absolute path to the directory containing user-specific application data that may be shared across devices.
 *
 * Windows: `%APPDATA%`
 * GNU/Linux: `$XDG_CONFIG_HOME` or `~/.config/` if `$XDG_CONFIG_HOME` is not set.
 *
 * @return Path to the shared config directory.
 */
[[nodiscard]] std::filesystem::path get_shared_config_path();

#endif // ANTKEEPER_UTILITY_PATHS_HPP
