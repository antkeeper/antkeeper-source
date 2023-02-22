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

#include <engine/resources/resource-manager.hpp>
#include <engine/debug/log.hpp>
#include <stdexcept>

resource_manager::resource_manager()
{
	// Log PhysicsFS info
	// PHYSFS_Version physfs_compiled_version;
	// PHYSFS_Version physfs_linked_version;
	// PHYSFS_VERSION(&physfs_compiled_version);
	// PHYSFS_getLinkedVersion(&physfs_linked_version);
	// debug::log::info
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
	debug::log::trace("Initializing PhysicsFS...");
	if (!PHYSFS_init(nullptr))
	{
		debug::log::error("Failed to initialize PhysicsFS: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		throw std::runtime_error("Failed to initialize PhysicsFS");
	}
	else
	{
		debug::log::trace("Initialized PhysicsFS");
	}
}

resource_manager::~resource_manager()
{
	debug::log::trace("Deleting cached resources...");
	
	// Delete cached resources
	for (auto it = resource_cache.begin(); it != resource_cache.end(); ++it)
	{
		delete it->second;
	}
	
	debug::log::trace("Deleted cached resources");
	
	// Deinit PhysicsFS
	debug::log::trace("Deinitializing PhysicsFS...");
	if (!PHYSFS_deinit())
	{
		debug::log::error("Failed to deinitialize PhysicsFS: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}
	else
	{
		debug::log::trace("Deinitialized PhysicsFS");
	}
}

bool resource_manager::mount(const std::filesystem::path& path)
{
	debug::log::trace("Mounting path \"{}\"...", path.string());
	if (!PHYSFS_mount(path.string().c_str(), nullptr, 1))
	{
		debug::log::error("Failed to mount path \"{}\": {}", path.string(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		return false;
	}
	else
	{
		debug::log::trace("Mounted path \"{}\"", path.string());
		return true;
	}
}

void resource_manager::set_write_dir(const std::filesystem::path& path)
{
	if (!PHYSFS_setWriteDir(path.string().c_str()))
	{
		debug::log::error("Failed set write directory to \"{}\": {}", path.string(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}
	else
	{
		debug::log::trace("Set write directory to \"{}\"", path.string());
	}
}

void resource_manager::unload(const std::filesystem::path& path)
{
	// Check if resource is in the cache
	auto it = resource_cache.find(path);
	if (it != resource_cache.end())
	{
		// Decrement the resource handle reference count
		--it->second->reference_count;
		
		// Free the resource if the resource handle is unreferenced
		if (it->second->reference_count <= 0)
		{
			debug::log::trace("Unloading resource \"{}\"...", path.string());
			
			delete it->second;
			
			debug::log::trace("Unloaded resource \"{}\"", path.string());
		}
		
		// Remove resource from the cache
		resource_cache.erase(it);
	}
}

void resource_manager::include(const std::filesystem::path& search_path)
{
	search_paths.push_back(search_path);
}
