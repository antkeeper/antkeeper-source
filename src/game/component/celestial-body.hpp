/*
 * Copyright (C) 2023  Christopher J. Howard
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

#ifndef ANTKEEPER_GAME_COMPONENT_CELESTIAL_BODY_HPP
#define ANTKEEPER_GAME_COMPONENT_CELESTIAL_BODY_HPP

namespace game {
namespace component {

/// A simple celestial body.
struct celestial_body
{
	/// Mean radius of the body, in meters.
	double radius;
	
	/// Mass of the body, in kilograms.
	double mass;
	
	/// Polynomial coefficients, in descending order of degree, of the right ascension of the body's north pole, in radians, w.r.t. Julian centuries (36525 days) from epoch.
	std::vector<double> pole_ra;
	
	/// Polynomial coefficients, in descending order of degree, of the declination of the body's north pole, in radians, w.r.t. Julian centuries (36525 days) from epoch.
	std::vector<double> pole_dec;
	
	/// Polynomial coefficients, in descending order of degree, of the rotation state of the body's prime meridian, in radians, w.r.t. days from epoch.
	std::vector<double> prime_meridian;
	
	/*
	/// Polynomial coefficients, in descending order of degree, of the nutation and precession angles, in radians. Angles are calculated as `x + y * d`, where `d` is the days from epoch.
	std::vector<std::vector<double>> nutation_precession_angles;
	
	/// Nutation and precession amplitudes of the right ascension of the body's north pole, in radians.
	std::vector<double> nutation_precession_ra;
	
	/// Nutation and precession amplitudes of the declination of the body's north pole, in radians.
	std::vector<double> nutation_precession_dec;
	
	/// Nutation and precession amplitudes of the rotation state of the body's prime meridian, in radians.
	std::vector<double> nutation_precession_pm;
	*/
	
	/// Geometric albedo
	double albedo;
};

} // namespace component
} // namespace game

#endif // ANTKEEPER_GAME_COMPONENT_CELESTIAL_BODY_HPP
