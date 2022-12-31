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

#include "game/save.hpp"
#include "application.hpp"
#include "debug/logger.hpp"
#include "resources/json.hpp"
#include <fstream>

namespace game {
namespace save {

void colony(game::context& ctx)
{
	std::filesystem::path path = ctx.saves_path / "colony.sav";
	ctx.logger->push_task("Saving colony to \"" + path.string() + "\"");
	try
	{
		// Construct JSON data describing the colony
		json data;
		
		auto& colony = data["colony"];
		{
			auto& species = colony["species"];
			{
				auto& morphology = species["morphology"];
				{
					
				}
				
				auto& diet = species["diet"];
				auto& aggression = species["aggression"];
				auto& nest = species["nest"];
			}
			
			auto& habitat = colony["habitat"];
			{
				auto& biome = habitat["biome"];
				auto& nest = habitat["nest"];
				{
					nest["entrance"] = {0, 0, 0};
				}
			}
			
			auto& members = colony["members"];
			members = json::array();
			{
				
			}
		}
		
		std::ofstream file(path);
		file << data;
	}
	catch (...)
	{
		ctx.logger->pop_task(EXIT_FAILURE);
	}
	ctx.logger->pop_task(EXIT_SUCCESS);
}

void config(game::context& ctx)
{
	std::filesystem::path path = ctx.config_path / "config.json";
	ctx.logger->push_task("Saving config to \"" + path.string() + "\"");
	try
	{
		std::ofstream file(path);
		file << *(ctx.config);
	}
	catch (...)
	{
		ctx.logger->pop_task(EXIT_FAILURE);
	}
	ctx.logger->pop_task(EXIT_SUCCESS);
}

} // namespace save
} // namespace game
