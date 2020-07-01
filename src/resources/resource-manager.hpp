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
#include <fstream>
#include <list>
#include <map>
#include <stdexcept>
#include <string>
#include <entt/entt.hpp>

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
	T* load(const std::string& path);

	/**
	 * Decrements a resource's reference count and unloads the resource if it's unreferenced.
	 *
	 * @param path Path to the resource, relative to the search paths.
	 */
	void unload(const std::string& path);

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
	std::list<std::string> paths;
	entt::registry archetype_registry;
};

template <typename T>
T* resource_manager::load(const std::string& path)
{
	// Check if resource is in the cache
	auto it = resource_cache.find(path);
	if (it != resource_cache.end())
	{
		// Resource found
		resource_handle<T>* resource = static_cast<resource_handle<T>*>(it->second);

		// Increment resource handle reference count
		++resource->reference_count;

		// Return resource data
		return resource->data;
	}

	// Resource not found, load resource data
	T* data = nullptr;
	try
	{
		// For each directory in search paths
		bool opened = false;
		for (const std::string& directory: paths)
		{
			// Attempt to open file
			std::string full_path = directory + path;
			std::ifstream fs;
			fs.open(full_path.c_str(), std::ios::in | std::ios::binary);

			// If unable to open file
			if (!fs.is_open() || !fs.good())
			{
				if (fs.is_open())
				{
					fs.close();
				}

				// Try again in next search path
				continue;
			}

			// File opened, load it
			opened = true;
			data = resource_loader<T>::load(this, &fs);
			fs.close();
			break;
		}

		if (!opened)
		{
			throw std::runtime_error("resource_manager::load<T>(): Unable to open file \"" + path + "\"");
		}
	}
	catch (const std::exception& e)
	{
		std::string error = std::string("resource_manager::load<T>(): Failed to load resource \"") + path + std::string("\": \"") + e.what() + std::string("\"");
		throw std::runtime_error(error.c_str());
	}

	// Create a resource handle for the resource data
	resource_handle<T>* resource = new resource_handle<T>();
	resource->data = data;
	resource->reference_count = 1;
	
	// Add resource to the cache
	resource_cache[path] = resource;

	return resource->data;
}

template <typename T>
void resource_manager::save(const T* resource, const std::string& path)
{
	// Attempt to open file
	std::ofstream fs;
	fs.open(path.c_str(), std::ios::out | std::ios::binary);

	// If unable to open file
	if (!fs.is_open() || !fs.good())
	{
		if (fs.is_open())
		{
			fs.close();
		}

		throw std::runtime_error("resource_manager::save<T>(): Unable to open file \"" + path + "\"");
	}

	try
	{
		resource_loader<T>::save(this, &fs, resource);
	}
	catch (const std::exception& e)
	{
		std::string error = std::string("resource_manager::load<T>(): Failed to save resource \"") + path + std::string("\": \"") + e.what() + std::string("\"");
		throw std::runtime_error(error.c_str());
	}
}

inline entt::registry& resource_manager::get_archetype_registry()
{
	return archetype_registry;
}

#endif // ANTKEEPER_RESOURCE_MANAGER_HPP

