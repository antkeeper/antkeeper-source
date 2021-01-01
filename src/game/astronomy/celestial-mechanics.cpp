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

#include "celestial-mechanics.hpp"
#include "math/angles.hpp"
#include <cmath>

namespace ast
{

double solve_kepler(double ec, double ma, double tolerance, std::size_t iterations)
{
	// Approximate eccentric anomaly, E
	double e0 = ma + ec * std::sin(ma) * (1.0 + ec * std::cos(ma));
	
	// Iteratively converge E0 and E1
	for (std::size_t i = 0; i < iterations; ++i)
	{
		double e1 = e0 - (e0 - ec * std::sin(e0) - ma) / (1.0 - ec * std::cos(e0));
		double error = std::abs(e1 - e0);
		e0 = e1;
		
		if (error < tolerance)
			break;
	}
	
	return e0;
}

double3 orbital_elements_to_ecliptic(const orbital_elements& elements, double ke_tolerance, std::size_t ke_iterations)
{
	// Calculate semi-minor axis, b
	double b = elements.a * std::sqrt(1.0 - elements.ec * elements.ec);
	
	// Solve Kepler's equation for eccentric anomaly, E
	double ea = solve_kepler(elements.ec, elements.ma, ke_tolerance, ke_iterations);
	
	// Calculate radial distance, r; and true anomaly, v
	double xv = elements.a * (std::cos(ea) - elements.ec);
	double yv = b * std::sin(ea);
	double r = std::sqrt(xv * xv + yv * yv);
	double v = std::atan2(yv, xv);
	
	// Calculate true longitude, l
	double l = elements.w + v;

	// Transform vector (r, 0, 0) from local coordinates to ecliptic coordinates
	// = Rz(-omega) * Rx(-i) * Rz(-l) * r
	double cos_om = std::cos(elements.om);
	double sin_om = std::sin(elements.om);
	double cos_i = std::cos(elements.i);
	double cos_l = std::cos(l);
	double sin_l = std::sin(l);
	return double3
	{
		r * (cos_om * cos_l - sin_om * sin_l * cos_i),
		r * (sin_om * cos_l + cos_om * sin_l * cos_i),
		r * sin_l * std::sin(elements.i)
	};
}

} // namespace ast
