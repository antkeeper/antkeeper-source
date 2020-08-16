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

resource_manager::resource_manager(::logger* logger):
	logger(logger)
{
	// Init PhysicsFS
	logger->push_task("Initializing PhysicsFS");
	if (!PHYSFS_init(nullptr))
	{
		logger->error(std::string("PhysicsFS error: ") + PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		logger->pop_task(EXIT_FAILURE);
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}
}

resource_manager::~resource_manager()
{
	// Delete cached resources
	for (auto it = resource_cache.begin(); it != resource_cache.end(); ++it)
	{
		delete it->second;
	}
	
	// Deinit PhysicsFS
	logger->push_task("Deinitializing PhysicsFS");
	if (!PHYSFS_deinit())
	{
		logger->error(std::string("PhysicsFS error: ") + PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		logger->pop_task(EXIT_FAILURE);
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}
}

bool resource_manager::mount(const std::string& path)
{
	logger->push_task("Mounting path \"" + path + "\"");
	if (!PHYSFS_mount(path.c_str(), nullptr, 1))
	{
		logger->error(std::string("PhysicsFS error: ") + PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		logger->pop_task(EXIT_FAILURE);
		return false;
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
		return true;
	}
}

void resource_manager::unload(const std::string& name)
{
	// Check if resource is in the cache
	auto it = resource_cache.find(name);
	if (it != resource_cache.end())
	{
		// Decrement the resource handle reference count
		--it->second->reference_count;

		// Free the resource if the resource handle is unreferenced
		if (it->second->reference_count <= 0)
		{
			if (logger)
			{
				logger->push_task("Unloading resource \"" + name + "\"");
			}
			
			delete it->second;
			
			if (logger)
			{
				logger->pop_task(EXIT_SUCCESS);
			}
		}

		// Remove resource from the cache
		resource_cache.erase(it);
	}
}

void resource_manager::include(const std::string& search_path)
{
	search_paths.push_back(search_path);
}

