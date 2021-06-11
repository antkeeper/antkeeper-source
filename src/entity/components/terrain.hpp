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

#ifndef ANTKEEPER_ENTITY_COMPONENT_TERRAIN_HPP
#define ANTKEEPER_ENTITY_COMPONENT_TERRAIN_HPP

#include "renderer/material.hpp"
#include <functional>

namespace entity {
namespace component {

struct terrain
{
	/// Function object which returns elevation (in meters) given latitude (radians) and longitude (radians).
	std::function<double(double, double)> elevation;
	
	/// Maximum level of detail (maximum quadtree depth level)
	std::size_t max_lod;
	
	/// Material for terrain patches;
	material* patch_material;
};

} // namespace component
} // namespace entity

#endif // ANTKEEPER_ENTITY_COMPONENT_TERRAIN_HPP
