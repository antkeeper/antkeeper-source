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

#ifndef ANTKEEPER_BIOME_HPP
#define ANTKEEPER_BIOME_HPP

#include <string>
#include "utility/fundamental-types.hpp"
class material;
class image;

struct biome
{
	std::string name;
	
	/// Latitude (radians), longitude (radians), altitude (meters)
	float3 location;
	
	// Terrain
	material* terrain_material;
	
	// Weather
	image* sky_palette;
	image* sun_palette;
	image* moon_palette;
	image* ambient_palette;
	image* shadow_palette;
	
	// Traits
};

#endif // ANTKEEPER_BIOME_HPP
