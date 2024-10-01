// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/resources/resource-manager.hpp>
#include <engine/debug/log.hpp>
#include <engine/resources/physfs/physfs-deserialize-context.hpp>
#include <engine/resources/physfs/physfs-serialize-context.hpp>
#include <physfs.h>
#include <stdexcept>

resource_manager::resource_manager()
{
	// Init PhysicsFS
	debug::log_debug("Initializing PhysicsFS...");

	#if defined(DEBUG)
		// Log PhysicsFS info
		PHYSFS_Version physfs_compiled_version;
		PHYSFS_Version physfs_linked_version;
		PHYSFS_VERSION(&physfs_compiled_version);
		PHYSFS_getLinkedVersion(&physfs_linked_version);
		debug::log_trace
		(
			"PhysicsFS compiled version: {}.{}.{}; linked version: {}.{}.{}",
			physfs_compiled_version.major,
			physfs_compiled_version.minor,
			physfs_compiled_version.patch,
			physfs_linked_version.major,
			physfs_linked_version.minor,
			physfs_linked_version.patch
		);
	#endif // DEBUG
	
	if (!PHYSFS_init(nullptr))
	{
		auto error_message = std::format("Failed to initialize PhysicsFS: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		debug::log_error("{}", error_message);
		debug::log_debug("Initializing PhysicsFS... FAILED");
		throw std::runtime_error(std::move(error_message));
	}
	else
	{
		debug::log_debug("Initializing PhysicsFS... OK");
	}
}

resource_manager::~resource_manager()
{
	// Deinit PhysicsFS
	debug::log_debug("Deinitializing PhysicsFS...");
	if (!PHYSFS_deinit())
	{
		debug::log_error("Failed to deinitialize PhysicsFS: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		debug::log_debug("Deinitializing PhysicsFS... FAILED");
	}
	else
	{
		debug::log_debug("Deinitializing PhysicsFS... OK");
	}
}

bool resource_manager::mount(const std::filesystem::path& path)
{
	const std::string path_string = path.string();
	
	debug::log_debug("Mounting path \"{}\"...", path_string);
	
	if (!PHYSFS_mount(path_string.c_str(), nullptr, 1))
	{
		debug::log_error("Failed to mount path \"{}\": {}", path_string, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		debug::log_debug("Mounting path \"{}\"... FAILED", path_string);
		return false;
	}
	
	debug::log_debug("Mounting path \"{}\"... OK", path_string);
	
	return true;
}

bool resource_manager::unmount(const std::filesystem::path& path)
{
	const std::string path_string = path.string();
	
	debug::log_debug("Unmounting path \"{}\"...", path_string);
	
	if (!PHYSFS_unmount(path_string.c_str()))
	{
		debug::log_error("Failed to unmount path \"{}\": {}", path_string, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		debug::log_debug("Unmounting path \"{}\"... FAILED", path_string);
		return false;
	}
	
	debug::log_debug("Unmounting path \"{}\"... OK", path_string);
	
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
	
	debug::log_debug("Set write path to \"{}\"", path_string);
	
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
			debug::log_debug("Fetched expired resource from cache \"{}\"", path.string());
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
