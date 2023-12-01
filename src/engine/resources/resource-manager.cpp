// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/resources/resource-manager.hpp>
#include <engine/debug/log.hpp>
#include <engine/resources/physfs/physfs-deserialize-context.hpp>
#include <engine/resources/physfs/physfs-serialize-context.hpp>
#include <physfs.h>
#include <stdexcept>

resource_manager::resource_manager()
{
	// Log PhysicsFS info
	// PHYSFS_Version physfs_compiled_version;
	// PHYSFS_Version physfs_linked_version;
	// PHYSFS_VERSION(&physfs_compiled_version);
	// PHYSFS_getLinkedVersion(&physfs_linked_version);
	// debug::log_info
	// (
		// "PhysicsFS compiled version: {}.{}.{}; linked version: {}.{}.{}",
		// physfs_compiled_version.major,
		// physfs_compiled_version.minor,
		// physfs_compiled_version.patch,
		// physfs_linked_version.major,
		// physfs_linked_version.minor,
		// physfs_linked_version.patch
	// );
	
	// Init PhysicsFS
	debug::log_trace("Initializing PhysicsFS...");
	if (!PHYSFS_init(nullptr))
	{
		debug::log_error("Failed to initialize PhysicsFS: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		throw std::runtime_error("Failed to initialize PhysicsFS");
	}
	else
	{
		debug::log_trace("Initialized PhysicsFS");
	}
}

resource_manager::~resource_manager()
{
	// Deinit PhysicsFS
	debug::log_trace("Deinitializing PhysicsFS...");
	if (!PHYSFS_deinit())
	{
		debug::log_error("Failed to deinitialize PhysicsFS: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}
	else
	{
		debug::log_trace("Deinitialized PhysicsFS");
	}
}

bool resource_manager::mount(const std::filesystem::path& path)
{
	const std::string path_string = path.string();
	
	debug::log_trace("Mounting path \"{}\"...", path_string);
	
	if (!PHYSFS_mount(path_string.c_str(), nullptr, 1))
	{
		debug::log_error("Failed to mount path \"{}\": {}", path_string, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		return false;
	}
	
	debug::log_trace("Mounted path \"{}\"", path_string);
	
	return true;
}

bool resource_manager::unmount(const std::filesystem::path& path)
{
	const std::string path_string = path.string();
	
	debug::log_trace("Unmounting path \"{}\"...", path_string);
	
	if (!PHYSFS_unmount(path_string.c_str()))
	{
		debug::log_error("Failed to unmount path \"{}\": {}", path_string, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		return false;
	}
	
	debug::log_trace("Unmounted path \"{}\"", path_string);
	
	return true;
}

bool resource_manager::set_write_path(const std::filesystem::path& path)
{
	const std::string path_string = path.string();
	
	if (!PHYSFS_setWriteDir(path_string.c_str()))
	{
		debug::log_error("Failed set write path to \"{}\": {}", path_string, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		return false;
	}
	
	write_path = path;
	
	debug::log_trace("Set write path to \"{}\"", path_string);
	
	return true;
}

std::shared_ptr<void> resource_manager::fetch(const std::filesystem::path& path) const
{
	if (auto i = resource_cache.find(path); i != resource_cache.end())
	{
		if (!i->second.expired())
		{
			return i->second.lock();
		}
		else
		{
			debug::log_trace("Fetched expired resource from cache \"{}\"", path.string());
		}
	}
	
	return nullptr;
}

std::unique_ptr<deserialize_context> resource_manager::open_read(const std::filesystem::path& path) const
{
	auto ctx = std::make_unique<physfs_deserialize_context>(path);
	if (!ctx->is_open())
	{
		debug::log_error("Failed to open file \"{}\" for reading: {}", path.string(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		return nullptr;
	}
	
	return ctx;
}

std::unique_ptr<serialize_context> resource_manager::open_write(const std::filesystem::path& path) const
{
	auto ctx = std::make_unique<physfs_serialize_context>(path);
	if (!ctx->is_open())
	{
		debug::log_error("Failed to open file \"{}\" for writing: {}", path.string(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		return nullptr;
	}
	
	return ctx;
}
