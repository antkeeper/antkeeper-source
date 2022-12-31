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

#ifndef ANTKEEPER_PHYSICS_ORBIT_ANOMALY_HPP
#define ANTKEEPER_PHYSICS_ORBIT_ANOMALY_HPP

#include "math/constants.hpp"
#include <cmath>

namespace physics {
namespace orbit {

/**
 * Orbital anomaly functions.
 */
namespace anomaly {

/**
 * Derives the eccentric anomaly given eccentricity and true anomaly.
 *
 * @param ec Eccentricity (e).
 * @param ta True anomaly (nu).
 * @return Eccentric anomaly (E).
 */
template <class T>
T true_to_eccentric(T ec, T ta);

/**
 * Derives the mean anomaly given eccentricity and true anomaly.
 *
 * @param ec Eccentricity (e).
 * @param ta True anomaly (nu).
 * @return Mean anomaly (M).
 */
template <class T>
T true_to_mean(T ec, T ta);

/**
 * Derives the true anomaly given eccentricity and eccentric anomaly.
 *
 * @param ec Eccentricity (e).
 * @param ea Eccentric anomaly (E).
 * @return True anomaly (nu).
 */
template <class T>
T eccentric_to_true(T ec, T ea);

/**
 * Derives the mean anomaly given eccentricity and eccentric anomaly.
 *
 * @param ec Eccentricity (e).
 * @param ea Eccentric anomaly (E).
 * @return Mean anomaly (M).
 */
template <class T>
T eccentric_to_mean(T ec, T ea);

/**
 * Iteratively derives the eccentric anomaly given eccentricity and mean anomaly.
 *
 * @param ec Eccentricity (e).
 * @param ma Mean anomaly (M).
 * @param iterations Maximum number of iterations.
 * @param tolerance Solution error tolerance.
 * @return Eccentric anomaly (E).
 *
 * @see Murison, Marc. (2006). A Practical Method for Solving the Kepler Equation. 10.13140/2.1.5019.6808.
 */
template <class T>
T mean_to_eccentric(T ec, T ma, std::size_t iterations, T tolerance);

/**
 * Iteratively derives the true anomaly given eccentricity and mean anomaly.
 *
 * @param ec Eccentricity (e).
 * @param ma Mean anomaly (M).
 * @param iterations Maximum number of iterations.
 * @param tolerance Solution error tolerance.
 * @return True anomaly (nu).
 */
template <class T>
T mean_to_true(T ec, T ma, std::size_t iterations, T tolerance);

template <class T>
T true_to_eccentric(T ec, T ta)
{
	// Parabolic orbit
	if (ec == T(1))
		return std::tan(ta * T(0.5));
	
	// Hyperbolic orbit
	if (ec > T(1))
		return std::acosh((ec + std::cos(ta)) / (T(1) + ec * std::cos(ta))) * ((ta < T(0)) ? T(-1) : T(1));
	
	// Elliptic orbit
	return std::atan2(std::sqrt(T(1) - ec * ec) * std::sin(ta), std::cos(ta) + ec);
}

template <class T>
T true_to_mean(T ec, T ta)
{
    return eccentric_to_mean(ec, true_to_eccentric(ec, ta));
}

template <class T>
T eccentric_to_true(T ec, T ea)
{
	// Parabolic orbit
	if (ec == T(1))
		return std::atan(ea) * T(2);
	
	// Hyperbolic orbit
	if (ec > T(1))
		return std::atan(std::sqrt((ec + T(1)) / (ec - T(1))) * std::tanh(ea * T(0.5))) * T(2);
	
	// Elliptic orbit
	return std::atan2(sqrt(T(1) - ec * ec) * std::sin(ea), std::cos(ea) - ec);
}

template <class T>
T eccentric_to_mean(T ec, T ea)
{
	// Parabolic orbit
	if (ec == T(1))
		return (ea * ea * ea) /  T(6) + ea * T(0.5);
	
	// Hyperbolic orbit
	if (ec > T(1))
		return ec * std::sinh(ea) - ea;
	
	// Elliptic orbit
	return ea - ec * std::sin(ea);
}

template <class T>
T mean_to_eccentric(T ec, T ma, std::size_t iterations, T tolerance)
{
	// Wrap mean anomaly to `[-pi, pi]`
	ma = std::remainder(ma, math::two_pi<T>);
	
	// Third-order approximation of eccentric anomaly starting value, E0
	const T t33 = std::cos(ma);
	const T t34 = ec * ec;
	const T t35 = t34 * ec;
	T ea0 = ma + (T(-0.5) * t35 + ec + (t34 + T(1.5) * t33 * t35) * t33) * std::sin(ma);
	
	// Iteratively converge E0 and E1
	for (std::size_t i = 0; i < iterations; ++i)
	{
		// Third-order approximation of eccentric anomaly, E1
		const T t1 = std::cos(ea0);
		const T t2 = T(-1) + ec * t1;
		const T t3 = std::sin(ea0);
		const T t4 = ec * t3;
		const T t5 = -ea0 + t4 + ma;
		const T t6 = t5 / (T(0.5) * t5 * t4 / t2 + t2);
		const T ea1 = ea0 - (t5 / ((T(0.5) * t3 - (T(1) / T(6)) * t1 * t6) * ec * t6 + t2));
		
		// Determine solution error
		const T error = std::abs(ea1 - ea0);
		
		// Set E0 to E1
		ea0 = ea1;
		
		// Break if solution is within error tolerance
		if (error < tolerance)
			break;
	}
	
	return ea0;
}

template <class T>
T mean_to_true(T ec, T ma, std::size_t iterations, T tolerance)
{
	eccentric_to_true(ec, mean_to_eccentric(ec, ma, iterations, tolerance));
}

} // namespace anomaly
} // namespace orbit
} // namespace physics

#endif // ANTKEEPER_PHYSICS_ORBIT_ANOMALY_HPP
