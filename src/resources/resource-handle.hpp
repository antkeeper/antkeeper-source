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

#ifndef RESOURCE_HANDLE_HPP
#define RESOURCE_HANDLE_HPP

#include <cstdlib>

/**
 * Base class for resource handles.
 */
class ResourceHandleBase
{
public:
	/// Creates a resource handle base.
	ResourceHandleBase();

	/// Destroys a resource handle base.
	virtual ~ResourceHandleBase() = default;

	/// Number of times the handle is referenced.
	std::size_t referenceCount;
};

/**
 * Templated resource handle class.
 */
template <typename T>
class ResourceHandle: public ResourceHandleBase
{
public:
	/// Creates a resource handle
	ResourceHandle();

	/// Destroys a resource handle and deletes its data.
	virtual ~ResourceHandle();

	/// Pointer to resource data
	T* data;
};

template <typename T>
ResourceHandle<T>::ResourceHandle():
	data(nullptr)
{}

template <typename T>
ResourceHandle<T>::~ResourceHandle()
{
	delete data;
}

#endif // RESOURCE_HANDLE_HPP

