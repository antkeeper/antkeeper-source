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

#ifndef ANTKEEPER_ENTITY_COMPONENT_CELESTIAL_BODY_HPP
#define ANTKEEPER_ENTITY_COMPONENT_CELESTIAL_BODY_HPP

namespace entity {
namespace component {

/// A simple celestial body.
struct celestial_body
{
	/// Mean radius of the body, in meters.
	double radius;
	
	/// Mass of the body, in kilograms.
	double mass;
	
	/// Right ascension of the body's north pole at epoch, in radians.
	double pole_ra;
	
	/// Declination of the body's north pole at epoch, in radians.
	double pole_dec;
	
	/// Location of the prime meridian at epoch, as a rotation about the north pole, in radians.
	double prime_meridian;
	
	/// Sidereal rotation period, in rotations per day.
	double rotation_period;
};

} // namespace component
} // namespace entity

#endif // ANTKEEPER_ENTITY_COMPONENT_CELESTIAL_BODY_HPP
