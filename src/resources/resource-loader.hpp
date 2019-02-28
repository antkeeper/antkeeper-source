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

#ifndef RESOURCE_LOADER_HPP
#define RESOURCE_LOADER_HPP

#include <istream>

class ResourceManager;

/**
 * Templated resource class. Different resources types should specialize the constructor and destructor to load and free resource data, respectively.
 */
template <typename T>
class ResourceLoader
{
public:
	/**
	 * Loads resource data.
	 */
	static T* load(ResourceManager* resourceManager, std::istream* is);
};

#endif // RESOURCE_LOADER_HPP

