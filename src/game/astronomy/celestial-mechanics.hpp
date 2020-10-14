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

#ifndef ANTKEEPER_CELESTIAL_MECHANICS_HPP
#define ANTKEEPER_CELESTIAL_MECHANICS_HPP

#include "utility/fundamental-types.hpp"

namespace ast
{

/**
 * Approximates the obliquity of the ecliptic.
 *
 * @param jd Julian date.
 * @return Obliquity of the ecliptic, in radians.
 */
double approx_ecliptic_obliquity(double jd);

/**
 * Approximates the ecliptic coordinates of the Earth's sun.
 *
 * @param jd Julian date.
 * @return Ecliptic rectangular geocentric coordinates of the Earth's sun, with distance in AU.
 */
double3 approx_sun_ecliptic(double jd);

/**
 * Approximates the ecliptic coordinates of the Earth's moon.
 *
 * @param jd Julian date.
 * @return Ecliptic rectangular geocentric coordinates of the Earth's moon, with distance in Earth radii.
 */
double3 approx_moon_ecliptic(double jd);

/**
 * Approximates the ecliptic rotation of the Earth's moon.
 *
 * @param jd Julian date.
 * @return Rotation matrix representing the moon's rotation in ecliptic space.
 */
double3x3 approx_moon_ecliptic_rotation(double jd);

struct kepler_orbit
{
	double a;  ///< Semi-major axis, a
	double ec; ///< Eccentricity, e
	double w;  ///< Argument of periapsis, w (radians)
	double ma; ///< Mean anomaly, M (radians)
	double i;  ///< Inclination, i (radians)
	double om; ///< Longitude of the ascending node, OMEGA (radians)
};

double3 solve_kepler(const kepler_orbit& orbit, double t);

/**
 *
 * @param a Semi-major axis, a.
 * @param ec Eccentricity, e.
 * @param w Argument of periapsis, w (radians).
 * @param ma Mean anomaly, M (radians).
 * @param i Inclination, i (radians).
 * @param om Longitude of the ascending node, OMEGA (radians).
 */
double3 solve_kepler(double a, double ec, double w, double ma, double i, double om);

} // namespace ast

#endif // ANTKEEPER_CELESTIAL_MECHANICS_HPP
