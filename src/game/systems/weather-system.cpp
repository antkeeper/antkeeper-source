/*
 * Copyright (C) 2020  Christopher J. Howard
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
#include "renderer/passes/sky-pass.hpp"
#include "renderer/passes/shadow-map-pass.hpp"
#include "renderer/passes/material-pass.hpp"
#include <cmath>

weather_system::weather_system(entt::registry& registry):
	entity_system(registry),
	ambient_light(nullptr),
	sun_light(nullptr),
	moon_light(nullptr),
	shadow_light(nullptr),
	sky_pass(nullptr),
	shadow_map_pass(nullptr),
	material_pass(nullptr),
	time_of_day(0.0f),
	time_scale(1.0f),
	sun_direction{0.0f, -1.0f, 0.0f}
{}

void weather_system::update(double t, double dt)
{
	set_time_of_day(time_of_day + dt * time_scale);
}

void weather_system::set_ambient_light(::ambient_light* light)
{
	this->ambient_light = light;
}

void weather_system::set_sun_light(directional_light* light)
{
	sun_light = light;
	
	if (sky_pass)
	{
		sky_pass->set_sun_light(sun_light);
	}
}

void weather_system::set_moon_light(directional_light* light)
{
	moon_light = light;
}

void weather_system::set_sky_pass(::sky_pass* pass)
{
	sky_pass = pass;
	
	if (sky_pass)
	{
		sky_pass->set_sun_light(sun_light);
	}
}

void weather_system::set_shadow_map_pass(::shadow_map_pass* pass)
{
	shadow_map_pass = pass;
	
	if (shadow_map_pass)
	{
		shadow_map_pass->set_light(shadow_light);
	}
}

void weather_system::set_material_pass(::material_pass* pass)
{
	material_pass = pass;
	
	if (material_pass)
	{
		material_pass->set_shadow_strength(0.75f);
	}
}

void weather_system::set_time_of_day(float t)
{
	static constexpr float seconds_per_day = 24.0f * 60.0f * 60.0f;
	time_of_day = std::fmod(t, seconds_per_day);
	
	sun_azimuth = 0.0f;
	sun_elevation = (time_of_day / seconds_per_day) * math::two_pi<float> - math::half_pi<float>;
	
	math::quaternion<float> sun_azimuth_rotation = math::angle_axis(sun_azimuth, float3{0, 1, 0});
	math::quaternion<float> sun_elevation_rotation = math::angle_axis(sun_elevation, float3{-1, 0, 0});
	math::quaternion<float> sun_rotation = math::normalize(sun_azimuth_rotation * sun_elevation_rotation);
	sun_direction = math::normalize(sun_rotation * float3{0, 0, -1});
	
	if (sun_light)
	{
		sun_light->set_rotation(sun_rotation);
	}
	
	shadow_light = sun_light;
	if (shadow_map_pass)
	{
		shadow_map_pass->set_light(shadow_light);
	}
}

void weather_system::set_time_scale(float scale)
{
	time_scale = scale;
}
