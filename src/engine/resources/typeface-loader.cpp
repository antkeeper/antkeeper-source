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

#include <engine/resources/resource-loader.hpp>
#include <engine/type/freetype/typeface.hpp>
#include <stdexcept>
#include <physfs.h>
#include <ft2build.h>
#include FT_FREETYPE_H

template <>
type::typeface* resource_loader<type::typeface>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	FT_Error error = 0;
	
	// Init FreeType library object
	FT_Library library;
	error = FT_Init_FreeType(&library);
	if (error)
	{
		throw std::runtime_error("Failed to init FreeType library (error code " + std::to_string(error) + ")");
	}
	
	// Read file into buffer
	std::size_t size = static_cast<std::size_t>(PHYSFS_fileLength(file));
	unsigned char* buffer = new unsigned char[size];
	PHYSFS_readBytes(file, buffer, size);
	
	// Load FreeType face from buffer
	FT_Face face;
	error = FT_New_Memory_Face(library, buffer, size, 0, &face);
	if (error)
	{
		delete[] buffer;
		FT_Done_FreeType(library);
		throw std::runtime_error("Failed to load FreeType face (error code " + std::to_string(error) + ")");
	}

	return new type::freetype::typeface(library, face, buffer);
}
