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

static constexpr double hours_per_day = 24.0;
static constexpr double minutes_per_day = hours_per_day * 60.0;
static constexpr double seconds_per_day = minutes_per_day * 60.0;

/**
 *
 * @param year Gregorian year
 * @param month Month (1 = January, 12 = December)
 * @param day Day (1-31)
 * @param time Universal time in decimal hours.
 */
static double julian_day(int year, int month, int day, double time)
{
	if (month < 3)
	{
		month += 12;
		year -= 1;
	}
	
	double y = static_cast<double>(year);
	double m = static_cast<double>(month);
	double d = static_cast<double>(day);
	
	return std::floor(365.25 * y) + std::floor(30.6001 * (m + 1.0)) - 15.0 + 1720996.5 + d + time;
}

/// @see A Physically-Based Night Sky Model
/// @see http://www.powerfromthesun.net/Book/chapter03/chapter03.html
void find_sun_ecliptic(double jd, double* longitude, double* latitude, double* distance)
{
	const double t = (jd - 2451545.0) / 36525.0;
	const double m = 6.24 + 628.302 * t;
	
	*longitude = 4.895048 + 628.331951 * t + (0.033417 - 0.000084 * t) * std::sin(m) + 0.000351 * std::sin(m * 2.0);
	*latitude = 0.0;
	*distance = 1.000140 - (0.016708 - 0.000042 * t) * std::cos(m) - 0.000141 * std::cos(m * 2.0);
}

/**
 * Calculates the ecliptic geocentric coordinates of the moon, given a Julian day.
 *
 * @param[in] jd Julian day.
 * @param[out] longitude Ecliptic longitude of the moon, in radians.
 * @param[out] latitude Ecliptic latitude of the moon, in radians.
 * @param[out] distance Distance to the moon, in Earth radii.
 
 * @return Array containing the ecliptic longitude and latitude of the moon, in radians.
 *
 * @see A Physically-Based Night Sky Model
 */
void find_moon_ecliptic(double jd, double* longitude, double* latitude, double* distance)
{
	const double t = (jd - 2451545.0) / 36525.0;
	const double l1 = 3.8104 + 8399.7091 * t;
	const double m1 = 2.3554 + 8328.6911 * t;
	const double m = 6.2300 + 628.3019 * t;
	const double d = 5.1985 + 7771.3772 * t;
	const double d2 = d * 2.0;
	const double f = 1.6280 + 8433.4663 * t;
	
	*longitude = l1
		+ 0.1098 * std::sin(m1)
		+ 0.0222 * std::sin(d2 - m1)
		+ 0.0115 * std::sin(d2)
		+ 0.0037 * std::sin(m1 * 2.0)
		- 0.0032 * std::sin(m)
		- 0.0020 * std::sin(d2)
		+ 0.0010 * std::sin(d2 - m1 * 2.0)
		+ 0.0010 * std::sin(d2 - m - m1)
		+ 0.0009 * std::sin(d2 + m1)
		+ 0.0008 * std::sin(d2 - m)
		+ 0.0007 * std::sin(m1 - m)
		- 0.0006 * std::sin(d)
		- 0.0005 * std::sin(m + m1);

	*latitude = 0.0895 * sin(f)
		+ 0.0049 * std::sin(m1 + f)
		+ 0.0048 * std::sin(m1 - f)
		+ 0.0030 * std::sin(d2 - f)
		+ 0.0010 * std::sin(d2 + f - m1)
		+ 0.0008 * std::sin(d2 - f - m1)
		+ 0.0006 * std::sin(d2 + f);
	
	*distance = 1.0 / (0.016593
		+ 0.000904 * std::cos(m1)
		+ 0.000166 * std::cos(d2 - m1)
		+ 0.000137 * std::cos(d2)
		+ 0.000049 * std::cos(m1 * 2.0)
		+ 0.000015 * std::cos(d2 + m1)
		+ 0.000009 * std::cos(d2 - m));
}

/// @see http://www.stjarnhimlen.se/comp/ppcomp.html
/// @see http://www.geoastro.de/elevazmoon/basics/index.htm
void ecliptic_to_equatorial(double longitude, double latitude, double ecl, double* right_ascension, double* declination)
{
	double eclip_x = std::cos(longitude) * std::cos(latitude);
	double eclip_y = std::sin(longitude) * std::cos(latitude);
	double eclip_z = std::sin(latitude);
	
	double equat_x = eclip_x;
	double equat_y = eclip_y * std::cos(ecl) - eclip_z * std::sin(ecl);
	double equat_z = eclip_y * std::sin(ecl) + eclip_z * std::cos(ecl);
	
    *right_ascension = std::atan2(equat_y, equat_x);
    *declination = std::atan2(equat_z, sqrt(equat_x * equat_x + equat_y * equat_y));
}

