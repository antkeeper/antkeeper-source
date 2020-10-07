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

/**
 * Calculates the ecliptic rectangular geocentric coordinates of the sun, with distance in AU.
 */
double3 calculate_sun_ecliptic(double jd)
{
	const double t = (jd - 2451545.0) / 36525.0;
	const double m = 6.24 + 628.302 * t;
	
	const double longitude = 4.895048 + 628.331951 * t + (0.033417 - 0.000084 * t) * std::sin(m) + 0.000351 * std::sin(m * 2.0);
	const double latitude = 0.0;
	const double distance = 1.000140 - (0.016708 - 0.000042 * t) * std::cos(m) - 0.000141 * std::cos(m * 2.0);
	
	double3 ecliptic;
	ecliptic.x = distance * std::cos(longitude) * std::cos(latitude);
	ecliptic.y = distance * std::sin(longitude) * std::cos(latitude);
	ecliptic.z = distance * std::sin(latitude);
	
	return ecliptic;
}

/**
 * Calculates the ecliptic rectangular geocentric coordinates of the moon, with distance in Earth radii.
 */
double3 calculate_moon_ecliptic(double jd)
{
	const double t = (jd - 2451545.0) / 36525.0;
	const double l1 = 3.8104 + 8399.7091 * t;
	const double m1 = 2.3554 + 8328.6911 * t;
	const double m = 6.2300 + 628.3019 * t;
	const double d = 5.1985 + 7771.3772 * t;
	const double d2 = d * 2.0;
	const double f = 1.6280 + 8433.4663 * t;
	
	const double longitude = l1
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

	const double latitude = 0.0895 * sin(f)
		+ 0.0049 * std::sin(m1 + f)
		+ 0.0048 * std::sin(m1 - f)
		+ 0.0030 * std::sin(d2 - f)
		+ 0.0010 * std::sin(d2 + f - m1)
		+ 0.0008 * std::sin(d2 - f - m1)
		+ 0.0006 * std::sin(d2 + f);
	
	const double r = 1.0 / (0.016593
		+ 0.000904 * std::cos(m1)
		+ 0.000166 * std::cos(d2 - m1)
		+ 0.000137 * std::cos(d2)
		+ 0.000049 * std::cos(m1 * 2.0)
		+ 0.000015 * std::cos(d2 + m1)
		+ 0.000009 * std::cos(d2 - m));
	
	double3 ecliptic;
	ecliptic.x = r * std::cos(longitude) * std::cos(latitude);
	ecliptic.y = r * std::sin(longitude) * std::cos(latitude);
	ecliptic.z = r * std::sin(latitude);
	
	return ecliptic;
}

double3x3 find_moon_ecliptic_rotation(double jd)
{
	const double t = (jd - 2451545.0) / 36525.0;
	const double l1 = 3.8104 + 8399.7091 * t;
	const double f = 1.6280 + 8433.4663 * t;
	
	const double az0 = f + math::pi<double>;
	const double ax  = 0.026920;
	const double az1 = l1 - f;
	
	double3x3 rz0 =
	{
		cos(az0), -sin(az0), 0,
		sin(az0), cos(az0), 0,
		0, 0, 1
	};
	
	double3x3 rx =
	{
		1, 0, 0,
		0, cos(ax), -sin(ax),
		0, sin(ax), cos(ax)
	};
	
	double3x3 rz1 =
	{
		cos(az1), -sin(az1), 0,
		sin(az1), cos(az1), 0,
		0, 0, 1
	};
	
	return rz0 * rx * rz1;
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
	
	*azimuth = math::wrap_radians<double>(std::atan2(horiz_y, horiz_x) + math::pi<double>);
	*elevation = math::wrap_radians<double>(std::atan2(horiz_z, std::sqrt(horiz_x * horiz_x + horiz_y * horiz_y)));
}


