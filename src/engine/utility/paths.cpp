// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/utility/paths.hpp>
#include <cstdlib>

#if defined(_WIN32)
	#include <system_error>
	#include <windows.h>
	#include <Shlobj.h>
#endif

std::filesystem::path executable_path()
{
	#if defined(_WIN32)

		wchar_t buffer[MAX_PATH];
		if (!GetModuleFileNameW(nullptr, buffer, MAX_PATH))
		{
			std::error_code ec(GetLastError(), std::system_category());
			throw std::filesystem::filesystem_error(ec.message(), ec);
		}
		return std::filesystem::path(buffer);

	#else

		return std::filesystem::read_symlink("/proc/self/exe");

	#endif
}

std::filesystem::path executable_data_directory_path()
{
	#if defined(_WIN32)
		return executable_path().parent_path();
	#else
		return executable_path().parent_path().parent_path() / "share";
	#endif
}

std::filesystem::path local_config_directory_path()
{
	#if defined(_WIN32)
		
		wchar_t buffer[MAX_PATH];
		const auto result = SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, buffer);
		if (!SUCCEEDED(result))
		{
			std::error_code ec(static_cast<int>(result), std::system_category());
			throw std::filesystem::filesystem_error(ec.message(), ec);
		}
		return std::filesystem::path(buffer);
		
	#else

		if (const char* xdg_config_home = std::getenv("XDG_CONFIG_HOME"))
		{
			return std::filesystem::path(xdg_config_home);
		}

		if (const char* home = std::getenv("HOME"))
		{
			return std::filesystem::path(home) / ".config";
		}

		return std::filesystem::path();

	#endif
}

std::filesystem::path shared_config_directory_path()
{
	#if defined(_WIN32)

		wchar_t buffer[MAX_PATH];
		const auto result = SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, buffer);
		if (!SUCCEEDED(result))
		{
			std::error_code ec(static_cast<int>(result), std::system_category());
			throw std::filesystem::filesystem_error(ec.message(), ec);
		}
		return std::filesystem::path(buffer);

	#else

		return local_config_directory_path();

	#endif
}
