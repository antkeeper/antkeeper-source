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

#ifndef ANTKEEPER_ENTITY_COMPONENT_ORBIT_HPP
#define ANTKEEPER_ENTITY_COMPONENT_ORBIT_HPP

#include "entity/id.hpp"
#include "math/vector-type.hpp"

namespace entity {
namespace component {

struct orbit
{
	/// Entity ID of the parent orbit.
	entity::id parent;
	
	/// Index of the orbit in the ephemeris.
	int ephemeris_index;
	
	/// Orbit scale, for two-body orbits with one ephemeris item.
	double scale;
	
	/// Cartesian position of the orbit, w.r.t. the ICRF frame.
	math::vector3<double> position;
};

} // namespace component
} // namespace entity

#endif // ANTKEEPER_ENTITY_COMPONENT_ORBIT_HPP
