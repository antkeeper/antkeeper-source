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
#include <engine/resources/serializer.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/i18n/string-map.hpp>

template <>
i18n::string_map* resource_loader<i18n::string_map>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	i18n::string_map* map = new i18n::string_map();
	
	deserialize_context ctx(file);
	deserializer<i18n::string_map>().deserialize(*map, ctx);
	
	return map;
}

template <>
void resource_loader<i18n::string_map>::save(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path, const i18n::string_map* map)
{
	serialize_context ctx(file);
	serializer<i18n::string_map>().serialize(*map, ctx);
}
