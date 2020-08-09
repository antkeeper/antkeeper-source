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

#ifndef RESOURCE_LOADER_HPP
#define RESOURCE_LOADER_HPP

#include <string>

class resource_manager;
struct PHYSFS_File;

/**
 * Templated resource loader.
 *
 * @tparam Type of resource which this loader handles.
 */
template <typename T>
class resource_loader
{
public:
	/**
	 * Loads resource data.
	 *
	 * @param resourceManager Pointer to a resource manager which will manage this resource.
	 * @param file PhysicsFS file handle.
	 * @return Pointer to the loaded resource.
	 */
	static T* load(resource_manager* resourceManager, PHYSFS_File* file);

	/**
	 * Saves resource data.
	 *
	 * @param resourceManager Pointer to a resource manager.
	 * @param file PhysicsFS file handle.
	 * @param resource Pointer to the resource data.
	 */
	static void save(resource_manager* resourceManager, PHYSFS_File* file, const T* resource);
};

/// getline function for PhysicsFS file handles
void physfs_getline(PHYSFS_File* file, std::string& line);

#endif // RESOURCE_LOADER_HPP