double3x3 horizontal_to_right_handed()
{
	return double3x3
	{
		 0.0,  0.0,  1.0,
		 1.0,  0.0,  0.0,
		 0.0, -1.0,  0.0
	};
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
	sun_direction{0.0f, -1.0f, 0.0f},
	location{0.0f, 0.0f, 0.0f},
	jd(0.0)
{}

void weather_system::update(double t, double dt)
{
	jd += (dt * time_scale) / seconds_per_day;
	
	const float latitude = location[0];
	const float longitude = location[1];
	
	// Calculate local mean sidereal time (LMST)
	double time_correction = longitude / (math::two_pi<double> / 24.0);
	double local_jd = jd + time_correction / 24.0 - 0.5;
	double local_time = (local_jd - std::floor(local_jd)) * 24.0;
	double hour = local_time;
	double gmst = jd_to_gmst(jd);
	double lmst = gmst + longitude;
	
	// Obliquity of the ecliptic
	double ecl = math::radians<double>(23.4393 - 3.563e-7 * (jd - 2451545.0));
	
	// Solar distance in AU
	//double sr = ...
	// Apparent radius in degrees
	//double sradius = 0.2666 / sr;
	
	double3x3 ecliptic_to_horizontal = ast::ecliptic_to_horizontal(ecl, latitude, lmst);
	
	double3 sun_ecliptic = calculate_sun_ecliptic(jd);
	double3 sun_horizontal = ecliptic_to_horizontal * sun_ecliptic;
	double3 sun_spherical = ast::rectangular_to_spherical(sun_horizontal);
	double3 sun_positiond = horizontal_to_right_handed() * sun_horizontal;
	float2 sun_az_el = {static_cast<float>(sun_spherical.z) - math::pi<float>, static_cast<float>(sun_spherical.y)};
	float3 sun_position = math::normalize(float3{static_cast<float>(sun_positiond.x), static_cast<float>(sun_positiond.y), static_cast<float>(sun_positiond.z)});
	
	double3 moon_ecliptic = calculate_moon_ecliptic(jd);
	double3 moon_horizontal = ecliptic_to_horizontal * moon_ecliptic;
	moon_horizontal.z -= 1.0; // Subtract one earth radius, for position of observer
	double3 moon_spherical = ast::rectangular_to_spherical(moon_horizontal);
	double3 moon_positiond = horizontal_to_right_handed() * moon_horizontal;
	float2 moon_az_el = {static_cast<float>(moon_spherical.z) - math::pi<float>, static_cast<float>(moon_spherical.y)};
	float3 moon_position = math::normalize(float3{static_cast<float>(moon_positiond.x), static_cast<float>(moon_positiond.y), static_cast<float>(moon_positiond.z)});
	
	//std::cout << "new moon: " << math::degrees(moon_az_el[0]) << ", " << math::degrees(moon_az_el[1]) << std::endl;
	
	double3x3 moon_rotation_matrix = horizontal_to_right_handed() * ecliptic_to_horizontal;
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
	
	std::size_t hour_index = static_cast<std::size_t>(hour);
	float lerp_factor = hour - std::floor(hour);
	
	float sun_gradient_position = static_cast<float>(std::max<double>(0.0, ((sun_az_el[1] + math::half_pi<double>) / math::pi<double>)));
	float moon_gradient_position = static_cast<float>(std::max<double>(0.0, ((moon_az_el[1] + math::half_pi<double>) / math::pi<double>)));
	float sky_gradient_position = sun_gradient_position;
	float ambient_gradient_position = sun_gradient_position;
	
	if (sky_pass)
	{
		float3 horizon_color = interpolate_gradient(horizon_colors, sun_gradient_position);
		float3 zenith_color = interpolate_gradient(zenith_colors, sun_gradient_position);
		float3 sun_color = interpolate_gradient(sun_colors, sun_gradient_position);
		float3 moon_color = interpolate_gradient(moon_colors, moon_gradient_position);
		float3 ambient_color = interpolate_gradient(ambient_colors, ambient_gradient_position);
		
		sun_light->set_color(sun_color);
		sun_light->set_intensity(1.0f);
		moon_light->set_color(moon_color);
		moon_light->set_intensity(1.0f);
		ambient_light->set_color(ambient_color);
		ambient_light->set_intensity(0.5f);
		
		sky_pass->set_horizon_color(horizon_color);
		sky_pass->set_zenith_color(zenith_color);
		sky_pass->set_time_of_day(static_cast<float>(hour * 60.0 * 60.0));
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
	position *= static_cast<float>(gradient.size() - 1);
	int index0 = static_cast<int>(position) % gradient.size();
	int index1 = (index0 + 1) % gradient.size();
	return math::lerp<float3>(gradient[index0], gradient[index1], position - std::floor(position));
}
