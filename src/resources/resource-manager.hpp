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

#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include "resource-handle.hpp"
#include "resource-loader.hpp"
#include <fstream>
#include <list>
#include <map>
#include <stdexcept>
#include <string>

/**
 * Loads resources.
 */
class ResourceManager
{
public:
	/**
	 * Creates a resource manager.
	 */
	ResourceManager();

	/**
	 * Destroys a resource manager and frees all of its resources.
	 */
	~ResourceManager();

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

private:
	std::map<std::string, ResourceHandleBase*> resourceCache;
	std::list<std::string> paths;
};

template <typename T>
T* ResourceManager::load(const std::string& path)
{
	// Check if resource is in the cache
	auto it = resourceCache.find(path);
	if (it != resourceCache.end())
	{
		// Resource found
		ResourceHandle<T>* resource = static_cast<ResourceHandle<T>*>(it->second);

		// Increment resource handle reference count
		++resource->referenceCount;

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
			std::string fullPath = directory + path;
			std::ifstream fs;
			fs.open(fullPath.c_str(), std::ios::in | std::ios::binary);

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
			data = ResourceLoader<T>::load(this, &fs);
			fs.close();
			break;
		}

		if (!opened)
		{
			throw std::runtime_error("ResourceManager::load<T>(): Unable to open file \"" + path + "\"");
		}
	}
	catch (const std::exception& e)
	{
		std::string error = std::string("ResourceManager::load<T>(): Failed to load resource \"") + path + std::string("\": \"") + e.what() + std::string("\"");
		throw std::runtime_error(error.c_str());
	}

	// Create a resource handle for the resource data
	ResourceHandle<T>* resource = new ResourceHandle<T>();
	resource->data = data;
	resource->referenceCount = 1;
	
	// Add resource to the cache
	resourceCache[path] = resource;

	return resource->data;
}

template <typename T>
void ResourceManager::save(const T* resource, const std::string& path)
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

		throw std::runtime_error("ResourceManager::save<T>(): Unable to open file \"" + path + "\"");
	}

	try
	{
		ResourceLoader<T>::save(this, &fs, resource);
	}
	catch (const std::exception& e)
	{
		std::string error = std::string("ResourceManager::load<T>(): Failed to save resource \"") + path + std::string("\": \"") + e.what() + std::string("\"");
		throw std::runtime_error(error.c_str());
	}
}

#endif // RESOURCE_MANAGER_HPP

