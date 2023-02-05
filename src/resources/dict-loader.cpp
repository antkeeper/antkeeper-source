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

#include "resources/resource-loader.hpp"
#include "resources/serializer.hpp"
#include "resources/deserializer.hpp"
#include "utility/dict.hpp"
#include <cstdint>
#include <physfs.h>

template <>
dict<std::uint32_t>* resource_loader<dict<std::uint32_t>>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	dict<std::uint32_t>* dict = new ::dict<std::uint32_t>();
	
	deserialize_context ctx(file);
	deserializer<::dict<std::uint32_t>>().deserialize(*dict, ctx);
	
	return dict;
}

template <>
void resource_loader<dict<std::uint32_t>>::save(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path, const dict<std::uint32_t>* dict)
{
	serialize_context ctx(file);
	serializer<::dict<std::uint32_t>>().serialize(*dict, ctx);
}
