/*
 * Copyright (C) 2023  Christopher J. Howard
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

#ifndef ANTKEEPER_RESOURCE_HANDLE_HPP
#define ANTKEEPER_RESOURCE_HANDLE_HPP

#include <cstddef>

/**
 * Base class for resource handles.
 */
class resource_handle_base
{
public:
	/// Creates a resource handle base.
	resource_handle_base();

	/// Destroys a resource handle base.
	virtual ~resource_handle_base() = default;

	/// Number of times the handle is referenced.
	std::size_t reference_count;
};

/**
 * Templated resource handle class.
 */
template <typename T>
class resource_handle: public resource_handle_base
{
public:
	/// Creates a resource handle
	resource_handle();

	/// Destroys a resource handle and deletes its data.
	virtual ~resource_handle();

	/// Pointer to resource data
	T* data;
};

template <typename T>
resource_handle<T>::resource_handle():
	data(nullptr)
{}

template <typename T>
resource_handle<T>::~resource_handle()
{
	delete data;
}

#endif // ANTKEEPER_RESOURCE_HANDLE_HPP

