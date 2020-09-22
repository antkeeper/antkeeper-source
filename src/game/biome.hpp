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

#ifndef ANTKEEPER_BIOME_HPP
#define ANTKEEPER_BIOME_HPP

#include <string>
#include "utility/fundamental-types.hpp"
class material;

struct biome
{
	std::string name;
	
	// Terrain
	material* terrain_material;
	
	// Weather
	float3 ambient_color;
	float ambient_intensity;
	float sun_azimuth;
	float sun_elevation;
	float3 sun_color;
	float sun_intensity;
	float sun_angular_radius;
	float3 horizon_color;
	float3 zenith_color;
	float wind_speed;
	float wind_direction;
	
	// Traits
};

#endif // ANTKEEPER_BIOME_HPP
