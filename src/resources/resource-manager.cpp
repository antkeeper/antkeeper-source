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

#include "resource-manager.hpp"

ResourceManager::ResourceManager()
{}

ResourceManager::~ResourceManager()
{
	for (auto it = resourceCache.begin(); it != resourceCache.end(); ++it)
	{
		delete it->second;
	}
}

void ResourceManager::unload(const std::string& path)
{
	// Check if resource is in the cache
	auto it = resourceCache.find(path);
	if (it != resourceCache.end())
	{
		// Decrement the resource handle reference count
		--it->second->referenceCount;

		// Free the resource if the resource handle is unreferenced
		if (it->second->referenceCount <= 0)
		{
			delete it->second;
		}

		// Remove resource from the cache
		resourceCache.erase(it);
	}
}

void ResourceManager::include(const std::string& path)
{
	paths.push_back(path);
}

