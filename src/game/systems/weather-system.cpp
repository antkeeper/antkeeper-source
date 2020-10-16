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
#include "game/astronomy/celestial-coordinates.hpp"
#include "game/astronomy/celestial-mechanics.hpp"
#include "game/astronomy/celestial-time.hpp"
#include <cmath>
#include <iostream>

static constexpr double hours_per_day = 24.0;
static constexpr double minutes_per_day = hours_per_day * 60.0;
static constexpr double seconds_per_day = minutes_per_day * 60.0;

weather_system::weather_system(entt::registry& registry):
	entity_system(registry),
	ambient_light(nullptr),
	sun_light(nullptr),
	moon_light(nullptr),
	shadow_light(nullptr),
	sky_pass(nullptr),
	shadow_map_pass(nullptr),
	material_pass(nullptr),
	time_scale(1.0f),
	sun_direction{0.0f, -1.0f, 0.0f},
	location{0.0f, 0.0f, 0.0f},
	jd(0.0)
{}

void weather_system::update(double t, double dt)
{
	jd += (dt * time_scale) / seconds_per_day;
	
	const float latitude = location[0];
	const float longitude = location[1];
	
	// Calculate local time
	double time_correction = longitude / (math::two_pi<double> / 24.0);
	double local_jd = jd + time_correction / 24.0 - 0.5;
	double local_time = (local_jd - std::floor(local_jd)) * 24.0;
	
	double lmst = ast::jd_to_lmst(jd, longitude);
	double ecl = ast::approx_ecliptic_obliquity(jd);
	double3x3 ecliptic_to_horizontal = ast::ecliptic_to_horizontal(ecl, latitude, lmst);

	
	double3 sun_ecliptic = ast::approx_sun_ecliptic(jd);
	double3 sun_horizontal = ecliptic_to_horizontal * sun_ecliptic;
	sun_horizontal.z -= 4.25875e-5; // Subtract one earth radius (in AU), for positon of observer
	double3 sun_spherical = ast::rectangular_to_spherical(sun_horizontal);
	double3 sun_positiond = ast::horizontal_to_right_handed * sun_horizontal;
	float2 sun_az_el = {static_cast<float>(sun_spherical.z) - math::pi<float>, static_cast<float>(sun_spherical.y)};
	float3 sun_position = math::normalize(float3{static_cast<float>(sun_positiond.x), static_cast<float>(sun_positiond.y), static_cast<float>(sun_positiond.z)});
	
	double3 moon_ecliptic = ast::approx_moon_ecliptic(jd);
	double3 moon_horizontal = ecliptic_to_horizontal * moon_ecliptic;
	moon_horizontal.z -= 1.0; // Subtract one earth radius, for position of observer
	
	double3 moon_spherical = ast::rectangular_to_spherical(moon_horizontal);
	double3 moon_positiond = ast::horizontal_to_right_handed * moon_horizontal;
	float2 moon_az_el = {static_cast<float>(moon_spherical.z) - math::pi<float>, static_cast<float>(moon_spherical.y)};	
	float3 moon_position = math::normalize(math::type_cast<float>(moon_positiond));
	
	double3x3 moon_rotation_matrix = ast::horizontal_to_right_handed * ecliptic_to_horizontal;
	math::quaternion<double> moon_rotationd = math::normalize(math::quaternion_cast(moon_rotation_matrix) * math::angle_axis(math::half_pi<double>, double3{0, 1, 0}) * math::angle_axis(-math::half_pi<double>, double3{0, 0, -1}));
	math::quaternion<float> moon_rotation =
	{
		static_cast<float>(moon_rotationd.w),
		static_cast<float>(moon_rotationd.x),
		static_cast<float>(moon_rotationd.y),
		static_cast<float>(moon_rotationd.z)
	};
	
	if (sun_light)
	{
		math::quaternion<float> sun_azimuth_rotation = math::angle_axis(sun_az_el[0], float3{0, 1, 0});
		math::quaternion<float> sun_elevation_rotation = math::angle_axis(sun_az_el[1], float3{-1, 0, 0});
		math::quaternion<float> sun_az_el_rotation = math::normalize(sun_azimuth_rotation * sun_elevation_rotation);
		sun_light->set_rotation(sun_az_el_rotation);
	}
	
	if (moon_light)
	{
		math::quaternion<float> moon_azimuth_rotation = math::angle_axis(moon_az_el[0], float3{0, 1, 0});
		math::quaternion<float> moon_elevation_rotation = math::angle_axis(moon_az_el[1], float3{-1, 0, 0});
		math::quaternion<float> moon_az_el_rotation = math::normalize(moon_azimuth_rotation * moon_elevation_rotation);
		moon_light->set_rotation(moon_az_el_rotation);
	}
	
	float sun_gradient_position = static_cast<float>(std::max<double>(0.0, ((sun_az_el[1] + math::half_pi<double>) / math::pi<double>)));
	float moon_gradient_position = static_cast<float>(std::max<double>(0.0, ((moon_az_el[1] + math::half_pi<double>) / math::pi<double>)));
	float sky_gradient_position = sun_gradient_position;
	float ambient_gradient_position = sun_gradient_position;
	
	if (sky_pass)
	{
		if (sun_light)
		{
			float3 sun_color = interpolate_gradient(sun_colors, sun_gradient_position);
			sun_light->set_color(sun_color);
			sun_light->set_intensity(1.0f);
		}
		
		if (moon_light)
		{
			float3 moon_color = interpolate_gradient(moon_colors, moon_gradient_position);
			moon_light->set_color(moon_color);
			moon_light->set_intensity(1.0f);
		}
		
		if (ambient_light)
		{
			float3 ambient_color = interpolate_gradient(ambient_colors, ambient_gradient_position);
			ambient_light->set_color(ambient_color);
			ambient_light->set_intensity(0.5f);
		}
		
		float3 horizon_color = interpolate_gradient(horizon_colors, sun_gradient_position);
		float3 zenith_color = interpolate_gradient(zenith_colors, sun_gradient_position);
		sky_pass->set_horizon_color(horizon_color);
		sky_pass->set_zenith_color(zenith_color);
		sky_pass->set_time_of_day(static_cast<float>(local_time * 60.0 * 60.0));
		sky_pass->set_observer_location(location[0], location[1], location[2]);
		sky_pass->set_sun_coordinates(sun_position, sun_az_el);
		sky_pass->set_moon_coordinates(moon_position, moon_az_el);
		sky_pass->set_julian_day(static_cast<float>(jd));
		sky_pass->set_moon_rotation(moon_rotation);
	}
	
	shadow_light = sun_light;
	if (shadow_map_pass)
	{
		if (sun_az_el[1] < 0.0f)
		{
			shadow_map_pass->set_light(moon_light);
		}
		else
		{
			shadow_map_pass->set_light(sun_light);
		}
	}
	
	if (material_pass)
	{
		float shadow_strength = interpolate_gradient(shadow_strengths, sun_gradient_position).x;
		material_pass->set_shadow_strength(shadow_strength);
	}
}

