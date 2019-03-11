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

#include "filesystem.hpp"
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

std::string getExecutablePath()
{
	std::string executablePath;
	
	#if defined(_WIN32)
		// Get executable path on Windows
		HMODULE hModule = GetModuleHandleW(nullptr);
		std::wstring wpath(MAX_PATH, L'\0');
		GetModuleFileNameW(hModule, &wpath[0], MAX_PATH);
		wpath.erase(std::find(wpath.begin(), wpath.end(), L'\0'), wpath.end());
		executablePath = narrow(wpath);
	#else
		// Get executable path on Linux
		char path[PATH_MAX];
		ssize_t length = ::readlink("/proc/self/exe", path, sizeof(path) - 1);
		if (length != -1)
		{
			path[length] = '\0';
			executablePath = path;
		}
	#endif

	return executablePath;
}

std::string getDataPath(const std::string& applicationName)
{
	std::string dataPath;
	
	#if defined(_WIN32)
		std::string executablePath = getExecutablePath();
		std::size_t delimeter =  executablePath.find_last_of("\\/") + 1;
		dataPath = executablePath.substr(0, delimeter);
	#else
		std::string executablePath = getExecutablePath();
		std::size_t delimeter =  executablePath.find_last_of("\\/") + 1;
		dataPath = executablePath.substr(0, delimeter) + std::string("../share/") + applicationName + std::string("/");
	#endif
	
	return dataPath;
}

std::string getConfigPath(const std::string& applicationName)
{
	std::string configPath;
	
	#if defined(_WIN32)
		std::wstring wpath(MAX_PATH, L'\0');
		if (SHGetSpecialFolderPathW(nullptr, &wpath[0], CSIDL_LOCAL_APPDATA, FALSE))
		{
			wpath.erase(std::find(wpath.begin(), wpath.end(), L'\0'), wpath.end());
			configPath = narrow(wpath);
			configPath += std::string("\\") + applicationName + std::string("\\");
		}
	#else
		// Determine home path
		std::string homePath = std::string(getpwuid(getuid())->pw_dir);

		// Determine config path
		char* xdgConfigHome = std::getenv("XDG_CONFIG_HOME");
		if (!xdgConfigHome)
		{
			// Default to $HOME/.config/ as per:
			// https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html#variables
			configPath = homePath + std::string("/.config/") + applicationName + std::string("/");
		}
		else
		{
			configPath = xdgConfigHome + std::string("/") + applicationName + std::string("/");
		}
	#endif
	
	return configPath;
}

bool pathExists(const std::string& path)
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

bool createDirectory(const std::string& path)
{
	#if defined(_WIN32)
		std::wstring wpath = widen(path);
		return (CreateDirectoryW(wpath.c_str(), nullptr) != 0);
	#else 
		return (mkdir(path.c_str(), 0777) == 0);
	#endif
}
