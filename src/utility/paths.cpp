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

#include "paths.hpp"
#include <cstdlib>
#include <limits.h>
#include <stdexcept>
#include <iostream>

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

std::string get_executable_path()
{
	std::string executable_path;
	
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

std::string get_data_path(const std::string& application_name)
{
	std::string data_path;
	
	#if defined(_WIN32)
		std::string executable_path = get_executable_path();
		std::size_t delimeter =  executable_path.find_last_of("\\/") + 1;
		data_path = executable_path.substr(0, delimeter);
	#else
		std::string executable_path = get_executable_path();
		std::size_t delimeter =  executable_path.find_last_of("\\/") + 1;
		data_path = executable_path.substr(0, delimeter) + std::string("../share/") + application_name + std::string("/");
	#endif
	
	return data_path;
}

std::string get_config_path(const std::string& application_name)
{
	std::string config_path;
	
	#if defined(_WIN32)
		std::wstring wpath(MAX_PATH, L'\0');
		if (SHGetSpecialFolderPathW(nullptr, &wpath[0], CSIDL_LOCAL_APPDATA, FALSE))
		{
			wpath.erase(std::find(wpath.begin(), wpath.end(), L'\0'), wpath.end());
			config_path = narrow(wpath);
			config_path += std::string("\\") + application_name + std::string("\\");
		}
	#else
		// Determine home path
		std::string home_path = std::string(getpwuid(getuid())->pw_dir);

		// Determine config path
		char* xdgConfigHome = std::getenv("XDG_CONFIG_HOME");
		if (!xdgConfigHome)
		{
			// Default to $HOME/.config/ as per:
			// https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html#variables
			config_path = home_path + std::string("/.config/") + application_name + std::string("/");
		}
		else
		{
			config_path = xdgConfigHome + std::string("/") + application_name + std::string("/");
		}
	#endif
	
	return config_path;
}

bool path_exists(const std::string& path)
{
	#if defined(_WIN32)
		std::wstring wpath = widen(path);
		DWORD attributes = GetFileAttributesW(wpath.c_str());
		return (attributes != INVALID_FILE_ATTRIBUTES);
	#else
		struct stat info;
		return (stat(path.c_str(), &info) == 0);
	#endif
}

bool create_directory(const std::string& path)
{
	#if defined(_WIN32)
		std::wstring wpath = widen(path);
		return (CreateDirectoryW(wpath.c_str(), nullptr) != 0);
	#else 
		return (mkdir(path.c_str(), 0777) == 0);
	#endif
}

