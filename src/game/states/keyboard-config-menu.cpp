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

#include "game/states/keyboard-config-menu.hpp"
#include "game/states/controls-menu.hpp"
#include "application.hpp"
#include "scene/text.hpp"
#include "render/passes/clear-pass.hpp"
#include "debug/logger.hpp"
#include "resources/resource-manager.hpp"

namespace game {
namespace state {
namespace keyboard_config_menu {

void enter(game::context* ctx)
{
	// Load control profile
	if (ctx->config->contains("control_profile"))
	{
		json* profile = ctx->resource_manager->load<json>((*ctx->config)["control_profile"].get<std::string>());
		
		for (auto& control_element: (*profile)["controls"].items())
		{
			ctx->logger->log(control_element.key());
			
			
		}
	}
}

void exit(game::context* ctx)
{

}

} // namespace keyboard_config_menu
} // namespace state
} // namespace game
