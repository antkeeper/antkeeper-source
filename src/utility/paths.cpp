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

#include "paths.hpp"
#include <cstddef>
#include <limits.h>
#include <stdexcept>

#if defined(_WIN32)
	#include <Shlobj.h>
	#include <windows.h>
#else
	#include <pwd.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif

#if defined(_WIN32)
	std::string narrow(const std::wstring& wstring)
	{
		std::string string(WideCharToMultiByte(CP_UTF8, 0, &wstring[0], static_cast<int>(wstring.size()), nullptr, 0, nullptr, nullptr), '\0');
		WideCharToMultiByte(CP_UTF8, 0, &wstring[0], static_cast<int>(wstring.size()), &string[0], static_cast<int>(string.size()), nullptr, nullptr);
		return string;
	}

	std::wstring widen(const std::string& string)
	{
		std::wstring wstring(MultiByteToWideChar(CP_UTF8, 0, &string[0], static_cast<int>(string.size()), nullptr, 0), L'\0');
		MultiByteToWideChar(CP_UTF8, 0, &string[0], static_cast<int>(string.size()), &wstring[0], static_cast<int>(wstring.size()));
		return wstring;
	}
#endif

std::filesystem::path get_executable_path()
{
	std::filesystem::path executable_path;
	
	#if defined(_WIN32)
		// Get executable path on Windows
		HMODULE hModule = GetModuleHandleW(nullptr);
		std::wstring wpath(MAX_PATH, L'\0');
		GetModuleFileNameW(hModule, &wpath[0], MAX_PATH);
		wpath.erase(std::find(wpath.begin(), wpath.end(), L'\0'), wpath.end());
		executable_path = narrow(wpath);
	#else
		// Get executable path on Linux
		char path[PATH_MAX];
		ssize_t length = ::readlink("/proc/self/exe", path, sizeof(path) - 1);
		if (length != -1)
		{
			path[length] = '\0';
			executable_path = path;
		}
	#endif

	return executable_path;
}

std::filesystem::path get_data_path(const std::string& application_name)
{
	#if defined(_WIN32)
		return get_executable_path().parent_path();
	#else
		return get_executable_path().parent_path().parent_path() / "share" / application_name;
	#endif
}

std::filesystem::path get_config_path(const std::string& application_name)
{
	std::filesystem::path config_path;
	
	#if defined(_WIN32)
		std::wstring wpath(MAX_PATH, L'\0');
		if (SHGetSpecialFolderPathW(nullptr, &wpath[0], CSIDL_LOCAL_APPDATA, FALSE))
		{
			wpath.erase(std::find(wpath.begin(), wpath.end(), L'\0'), wpath.end());
			config_path = std::filesystem::path(narrow(wpath)) / application_name;
		}
	#else
		// Determine home path
		std::filesystem::path home_path = getpwuid(getuid())->pw_dir;

		// Determine config path
		char* xdgConfigHome = std::getenv("XDG_CONFIG_HOME");
		if (!xdgConfigHome)
		{
			// Default to $HOME/.config/ as per:
			// https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html#variables
			config_path = home_path / ".config/" / application_name;
		}
		else
		{
			config_path = std::filesystem::path(xdgConfigHome) / application_name;
		}
	#endif
	
	return config_path;
}
