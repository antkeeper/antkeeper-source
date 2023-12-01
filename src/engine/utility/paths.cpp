// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/utility/paths.hpp>
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

std::filesystem::path get_executable_path()
{
	std::filesystem::path executable_path;
	
	#if defined(_WIN32)
		// Get executable path on Windows
		std::wstring path(MAX_PATH, L'\0');
		GetModuleFileNameW(GetModuleHandleW(nullptr), path.data(), MAX_PATH);
		path.erase(std::find(path.begin(), path.end(), L'\0'), path.end());
		executable_path = path;
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

std::filesystem::path get_executable_data_path()
{
	#if defined(_WIN32)
		return get_executable_path().parent_path();
	#else
		return get_executable_path().parent_path().parent_path() / "share";
	#endif
}

std::filesystem::path get_local_config_path()
{
	std::filesystem::path local_config_path;
	
	#if defined(_WIN32)
		
		std::wstring path(MAX_PATH, L'\0');
		if (SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, path.data()) == S_OK)
		{
			path.erase(std::find(path.begin(), path.end(), L'\0'), path.end());
			local_config_path = path;
		}
		
		// Windows Vista+
		// wchar_t* path_buffer = nullptr;
		// if (SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT, nullptr, &path_buffer) == S_OK)
		// {
			// local_config_path = std::filesystem::path(path_buffer);
			// CoTaskMemFree(static_cast<void*>(path_buffer));
		// }
		
	#else
		// Determine home path
		std::filesystem::path home_path = getpwuid(getuid())->pw_dir;

		// Determine config path
		char* xdg_config_home = std::getenv("XDG_CONFIG_HOME");
		if (!xdg_config_home)
		{
			// Default to $HOME/.config/ as per:
			// https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html#variables
			local_config_path = home_path / ".config/";
		}
		else
		{
			local_config_path = xdg_config_home;
		}
	#endif
	
	return local_config_path;
}

std::filesystem::path get_shared_config_path()
{
	#if defined(_WIN32)
		std::filesystem::path shared_config_path;
		
		std::wstring path(MAX_PATH, L'\0');
		if (SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, path.data()) == S_OK)
		{
			path.erase(std::find(path.begin(), path.end(), L'\0'), path.end());
			shared_config_path = path;
		}
		
		// Windows Vista+
		// wchar_t* path_buffer = nullptr;
		// if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, nullptr, &path_buffer) == S_OK)
		// {
			// shared_config_path = path_buffer;
			// CoTaskMemFree(static_cast<void*>(path_buffer));
		// }
		
		return shared_config_path;
	#else
		return get_local_config_path();
	#endif
}
