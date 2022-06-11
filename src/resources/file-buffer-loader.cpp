/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "resources/resource-loader.hpp"
#include "resources/file-buffer.hpp"
#include <physfs.h>

template <>
file_buffer* resource_loader<file_buffer>::load(resource_manager* resource_manager, PHYSFS_File* file)
{
	file_buffer* buffer = new file_buffer();
	
	// Read file into buffer
	std::size_t size = static_cast<std::size_t>(PHYSFS_fileLength(file));
	buffer->resize(size);
	PHYSFS_readBytes(file, buffer->data(), size);

	return buffer;
}