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

#include "game/systems/weather-system.hpp"
#include "scene/directional-light.hpp"
#include "scene/ambient-light.hpp"
#include "renderer/passes/sky-pass.hpp"
#include "renderer/passes/shadow-map-pass.hpp"
#include "renderer/passes/material-pass.hpp"
#include "utility/gamma.hpp"
#include "resources/image.hpp"
#include "game/astronomy/celestial-coordinates.hpp"
#include "game/astronomy/celestial-mechanics.hpp"
#include <cmath>

static constexpr double seconds_per_day = 24.0 * 60.0 * 60.0;

weather_system::weather_system(entt::registry& registry):
	entity_system(registry),
	sky_pass(nullptr),
	shadow_map_pass(nullptr),
	material_pass(nullptr),
	universal_time(0.0),
	days_per_timestep(1.0 / seconds_per_day)
{}

void weather_system::update(double t, double dt)
{
	// Add scaled timestep to current time
	set_universal_time(universal_time + dt * days_per_timestep);
}

void weather_system::set_sky_pass(::sky_pass* pass)
{
	sky_pass = pass;
}

void weather_system::set_shadow_map_pass(::shadow_map_pass* pass)
{
	shadow_map_pass = pass;
}

void weather_system::set_material_pass(::material_pass* pass)
{
	material_pass = pass;
	material_pass->set_shadow_strength(0.5f);
}

void weather_system::set_universal_time(double time)
{
	universal_time = time;
}

void weather_system::set_time_scale(double scale)
{
	days_per_timestep = scale / seconds_per_day;
}
