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

#ifndef ANTKEEPER_CELESTIAL_COORDINATES_HPP
#define ANTKEEPER_CELESTIAL_COORDINATES_HPP

#include "utility/fundamental-types.hpp"

namespace ast
{

/**
 * Converts rectangular coordinates to spherical coordinates.
 *
 * @param rectangular Rectangular coordinates.
 * @return Equivalent spherical coordinates, in the ISO order of radial distance, polar angle (radians), and azimuthal angle (radians).
 */
double3 rectangular_to_spherical(const double3& rectangular);

/**
 * Convert spherical coordinates to rectangular coordinates.
 *
 * @param spherical Spherical coordinates, in the ISO order of radial distance, polar angle (radians), and azimuthal angle (radians).
 * @return Equivalent rectangular coordinates.
 */
double3 spherical_to_rectangular(const double3& spherical);

/**
 * Produces a matrix which transforms rectangular coordinates from ecliptic space to equatorial space.
 *
 * @param ecl Obliquity of the ecliptic, in radians.
 * @return Transformation matrix.
 */
double3x3 ecliptic_to_equatorial(double ecl);

/**
 * Produces a matrix which transforms coordinates from ecliptic space to horizontal space.
 *
 * @param ecl Obliquity of the ecliptic, in radians.
 * @param lat Observer's latitude, in radians.
 * @param lst Local sidereal time, in radians.
 * @return Transformation matrix.
 */
double3x3 ecliptic_to_horizontal(double ecl, double lat, double lst);

/**
 * Produces a matrix which transforms rectangular coordinates from equatorial space to ecliptic space.
 *
 * @param ecl Obliquity of the ecliptic, in radians.
 * @return Transformation matrix.
 */
double3x3 equatorial_to_ecliptic(double ecl);

/**
 * Produces a matrix which transforms rectangular coordinates from equatorial space to horizontal space.
 *
 * @param lat Observer's latitude, in radians.
 * @param lst Local sidereal time, in radians.
 * @return Transformation matrix.
 */
double3x3 equatorial_to_horizontal(double lat, double lst);

/**
 * Produces a matrix which transforms rectangular coordinates from horizontal space to equatorial space.
 *
 * @param lat Observer's latitude, in radians.
 * @param lst Local sidereal time, in radians.
 * @return Transformation matrix.
 */
double3x3 horizontal_to_equatorial(double lat, double lst);

/**
 * Produces a matrix which transforms rectangular coordinates from horizontal space to ecliptic space.
 *
 * @param ecl Obliquity of the ecliptic, in radians.
 * @param lat Observer's latitude, in radians.
 * @param lst Local sidereal time, in radians.
 * @return Transformation matrix.
 */
double3x3 horizontal_to_ecliptic(double ecl, double lat, double lst);

/// Matrix which transforms coordinates from horizontal space to a right-handed coordinate system.
constexpr double3x3 horizontal_to_right_handed =
{
	0.0,  0.0, 1.0,
	1.0,  0.0, 0.0,
	0.0, -1.0, 0.0
};

} // namespace ast

#endif // ANTKEEPER_CELESTIAL_COORDINATES_HPP
