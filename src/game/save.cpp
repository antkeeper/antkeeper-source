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

#include "game/save.hpp"
#include "application.hpp"
#include "debug/logger.hpp"
#include <fstream>

namespace game {

void save_config(game::context* ctx)
{
	const std::string config_file_path = ctx->config_path + "config.json";
	ctx->logger->push_task("Saving config to \"" + config_file_path + "\"");
	try
	{
		std::ofstream config_file(config_file_path);
		config_file << *(ctx->config);
	}
	catch (...)
	{
		ctx->logger->pop_task(EXIT_FAILURE);
	}
	ctx->logger->pop_task(EXIT_SUCCESS);
}

} // namespace game
