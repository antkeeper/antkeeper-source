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

#ifndef ANTKEEPER_RESOURCE_MANAGER_HPP
#define ANTKEEPER_RESOURCE_MANAGER_HPP

#include "resource-handle.hpp"
#include "resource-loader.hpp"
#include "debug/logger.hpp"
#include <fstream>
#include <list>
#include <map>
#include <stdexcept>
#include <string>
#include <entt/entt.hpp>
#include <physfs.h>

/**
 * Loads resources.
 */
class resource_manager
{
public:
	/**
	 * Creates a resource manager.
	 */
	resource_manager(::logger* logger);

	/**
	 * Destroys a resource manager and frees all of its resources.
	 */
	~resource_manager();
	
	bool mount(const std::string& path);

	/**
	 * Adds a path to be searched when a resource is requested.
	 *
	 * @param path Search path.
	 */
	void include(const std::string& path);

	/**
	 * Loads the requested resource. If the resource has already been loaded it will be retrieved from the resource cache and its reference count incremented.
	 *
	 * @tparam T Resource type.
	 * @param path Path to the resource, relative to the search paths.
	 * @return Pointer to the requested resource, or nullptr if the resource could not be found nor loaded.
	 */
	template <typename T>
	T* load(const std::string& name);

	/**
	 * Decrements a resource's reference count and unloads the resource if it's unreferenced.
	 *
	 * @param path Path to the resource, relative to the search paths.
	 */
	void unload(const std::string& name);

	/**
	 * Saves the specified resource.
	 *
	 * @tparam T Resource type.
	 * @param resource Pointer to the resource.
	 * @param path Path to the resource.
	 */
	template <typename T>
	void save(const T* resource, const std::string& path);

	entt::registry& get_archetype_registry();

private:
	std::map<std::string, resource_handle_base*> resource_cache;
	std::list<std::string> search_paths;
	entt::registry archetype_registry;
	logger* logger;
};

template <typename T>
T* resource_manager::load(const std::string& name)
{
	// Check if resource is in the cache
	auto it = resource_cache.find(name);
	if (it != resource_cache.end())
	{
		/*
		if (logger)
		{
			logger->log("Fetched resource \"" + name + "\"");
		}
		*/
		
		// Resource found
		resource_handle<T>* resource = static_cast<resource_handle<T>*>(it->second);

		// Increment resource handle reference count
		++resource->reference_count;

		// Return resource data
		return resource->data;
	}
	
	if (logger)
	{
		logger->push_task("Loading resource \"" + name + "\"");
	}

	// Resource not cached, look for file in search paths
	T* data = nullptr;
	bool found = false;
	for (const std::string& search_path: search_paths)
	{
		std::string path = search_path + name;
		
		// Check if file exists
		if (!PHYSFS_exists(path.c_str()))
		{
			continue;
		}
		
		// File found
		found = true;
		
		// Open file for reading
		PHYSFS_File* file = PHYSFS_openRead(path.c_str());
		if (!file)
		{
			logger->error(std::string("PhysicsFS error: ") + PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			break;
		}

		// Load opened file
		try
		{
			data = resource_loader<T>::load(this, file);
		}
		catch (const std::exception& e)
		{
			logger->error("Failed to load resource: \"" + std::string(e.what()) + "\"");
		}
		
		// Close opened file
		if (!PHYSFS_close(file))
		{
			logger->error(std::string("PhysicsFS error: ") + PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}
		
		break;
	}
	
	if (!data)
	{
		if (!found)
		{
			logger->error("File not found");
		}
	
		logger->pop_task(EXIT_FAILURE);
		return nullptr;
	}

	// Create a resource handle for the resource data
	resource_handle<T>* resource = new resource_handle<T>();
	resource->data = data;
	resource->reference_count = 1;
	
	// Add resource to the cache
	resource_cache[name] = resource;
	
	if (logger)
	{
		logger->pop_task(EXIT_SUCCESS);
	}

	return resource->data;
}

template <typename T>
void resource_manager::save(const T* resource, const std::string& path)
{
	logger->push_task("Saving resource to \"" + path + "\"");
	
	// Open file for writing
	PHYSFS_File* file = PHYSFS_openWrite(path.c_str());
	if (!file)
	{
		logger->error(std::string("PhysicsFS error: ") + PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		logger->pop_task(EXIT_FAILURE);
		return;
	}
	
	// Save to opened file
	int status = EXIT_SUCCESS;
	try
	{
		resource_loader<T>::save(this, file, resource);
	}
	catch (const std::exception& e)
	{
		logger->error("Failed to save resource: \"" + std::string(e.what()) + "\"");
		status = EXIT_FAILURE; 
	}
	
	// Close opened file
	if (!PHYSFS_close(file))
	{
		logger->error(std::string("PhysicsFS error: ") + PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		status = EXIT_FAILURE;
	}
	
	logger->pop_task(status)
}

inline entt::registry& resource_manager::get_archetype_registry()
{
	return archetype_registry;
}

#endif // ANTKEEPER_RESOURCE_MANAGER_HPP

