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

#ifndef ANTKEEPER_PHYSICS_ORBIT_ELEMENTS_HPP
#define ANTKEEPER_PHYSICS_ORBIT_ELEMENTS_HPP

#include "utility/fundamental-types.hpp"
#include "physics/orbit/state.hpp"
#include <cmath>

namespace physics {
namespace orbit {

/**
 * Set of six Keplerian elements required to uniquely identify an orbit.
 *
 * @tparam T Scalar type.
 */
template <class T>
struct elements
{
	/// Scalar type.
	typedef T scalar_type;
	
	/// Eccentricity (e).
	scalar_type e;
	
	/// Semimajor axis (a).
	scalar_type a;
	
	/// Inclination (i), in radians.
	scalar_type i;
	
	/// Right ascension of the ascending node (OMEGA), in radians.
	scalar_type raan;
	
	/// Argument of periapsis (omega), in radians.
	scalar_type w;
	
	/// True anomaly (nu) at epoch, in radians.
	scalar_type ta;
};

/**
 * Derives the longitude of the periapsis (pomega) of an orbit, given the argument of periapsis (omega) and longitude of the ascending node (OMEGA).
 *
 * @param w Argument of periapsis (omega), in radians.
 * @param raan Right ascension of the ascending node (OMEGA), in radians.
 * @return Longitude of the periapsis (pomega), in radians.
 */
template <class T>
T derive_longitude_periapsis(T w, T raan);

/**
 * Derives the semiminor axis (b) of an orbit, given the semimajor axis (a) and eccentricity (e).
 *
 * @param a Semimajor axis (a).
 * @param e Eccentricity (e).
 * @return Semiminor axis (b).
 */
template <class T>
T derive_semiminor_axis(T a, T e);

template <class T>
T derive_longitude_periapsis(T w, T raan)
{
	return w + raan;
}

template <class T>
T derive_semiminor_axis(T a, T e)
{
	return a * std::sqrt(T(1) - e * e);
}

} // namespace orbit
} // namespace physics

#endif // ANTKEEPER_PHYSICS_ORBIT_ELEMENTS_HPP
