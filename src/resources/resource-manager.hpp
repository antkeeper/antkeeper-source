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

#ifndef ANTKEEPER_RESOURCES_RESOURCE_MANAGER_HPP
#define ANTKEEPER_RESOURCES_RESOURCE_MANAGER_HPP

#include "resources/resource-handle.hpp"
#include "resources/resource-loader.hpp"
#include "debug/log.hpp"
#include <filesystem>
#include <list>
#include <map>
#include <physfs.h>
#include <stdexcept>
#include <string>

/**
 * Loads resources.
 */
class resource_manager
{
public:
	/**
	 * Creates a resource manager.
	 */
	resource_manager();

	/**
	 * Destroys a resource manager and frees all of its resources.
	 */
	~resource_manager();
	
	bool mount(const std::filesystem::path& path);

	/**
	 * Adds a path to be searched when a resource is requested.
	 *
	 * @param path Search path.
	 */
	void include(const std::filesystem::path& path);

	/**
	 * Loads the requested resource. If the resource has already been loaded it will be retrieved from the resource cache and its reference count incremented.
	 *
	 * @tparam T Resource type.
	 * @param path Path to the resource, relative to the search paths.
	 * @return Pointer to the requested resource, or nullptr if the resource could not be found nor loaded.
	 */
	template <typename T>
	T* load(const std::filesystem::path& path);

	/**
	 * Decrements a resource's reference count and unloads the resource if it's unreferenced.
	 *
	 * @param path Path to the resource, relative to the search paths.
	 */
	void unload(const std::filesystem::path& path);

	/**
	 * Saves the specified resource.
	 *
	 * @tparam T Resource type.
	 * @param resource Pointer to the resource.
	 * @param path Path to the resource.
	 */
	template <typename T>
	void save(const T* resource, const std::filesystem::path& path);

private:
	std::map<std::filesystem::path, resource_handle_base*> resource_cache;
	std::list<std::filesystem::path> search_paths;
};

template <typename T>
T* resource_manager::load(const std::filesystem::path& path)
{
	// Check if resource is in the cache
	auto it = resource_cache.find(path);
	if (it != resource_cache.end())
	{
		//debug::log::trace("Fetched cached resource \"{}\"". path.string());
		
		// Resource found
		resource_handle<T>* resource = static_cast<resource_handle<T>*>(it->second);
		
		// Increment resource handle reference count
		++resource->reference_count;
		
		// Return resource data
		return resource->data;
	}
	
	debug::log::trace("Loading resource \"{}\"...", path.string());
	
	// Resource not cached, look for file in search paths
	T* data = nullptr;
	bool found = false;
	for (const std::filesystem::path& search_path: search_paths)
	{
		std::filesystem::path full_path = search_path / path;
		
		// Check if file exists
		if (!PHYSFS_exists(full_path.string().c_str()))
		{
			continue;
		}
		
		// File found
		found = true;
		
		// Open file for reading
		PHYSFS_File* file = PHYSFS_openRead(full_path.string().c_str());
		if (!file)
		{
			debug::log::error("Failed to load resource \"{}\": {}", path.string(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			break;
		}
		
		// Load opened file
		try
		{
			data = resource_loader<T>::load(this, file, full_path);
		}
		catch (const std::exception& e)
		{
			debug::log::error("Failed to load resource \"{}\": {}", path.string(), e.what());
		}
		
		// Close opened file
		if (!PHYSFS_close(file))
		{
			debug::log::error("Failed to close resource file \"{}\": {}", path.string(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}
		
		break;
	}
	
	if (!data)
	{
		if (!found)
		{
			debug::log::error("Failed to load resource \"{}\": file not found", path.string());
		}
		
		return nullptr;
	}

	// Create a resource handle for the resource data
	resource_handle<T>* resource = new resource_handle<T>();
	resource->data = data;
	resource->reference_count = 1;
	
	// Add resource to the cache
	resource_cache[path] = resource;
	
	debug::log::trace("Loaded resource \"{}\"", path.string());

	return resource->data;
}

template <typename T>
void resource_manager::save(const T* resource, const std::filesystem::path& path)
{
	debug::log::trace("Saving resource to \"{}\"", path.string());
	
	// Open file for writing
	PHYSFS_File* file = PHYSFS_openWrite(path.string().c_str());
	if (!file)
	{
		debug::log::error("Failed to save resource to \"{}\": {}", path.string(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		return;
	}
	
	// Save to opened file
	try
	{
		resource_loader<T>::save(this, file, path, resource);
		debug::log::trace("Saved resource to \"{}\"", path.string());
	}
	catch (const std::exception& e)
	{
		debug::log::error("Failed to save resource to \"{}\": {}", e.what());
	}
	
	// Close opened file
	if (!PHYSFS_close(file))
	{
		debug::log::error("Failed to close file \"{}\": {}", path.string(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}
}

#endif // ANTKEEPER_RESOURCES_RESOURCE_MANAGER_HPP
