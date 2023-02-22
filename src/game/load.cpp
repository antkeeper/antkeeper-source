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

#include "game/load.hpp"
#include <engine/debug/log.hpp>
#include <engine/resources/json.hpp>
#include <engine/resources/resource-manager.hpp>

namespace load {

void colony(::game& ctx, const std::filesystem::path& path)
{
	const std::string path_string = path.string();
	
	debug::log::trace("Loading colony from \"{}\"...", path_string);
	try
	{
		json* data = ctx.resource_manager->load<json>(path);
		
		debug::log::trace("Loaded colony from \"{}\"", path_string);
	}
	catch (...)
	{
		debug::log::error("Failed to load colony from \"{}\"", path_string);
	}
}

} // namespace load
