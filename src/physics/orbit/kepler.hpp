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

#ifndef ANTKEEPER_PHYSICS_ORBIT_KEPLER_HPP
#define ANTKEEPER_PHYSICS_ORBIT_KEPLER_HPP

#include <cmath>

namespace physics {
namespace orbit {

/**
 * Iteratively solves Kepler's equation for eccentric anomaly (E).
 *
 * @param ec Eccentricity (e).
 * @param ma Mean anomaly (M).
 * @param iterations Maximum number of iterations.
 * @param tolerance Solution error tolerance.
 * @return Eccentric anomaly (E).
 */
template <class T>
T kepler_ea(T ec, T ma, std::size_t iterations, T tolerance = T(0));

/**
 * Solves Kepler's equation for mean anomaly (M).
 *
 * @param ec Eccentricity (e).
 * @param ea Eccentric anomaly (E).
 * @return Mean anomaly (M).
 */
template <class T>
T kepler_ma(T ec, T ea);

template <class T>
T kepler_ea(T ec, T ma, std::size_t iterations, T tolerance)
{
	// Approximate eccentric anomaly, E
	T ea0 = ma + ec * std::sin(ma) * (T(1.0) + ec * std::cos(ma));
	
	// Iteratively converge E0 and E1
	for (std::size_t i = 0; i < iterations; ++i)
	{
		const T ea1 = ea0 - (ea0 - ec * std::sin(ea0) - ma) / (T(1.0) - ec * std::cos(ea0));
		const T error = std::abs(ea1 - ea0);
		ea0 = ea1;
		
		if (error < tolerance)
			break;
	}
	
	return ea0;
}

template <class T>
T kepler_ma(T ec, T ea)
{
	return ea - ec * std::sin(ea);
}

} // namespace orbit
} // namespace physics

#endif // ANTKEEPER_PHYSICS_ORBIT_KEPLER_HPP
