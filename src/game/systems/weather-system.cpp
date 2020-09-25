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
#include "scene/ambient-light.hpp"
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
	
	if (sky_pass)
	{
		sky_pass->set_moon_light(moon_light);
	}
}

void weather_system::set_sky_pass(::sky_pass* pass)
{
	sky_pass = pass;
	
	if (sky_pass)
	{
		sky_pass->set_sun_light(sun_light);
		sky_pass->set_moon_light(moon_light);
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
	time_of_day = std::fmod(seconds_per_day + fmod(t, seconds_per_day), seconds_per_day);
	
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
	
	if (moon_light)
	{
		math::quaternion<float> moon_azimuth_rotation = math::angle_axis(sun_azimuth, float3{0, 1, 0});
		math::quaternion<float> moon_elevation_rotation = math::angle_axis(sun_elevation, float3{1, 0, 0});
		math::quaternion<float> moon_rotation = math::normalize(moon_azimuth_rotation * moon_elevation_rotation);
		moon_light->set_rotation(moon_rotation);
	}
	
	if (sky_pass)
	{
		float hour = time_of_day / (60.0f * 60.0f);
		std::size_t hour_index = static_cast<std::size_t>(hour);
		
		const std::array<float4, 4>& gradient0 = sky_gradients[hour_index];
		const std::array<float4, 4>& gradient1 = sky_gradients[(hour_index + 1) % sky_gradients.size()];
		float t = hour - std::floor(hour);
		
		std::array<float4, 4> gradient;
		for (int i = 0; i < 4; ++i)
		{
			gradient[i] = math::lerp(gradient0[i], gradient1[i], t);
		}
		
		float3 sun_color0 = sun_colors[hour_index];
		float3 sun_color1 = sun_colors[(hour_index + 1) % sun_colors.size()];
		float3 sun_color = math::lerp(sun_color0, sun_color1, t);
		
		float3 moon_color0 = moon_colors[hour_index];
		float3 moon_color1 = moon_colors[(hour_index + 1) % moon_colors.size()];
		float3 moon_color = math::lerp(moon_color0, moon_color1, t);
		
		float3 ambient_color0 = ambient_colors[hour_index];
		float3 ambient_color1 = ambient_colors[(hour_index + 1) % sun_colors.size()];
		float3 ambient_color = math::lerp(ambient_color0, ambient_color1, t);
		
		sun_light->set_color(sun_color);
		
		moon_light->set_color(moon_color);
		moon_light->set_intensity(1.0f);
		
		ambient_light->set_color(ambient_color);
		
		sky_pass->set_sky_gradient(gradient);
		sky_pass->set_time_of_day(time_of_day);
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
			std::array<float4, 4> gradient;
			
			for (unsigned int y = 0; y < std::min<unsigned int>(4, h); ++y)
			{
				unsigned int i = y * w * c + x * c;
				float r = srgb_to_linear(static_cast<float>(pixels[i]) / 255.0f);
				float g = srgb_to_linear(static_cast<float>(pixels[i + 1]) / 255.0f);
				float b = srgb_to_linear(static_cast<float>(pixels[i + 2]) / 255.0f);
				
				gradient[y] = {r, g, b,  static_cast<float>(y) * (1.0f / 3.0f)};
			}
			
			sky_gradients.push_back(gradient);
		}
	}
}

void weather_system::set_sun_palette(const ::image* image)
{
	sun_palette = image;
	if (sun_palette)
	{
		unsigned int w = image->get_width();
		unsigned int h = image->get_height();
		unsigned int c = image->get_channels();
		const unsigned char* pixels = static_cast<const unsigned char*>(image->get_pixels());
		
		for (unsigned int x = 0; x < w; ++x)
		{
			float3 color;
			
			unsigned int y = 0;
			
			unsigned int i = y * w * c + x * c;
			float r = srgb_to_linear(static_cast<float>(pixels[i]) / 255.0f);
			float g = srgb_to_linear(static_cast<float>(pixels[i + 1]) / 255.0f);
			float b = srgb_to_linear(static_cast<float>(pixels[i + 2]) / 255.0f);
				
			color = {r, g, b};
			
			sun_colors.push_back(color);
		}
	}
}

void weather_system::set_moon_palette(const ::image* image)
{
	moon_palette = image;
	if (moon_palette)
	{
		unsigned int w = image->get_width();
		unsigned int h = image->get_height();
		unsigned int c = image->get_channels();
		const unsigned char* pixels = static_cast<const unsigned char*>(image->get_pixels());
		
		for (unsigned int x = 0; x < w; ++x)
		{
			float3 color;
			
			unsigned int y = 0;
			
			unsigned int i = y * w * c + x * c;
			float r = srgb_to_linear(static_cast<float>(pixels[i]) / 255.0f);
			float g = srgb_to_linear(static_cast<float>(pixels[i + 1]) / 255.0f);
			float b = srgb_to_linear(static_cast<float>(pixels[i + 2]) / 255.0f);
				
			color = {r, g, b};
			
			moon_colors.push_back(color);
		}
	}
}

void weather_system::set_ambient_palette(const ::image* image)
{
	ambient_palette = image;
	if (ambient_palette)
	{
		unsigned int w = image->get_width();
		unsigned int h = image->get_height();
		unsigned int c = image->get_channels();
		const unsigned char* pixels = static_cast<const unsigned char*>(image->get_pixels());
		
		for (unsigned int x = 0; x < w; ++x)
		{
			float3 color;
			
			unsigned int y = 0;
			
			unsigned int i = y * w * c + x * c;
			float r = srgb_to_linear(static_cast<float>(pixels[i]) / 255.0f);
			float g = srgb_to_linear(static_cast<float>(pixels[i + 1]) / 255.0f);
			float b = srgb_to_linear(static_cast<float>(pixels[i + 2]) / 255.0f);
				
			color = {r, g, b};
			
			ambient_colors.push_back(color);
		}
	}
}

void weather_system::set_shadow_palette(const ::image* image)
{
	shadow_palette = image;
}
