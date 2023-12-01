// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
