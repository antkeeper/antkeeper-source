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
	
	/*
	/// Quadratic e coefficients for the right ascension of the body's north pole, in radians. Right ascension is calculated as `x + y * T + z * T^2`, where `T` is the Julian centuries (36525 days) from epoch.
	double3 pole_ra;
	
	/// Quadratic coefficients for the declination of the body's north pole, in radians. Declination is calculated as `x + y * T + z * T^2`, where `T` is the Julian centuries (36525 days) from epoch.
	double3 pole_ra;
	
	/// Quadratic coefficients for the rotation state of the body's prime meridian, in radians. Prime meridian rotation is calculated as `x + y * d + z * d^2`, where `d` is the days from epoch.
	double3 prime_meridian;
	
	/// Linear coefficients of the nutation and precession angles, in radians. Angles are calculated as `x + y * d`, where `d` is the days from epoch.
	std::vector<double2> nutation_precession_angles;
	std::vector<double> nutation_precession_ra;
	std::vector<double> nutation_precession_dec;
	std::vector<double> nutation_precession_pm;
	*/
	
	/// Sidereal rotation period, in rotations per day.
	double rotation_period;
	
	/// Geometric albedo
	double albedo;
};

} // namespace component
} // namespace entity

#endif // ANTKEEPER_ENTITY_COMPONENT_CELESTIAL_BODY_HPP
