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
#include "game/world.hpp"
#include "application.hpp"
#include "debug/logger.hpp"
#include "resources/json.hpp"
#include "resources/resource-manager.hpp"
#include "render/model.hpp"
#include "render/material.hpp"
#include "render/passes/sky-pass.hpp"
#include "render/passes/ground-pass.hpp"
#include "game/system/astronomy.hpp"
#include "game/system/terrain.hpp"
#include "math/noise/noise.hpp"
#include "math/hash/hash.hpp"
#include <fstream>
#include <iostream>
#include <stb/stb_image_write.h>
#include "resources/image.hpp"

#include <algorithm>
#include <execution>


namespace game {
namespace load {

void colony(game::context& ctx, const std::filesystem::path& path)
{
	ctx.logger->push_task("Loading colony from \"" + path.string() + "\"");
	try
	{
		json* data = ctx.resource_manager->load<json>(path);
		
	}
	catch (...)
	{
		ctx.logger->pop_task(EXIT_FAILURE);
	}
	ctx.logger->pop_task(EXIT_SUCCESS);
}

} // namespace load
} // namespace game
