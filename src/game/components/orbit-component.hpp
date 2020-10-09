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

#ifndef ANTKEEPER_ECS_ORBIT_COMPONENT_HPP
#define ANTKEEPER_ECS_ORBIT_COMPONENT_HPP

#include "game/astronomy/celestial-mechanics.hpp"

namespace ecs {

struct orbit_component
{
	double a;    ///< Semi-major axis, a
	double d_a;
	
	double ec;   ///< Eccentricity, e
	double d_ec;
	
	double w;    ///< Argument of periapsis, w (radians)
	double d_w;
	
	double ma;   ///< Mean anomaly, M (radians)
	double d_ma;
	
	double i;    ///< Inclination, i (radians)
	double d_i;
	
	double om;   ///< Longitude of the ascending node, OMEGA (radians)
	double d_om;
};

} // namespace ecs

#endif // ANTKEEPER_ECS_ORBIT_COMPONENT_HPP

