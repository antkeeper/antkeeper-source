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
#include "physics/orbit/elements.hpp"
#include "physics/orbit/state.hpp"
#include "math/se3.hpp"

namespace entity {
namespace component {

struct orbit
{
	/// Parent body.
	entity::id parent;
	
	/// Keplerian orbital elements at epoch.
	physics::orbit::elements<double> elements;
	
	/// Orbital semi-minor axis (b) at epoch.
	double semiminor_axis;
	
	/// Orbital mean motion (n) at epoch.
	double mean_motion;
	
	/// Transformation from the PQW frame to the BCI frame of the parent body.
	math::transformation::se3<double> pqw_to_bci;
	
	/// Orbital Cartesian position of the body in the BCI frame of the parent body.
	math::vector3<double> bci_position;
	
	/// Orbital Cartesian position of the body in the ICRF frame.
	math::vector3<double> icrf_position;
};

} // namespace component
} // namespace entity

#endif // ANTKEEPER_ENTITY_COMPONENT_ORBIT_HPP
