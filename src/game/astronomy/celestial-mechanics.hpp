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
 * Contains six orbital elements which describe a Keplerian orbit.
 */
struct orbital_elements
{
	double ec; ///< Eccentricity, e
	double a;  ///< Semi-major axis, a
	double i;  ///< Inclination, i (radians)
	double om; ///< Longitude of the ascending node, OMEGA (radians)
	double w;  ///< Argument of periapsis, w (radians)
	double ma; ///< Mean anomaly, M (radians)
};

/**
 * Orbital state vectors.
 */
struct orbital_state
{
	double3 r; ///< Cartesian position, r.
	double3 v; ///< Cartesian velocity, v.
};

/**
 * Iteratively solves Kepler's equation for eccentric anomaly, E.
 *
 * @param ec Eccentricity, e.
 * @param ma Mean anomaly, M (radians).
 * @param tolerance Tolerance of solution.
 * @param iterations Maximum number of iterations.
 * @return Eccentric anomaly.
 */
double solve_kepler(double ec, double ma, double tolerance, std::size_t iterations);

/**
 * Calculates the ecliptic rectangular orbital position from Keplerian orbital elements.
 *
 * @note Only works for elliptical orbits.
 *
 * @param elements Orbital elements.
 * @param ke_tolerance Kepler's equation tolerance.
 * @param ke_iterations Kepler's equation iterations.
 * @return Orbital state.
 */
double3 orbital_elements_to_ecliptic(const orbital_elements& elements, double ke_tolerance, std::size_t ke_iterations);

} // namespace ast

#endif // ANTKEEPER_CELESTIAL_MECHANICS_HPP
