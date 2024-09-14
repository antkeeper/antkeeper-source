// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_UTILITY_PATHS_HPP
#define ANTKEEPER_UTILITY_PATHS_HPP

#include <filesystem>

/**
 * Returns the absolute path to the current executable.
 *
 * @return Path to the current executable.
 *
 * @exception std::filesystem::filesystem_error Failed to get executable path.
 */
[[nodiscard]] std::filesystem::path executable_path();

/**
 * Returns the absolute path to the directory containing application data.
 *
 * - Windows: ` <folder containing executable>`
 * - GNU/Linux: `executable_path()/../share`
 *
 * @return Path to the data directory of the current executable.
 *
 * @exception std::filesystem::filesystem_error Failed to get executable data path.
 */
[[nodiscard]] std::filesystem::path executable_data_directory_path();

/**
 * Returns the absolute path to the directory containing user-specific, device-specific application data.
 *
 * - Windows: `%LOCALAPPDATA%`
 * - GNU/Linux: `$XDG_CONFIG_HOME` or `~/.config` if `$XDG_CONFIG_HOME` is not set.
 *
 * @return Path to the local config directory.
 */
[[nodiscard]] std::filesystem::path local_config_directory_path();

/**
 * Returns the absolute path to the directory containing user-specific application data that may be shared across devices.
 *
 * - Windows: `%APPDATA%`
 * - GNU/Linux: `$XDG_CONFIG_HOME` or `~/.config`, if `$XDG_CONFIG_HOME` is not set.
 *
 * @return Path to the shared config directory.
 */
[[nodiscard]] std::filesystem::path shared_config_directory_path();

#endif // ANTKEEPER_UTILITY_PATHS_HPP
