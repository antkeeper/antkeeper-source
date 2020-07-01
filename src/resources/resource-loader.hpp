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

#include <istream>
#include <ostream>

class resource_manager;

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
	 * @param is Input stream containing the resource data.
	 * @return Pointer to the loaded resource.
	 */
	static T* load(resource_manager* resourceManager, std::istream* is);

	/**
	 * Saves resource data.
	 *
	 * @param resourceManager Pointer to a resource manager.
	 * @param os Output stream which will contain the resource data.
	 * @param resource Pointer to the resource data.
	 */
	static void save(resource_manager* resourceManager, std::ostream* os, const T* resource);
};

#endif // RESOURCE_LOADER_HPP

