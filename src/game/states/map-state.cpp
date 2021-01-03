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

#include "animation/ease.hpp"
#include "animation/screen-transition.hpp"
#include "animation/timeline.hpp"
#include "application.hpp"
#include "debug/logger.hpp"
#include "game/game-context.hpp"
#include "input/input-listener.hpp"
#include "event/input-events.hpp"
#include "rasterizer/rasterizer.hpp"
#include "game/states/game-states.hpp"
#include "renderer/passes/sky-pass.hpp"
#include "scene/billboard.hpp"
#include "scene/scene.hpp"
#include <functional>

void map_state_enter(game_context* ctx)
{
	debug::logger* logger = ctx->logger;	
	logger->push_task("Entering map state");
	
	// Disable sky pass
	ctx->overworld_sky_pass->set_enabled(false);
	
	// Start fade in
	ctx->fade_transition->transition(1.0f, true, ease<float>::in_quad);
	
	logger->pop_task(EXIT_SUCCESS);
}

void map_state_exit(game_context* ctx)
{
	debug::logger* logger = ctx->logger;
	logger->push_task("Exiting map state");
	
	logger->pop_task(EXIT_SUCCESS);
}