/// @see http://www.stjarnhimlen.se/comp/ppcomp.html
/// @see http://www.geoastro.de/elevazmoon/basics/index.htm
void equatorial_to_horizontal(double right_ascension, double declination, double lmst, double latitude, double* azimuth, double* elevation)
{
	double hour_angle = lmst - right_ascension;
	
	double x = std::cos(hour_angle) * std::cos(declination);
	double y = std::sin(hour_angle) * std::cos(declination);
	double z = std::sin(declination);
	
	double horiz_x = x * std::cos(math::half_pi<double> - latitude) - z * std::sin(math::half_pi<double> - latitude);
	double horiz_y = y;
	double horiz_z = x * std::sin(math::half_pi<double> - latitude) + z * std::cos(math::half_pi<double> - latitude);
	
	*azimuth = std::atan2(horiz_y, horiz_x) + math::pi<double>;
	*elevation = std::atan2(horiz_z, std::sqrt(horiz_x * horiz_x + horiz_y * horiz_y));
}

/**
 * Calculates the Greenwich mean sidereal time (GMST) from a Julian day.
 *
 * @param jd Julian day.
 * @return GMST, in radians.
 */
static double jd_to_gmst(double jd)
{	 
	return math::wrap_radians<double>(4.894961212 + 6.300388098 * (jd - 2451545.0));
}

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
	sky_palette(nullptr),
	shadow_palette(nullptr),
	sun_direction{0.0f, -1.0f, 0.0f},
	coordinates{0.0f, 0.0f},
	jd(0.0)
{}

