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

#include "resources/resource-manager.hpp"

resource_manager::resource_manager():
	logger(nullptr)
{}

resource_manager::~resource_manager()
{
	for (auto it = resource_cache.begin(); it != resource_cache.end(); ++it)
	{
		delete it->second;
	}
}

void resource_manager::unload(const std::string& path)
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
			delete it->second;
		}

		// Remove resource from the cache
		resource_cache.erase(it);
	}
}

void resource_manager::include(const std::string& path)
{
	paths.push_back(path);
}

void resource_manager::set_logger(::logger* logger)
{
	this->logger = logger;
}