void weather_system::set_location(float latitude, float longitude, float altitude)
{
	location = {latitude, longitude, altitude};
}

void weather_system::set_ambient_light(::ambient_light* light)
{
	ambient_light = light;
}

void weather_system::set_sun_light(directional_light* light)
{
	sun_light = light;
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
		sky_pass->set_moon_angular_radius(math::radians(1.0f));
		sky_pass->set_sun_angular_radius(math::radians(1.1f));
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
}

void weather_system::set_time(int year, int month, int day, int hour, int minute, double second, double tc)
{
	jd = ast::ut_to_jd(year, month, day, hour, minute, second) - tc / 24.0;
}

void weather_system::set_time_scale(float scale)
{
	time_scale = scale;
}

void weather_system::set_sky_palette(const ::image* image)
{
	load_palette(&horizon_colors, image, 0);
	load_palette(&zenith_colors, image, 1);
}

void weather_system::set_sun_palette(const ::image* image)
{
	load_palette(&sun_colors, image, 0);
}

void weather_system::set_moon_palette(const ::image* image)
{
	load_palette(&moon_colors, image, 0);
}

void weather_system::set_ambient_palette(const ::image* image)
{
	load_palette(&ambient_colors, image, 0);
}

void weather_system::set_shadow_palette(const ::image* image)
{
	load_palette(&shadow_strengths, image, 0);
}

void weather_system::load_palette(std::vector<float3>* palette, const ::image* image, unsigned int row)
{
	unsigned int w = image->get_width();
	unsigned int h = image->get_height();
	unsigned int c = image->get_channels();
	unsigned int y = std::min<unsigned int>(row, h - 1);
	
	palette->clear();
	
	if (image->is_hdr())
	{
		const float* pixels = static_cast<const float*>(image->get_pixels());
		
		for (unsigned int x = 0; x < w; ++x)
		{
			unsigned int i = y * w * c + x * c;
			
			float r = pixels[i];
			float g = pixels[i + 1];
			float b = pixels[i + 2];
			
			palette->push_back(float3{r, g, b});
		}
	}
	else
	{
		const unsigned char* pixels = static_cast<const unsigned char*>(image->get_pixels());
		
		for (unsigned int x = 0; x < w; ++x)
		{
			unsigned int i = y * w * c + x * c;
			
			float r = srgb_to_linear(static_cast<float>(pixels[i]) / 255.0f);
			float g = srgb_to_linear(static_cast<float>(pixels[i + 1]) / 255.0f);
			float b = srgb_to_linear(static_cast<float>(pixels[i + 2]) / 255.0f);
			
			palette->push_back(float3{r, g, b});
		}
	}
}

float3 weather_system::interpolate_gradient(const std::vector<float3>& gradient, float position)
{
	if (gradient.empty())
		return float3{0.0f, 0.0f, 0.0f};
	
	position *= static_cast<float>(gradient.size() - 1);
	int index0 = static_cast<int>(position) % gradient.size();
	int index1 = (index0 + 1) % gradient.size();
	return math::lerp<float3>(gradient[index0], gradient[index1], position - std::floor(position));
}
