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
#include "utility/gamma.hpp"
#include "resources/image.hpp"
#include <cmath>
#include <iostream>

static constexpr float seconds_per_day = 24.0f * 60.0f * 60.0f;

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
	sky_palette(nullptr),
	shadow_palette(nullptr),
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
	time_of_day = std::fmod(t, seconds_per_day);
	
	//sun_azimuth = 0.0f;
	//sun_elevation = (time_of_day / seconds_per_day) * math::two_pi<float> - math::half_pi<float>;
	
	float hour_angle = math::wrap_radians(time_of_day * (math::two_pi<float> / seconds_per_day) - math::pi<float>);
	float declination = math::radians(0.0f);
	float latitude = math::radians(0.0f);
	

	sun_elevation = std::asin(std::sin(declination) * std::sin(latitude) + std::cos(declination) * std::cos(hour_angle) * std::cos(latitude));
	sun_azimuth = std::acos((std::sin(declination) * std::cos(latitude) - std::cos(declination) * std::cos(hour_angle) * std::sin(latitude)) / std::cos(sun_elevation));
	if (hour_angle < 0.0f)
		sun_azimuth = math::two_pi<float> - sun_azimuth;
	
	//std::cout << "hour angle: " << math::degrees(hour_angle) << std::endl;
	//std::cout << "azimuth: " << math::degrees(sun_azimuth) << std::endl;
	//std::cout << "time: " << (time_of_day / 60.0f / 60.0f) << std::endl;
	
	math::quaternion<float> sun_azimuth_rotation = math::angle_axis(sun_azimuth, float3{0, 1, 0});
	math::quaternion<float> sun_elevation_rotation = math::angle_axis(sun_elevation, float3{-1, 0, 0});
	math::quaternion<float> sun_rotation = math::normalize(sun_azimuth_rotation * sun_elevation_rotation);
	sun_direction = math::normalize(sun_rotation * float3{0, 0, -1});
	
	if (sun_light)
	{
		sun_light->set_rotation(sun_rotation);
	}
	
	if (sky_pass)
	{
		sky_pass->set_sky_gradient(sky_gradient);
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

void weather_system::set_sky_palette(const ::image* image)
{
	sky_palette = image;
	if (sky_palette)
	{
		unsigned int w = image->get_width();
		unsigned int h = image->get_height();
		unsigned int c = image->get_channels();
		const unsigned char* pixels = static_cast<const unsigned char*>(image->get_pixels());
		
		for (unsigned int x = 0; x < w; ++x)
		{
			for (unsigned int y = 0; y < std::min<unsigned int>(4, h); ++y)
			{
				unsigned int i = y * w * c + x * c;
				float r = srgb_to_linear(static_cast<float>(pixels[i]) / 255.0f);
				float g = srgb_to_linear(static_cast<float>(pixels[i + 1]) / 255.0f);
				float b = srgb_to_linear(static_cast<float>(pixels[i + 2]) / 255.0f);
				
				sky_gradient[y] = {r, g, b,  static_cast<float>(y) * (1.0f / 3.0f)};
			}
		}
	}
}

void weather_system::set_shadow_palette(const ::image* image)
{
	shadow_palette = image;
}
