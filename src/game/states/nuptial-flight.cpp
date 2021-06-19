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

#include "game/states/nuptial-flight.hpp"
#include "entity/systems/astronomy.hpp"
#include "entity/systems/orbit.hpp"
#include "renderer/material-property.hpp"
#include "animation/screen-transition.hpp"
#include "animation/ease.hpp"
#include "resources/config-file.hpp"

namespace game {
namespace state {
namespace nuptial_flight {

void enter(game::context* ctx)
{
	// Pause motion of celestial objects
	ctx->astronomy_system->set_time_scale(0.0);
	ctx->orbit_system->set_time_scale(0.0);
	
	// Start fade in from white
	ctx->fade_transition_color->set_value({1, 1, 1});
	ctx->fade_transition->transition(1.0f, true, ease<float>::in_quad);
}

void exit(game::context* ctx)
{
	// Resume motion of celestial objects
	const double time_scale = ctx->config->get<double>("time_scale");
	ctx->astronomy_system->set_time_scale(time_scale);
	ctx->orbit_system->set_time_scale(time_scale);
}

} // namespace nuptial_flight
} // namespace state
} // namespace game