void weather_system::update(double t, double dt)
{
	jd += (dt * time_scale) / seconds_per_day;
	
	const float latitude = coordinates[0];
	const float longitude = coordinates[1];
	
	// Time correction
	double tc = longitude / (math::two_pi<double> / 24.0);
	
	//double pst_tc = -7.0;
	
	double local_jd = jd + tc / 24.0 - 0.5;
	double local_time = (local_jd - std::floor(local_jd)) * 24.0;
	double hour = local_time;
	
	

	
	// Calculate equation of time
	//float eot_b = (360.0f / 365.0f) * (day_of_year - 81.0f);
	//float eot = 9.87f * std::sin(eot_b * 2.0f) - 7.53f * std::cos(eot_b) - 1.5f * std::sin(eot_b);
	
	// Calculate local mean sidereal time (LST)
	//double tc = longitude / (math::two_pi<double> / 24.0); // Time correction
	//double ut = local_time + tc;                           // Universal time
	double gmst = jd_to_gmst(jd);
	double lmst = gmst + longitude;
	
	// Calculate sun position
	//float local_solar_time = local_time;// + eot / 60.0f;
	/*
	float sun_declination = math::radians(23.45f) * std::sin((math::two_pi<float> / 365.0f) * (284.0f + day_of_year));
	float sun_hour_angle = math::radians(15.0f) * (local_solar_time - 12.0f);
	sun_elevation = std::asin(std::sin(sun_declination) * std::sin(latitude) + std::cos(sun_declination) * std::cos(sun_hour_angle) * std::cos(latitude));
	sun_azimuth = std::acos((std::sin(sun_declination) * std::cos(latitude) - std::cos(sun_declination) * std::cos(sun_hour_angle) * std::sin(latitude)) / std::cos(sun_elevation));
	if (sun_hour_angle > 0.0f)
		sun_azimuth = math::two_pi<float> - sun_azimuth;
	*/
	
	// J2000 day
	double d = jd - 2451545.0;
	
	// Obliquity of the ecliptic
	double ecl = math::radians<double>(23.4393 - 3.563e-7 * d);
	
	// Calculation sun coordinates
	double sun_longitude;
	double sun_latitude;
	double sun_distance;
	double sun_right_ascension;
	double sun_declination;
	double sun_azimuth;
	double sun_elevation;
	find_sun_ecliptic(jd, &sun_longitude, &sun_latitude, &sun_distance);
	ecliptic_to_equatorial(sun_longitude, sun_latitude, ecl, &sun_right_ascension, &sun_declination);
	equatorial_to_horizontal(sun_right_ascension, sun_declination, lmst, latitude, &sun_azimuth, &sun_elevation);
	
	// Calculate moon coordinates
	double moon_longitude;
	double moon_latitude;
	double moon_distance;
	double moon_right_ascension;
	double moon_declination;
	double moon_azimuth;
	double moon_elevation;
	find_moon_ecliptic(jd, &moon_longitude, &moon_latitude, &moon_distance);
	ecliptic_to_equatorial(moon_longitude, moon_latitude, ecl, &moon_right_ascension, &moon_declination);
	equatorial_to_horizontal(moon_right_ascension, moon_declination, lmst, latitude, &moon_azimuth, &moon_elevation);
	
	/*
	std::cout.precision(10);
	std::cout << std::fixed;
	//std::cout << "gmst: " << math::degrees<double>(gmst) << std::endl;
	std::cout << "JD: " << jd << std::endl;
	std::cout << "PST: " << pst_time << std::endl;
	std::cout << "AZ: " << math::degrees(sun_azimuth) << std::endl;
	std::cout << "EL: " << math::degrees(sun_elevation) << std::endl;
	std::cout << "DEC: " << math::degrees(sun_declination) << std::endl;
	//std::cout << "eOT: " << eot << std::endl;
	*/

	float2 sun_az_el = float2{static_cast<float>(sun_azimuth), static_cast<float>(sun_elevation)};
	math::quaternion<float> sun_azimuth_rotation = math::angle_axis(sun_az_el[0], float3{0, 1, 0});
	math::quaternion<float> sun_elevation_rotation = math::angle_axis(sun_az_el[1], float3{-1, 0, 0});
	math::quaternion<float> sun_rotation = math::normalize(sun_azimuth_rotation * sun_elevation_rotation);
	float3 sun_position = math::normalize(sun_rotation * float3{0, 0, -1});
	
	float2 moon_az_el = float2{static_cast<float>(moon_azimuth), static_cast<float>(moon_elevation)};
	math::quaternion<float> moon_azimuth_rotation = math::angle_axis(moon_az_el[0], float3{0, 1, 0});
	math::quaternion<float> moon_elevation_rotation = math::angle_axis(moon_az_el[1], float3{-1, 0, 0});
	math::quaternion<float> moon_rotation = math::normalize(moon_azimuth_rotation * moon_elevation_rotation);
	float3 moon_position = math::normalize(moon_rotation * float3{0, 0, -1});
	
	if (sun_light)
	{
		sun_light->set_rotation(sun_rotation);
	}
	
	if (moon_light)
	{
		moon_light->set_rotation(moon_rotation);
	}
	
	std::size_t hour_index = static_cast<std::size_t>(hour);
	float lerp_factor = hour - std::floor(hour);
	
	if (sky_pass)
	{
		const std::array<float4, 4>& gradient0 = sky_gradients[hour_index];
		const std::array<float4, 4>& gradient1 = sky_gradients[(hour_index + 1) % sky_gradients.size()];
		
		std::array<float4, 4> gradient;
		for (int i = 0; i < 4; ++i)
		{
			gradient[i] = math::lerp(gradient0[i], gradient1[i], lerp_factor);
		}
		
		float3 sun_color0 = sun_colors[hour_index];
		float3 sun_color1 = sun_colors[(hour_index + 1) % sun_colors.size()];
		float3 sun_color = math::lerp(sun_color0, sun_color1, lerp_factor);
		
		float3 moon_color0 = moon_colors[hour_index];
		float3 moon_color1 = moon_colors[(hour_index + 1) % moon_colors.size()];
		float3 moon_color = math::lerp(moon_color0, moon_color1, lerp_factor);
		
		float3 ambient_color0 = ambient_colors[hour_index];
		float3 ambient_color1 = ambient_colors[(hour_index + 1) % sun_colors.size()];
		float3 ambient_color = math::lerp(ambient_color0, ambient_color1, lerp_factor);
		
		sun_light->set_color(sun_color);
		
		moon_light->set_color(moon_color);
		moon_light->set_intensity(1.0f);
		
		ambient_light->set_color(ambient_color);
		
		sky_pass->set_sky_gradient(gradient);
		sky_pass->set_time_of_day(static_cast<float>(hour * 60.0 * 60.0));
		sky_pass->set_observer_coordinates(coordinates);
		sky_pass->set_sun_coordinates(sun_position, sun_az_el);
		sky_pass->set_moon_coordinates(moon_position, moon_az_el);
		sky_pass->set_julian_day(static_cast<float>(jd));
	}
	
	shadow_light = sun_light;
	if (shadow_map_pass)
	{
		if (sun_elevation < 0.0f)
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
		float shadow_strength0 = shadow_strengths[hour_index];
		float shadow_strength1 = shadow_strengths[(hour_index + 1) % shadow_strengths.size()];
		float shadow_strength = math::lerp(shadow_strength0, shadow_strength1, lerp_factor);
		
		material_pass->set_shadow_strength(shadow_strength);
	}
}

void weather_system::set_coordinates(const float2& coordinates)
{
	this->coordinates = coordinates;
}

void weather_system::set_ambient_light(::ambient_light* light)
{
	this->ambient_light = light;
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

void weather_system::set_time(int year, int month, int day, int hour, int minute, int second, double tc)
{
	double time = ((static_cast<double>(hour) - tc) + ((static_cast<double>(minute) + static_cast<double>(second) / 60.0) / 60.0)) / 24.0;
	jd = julian_day(year, month, day, time);
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
	if (shadow_palette)
	{
		unsigned int w = image->get_width();
		unsigned int h = image->get_height();
		unsigned int c = image->get_channels();
		const unsigned char* pixels = static_cast<const unsigned char*>(image->get_pixels());
		
		for (unsigned int x = 0; x < w; ++x)
		{		
			unsigned int y = 0;
			
			unsigned int i = y * w * c + x * c;
			float r = 1.0f - (static_cast<float>(pixels[i]) / 255.0f);
			
			shadow_strengths.push_back(r);
		}
	}
}
