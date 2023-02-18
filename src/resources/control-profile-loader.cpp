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
#include "game/control-profile.hpp"

template <>
game::control_profile* resource_loader<game::control_profile>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	game::control_profile* profile = new game::control_profile();
	
	deserialize_context ctx(file);
	deserializer<game::control_profile>().deserialize(*profile, ctx);
	
	return profile;
}

template <>
void resource_loader<game::control_profile>::save(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path, const game::control_profile* profile)
{
	serialize_context ctx(file);
	serializer<game::control_profile>().serialize(*profile, ctx);
}
