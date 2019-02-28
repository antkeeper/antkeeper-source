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

#include "paths.hpp"
#include <cstdlib>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdexcept>

#include <iostream>

static std::string getExecutablePath()
{
	char path[PATH_MAX];
	ssize_t length = ::readlink("/proc/self/exe", path, sizeof(path) - 1);
	if (length != -1)
	{
		path[length] = '\0';
		return std::string(path);
	}

	return std::string();
}

std::string getDataPath()
{
	std::string executablePath = getExecutablePath();
	std::size_t delimeter =  executablePath.find_last_of("\\/") + 1;
	std::string executableName = executablePath.substr(delimeter, executablePath.size() - delimeter + 1);
	return executablePath.substr(0, delimeter) + std::string("../share/") + executableName + std::string("/");
}

std::string getConfigPath()
{
	std::string executablePath = getExecutablePath();
	std::size_t delimeter =  executablePath.find_last_of("\\/") + 1;
	std::string executableName = executablePath.substr(delimeter, executablePath.size() - delimeter + 1);
	std::string configPath;

	// Determine home path
	std::string homePath = std::string(getpwuid(getuid())->pw_dir);

	// Determine config path
	char* xdgConfigHome = std::getenv("XDG_CONFIG_HOME");
	if (!xdgConfigHome)
	{
		// Default to $HOME/.config/ as per:
		// https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html#variables
		configPath = homePath + std::string("/.config/") + executableName + std::string("/");
	}
	else
	{
		configPath = xdgConfigHome + std::string("/") + executableName + std::string("/");
	}
	
	return configPath;
}

bool pathExists(const std::string& path)
{
	struct stat info;
	return (stat(path.c_str(), &info) == 0);
}

bool createDirectory(const std::string& path)
{
	int error = 0;
	#if defined(_WIN32)
		error = _mkdir(path.c_str());
	#else 
		error = mkdir(path.c_str(), 0777);
	#endif
	
	return (error == 0);
}

