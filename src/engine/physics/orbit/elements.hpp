// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_ORBIT_ELEMENTS_HPP
#define ANTKEEPER_PHYSICS_ORBIT_ELEMENTS_HPP

#include <engine/math/vector.hpp>
#include <engine/math/constants.hpp>
#include <cmath>

namespace physics {
namespace orbit {

/// Set of six Keplerian elements required to uniquely identify an orbit.
/// @tparam T Scalar type.
template <class T>
struct elements
{
	/// Scalar type.
	typedef T scalar_type;
	
	/// Eccentricity (e).
	scalar_type ec;
	
	/// Semimajor axis (a).
	scalar_type a;
	
	/// Inclination (i), in radians.
	scalar_type in;
	
	/// Right ascension of the ascending node (OMEGA), in radians.
	scalar_type om;
	
	/// Argument of periapsis (omega), in radians.
	scalar_type w;
	
	/// Mean anomaly (M) at epoch, in radians.
	scalar_type ma;
};

/// Calculates the period of an elliptical orbit according to Kepler's third law.
/// @param a Semimajor axis (a).
/// @param gm Standard gravitational parameter (GM).
/// @return Orbital period (T).
template <class T>
T period(T a, T gm);

/// Calculates the mean motion (n) of an orbit.
/// @param a Semimajor axis (a).
/// @param gm Standard gravitational parameter (GM).
/// @return Mean motion (n), in radians per unit time.
template <class T>
T mean_motion(T a, T gm);

/// Calculates the mean motion (n) of an orbit.
/// @param t Orbital period (T).
/// @return Mean motion (n), in radians per unit time.
template <class T>
T mean_motion(T t);

/// Derives the argument of the periapsis (omega) of an orbit, given the longitude of periapsis (pomega) and longitude of the ascending node (OMEGA).
/// @param lp Longitude of the periapsis (pomega), in radians.
/// @param om Right ascension of the ascending node (OMEGA), in radians.
/// @return Argument of the periapsis (omega), in radians.
template <class T>
T argument_periapsis(T om, T lp);

/// Derives the longitude of the periapsis (pomega) of an orbit, given the argument of periapsis (omega) and longitude of the ascending node (OMEGA).
/// @param w Argument of the periapsis (omega), in radians.
/// @param om Right ascension of the ascending node (OMEGA), in radians.
/// @return Longitude of the periapsis (pomega), in radians.
template <class T>
T longitude_periapsis(T om, T w);

/// Derives the semiminor axis (b) of an orbit, given the semimajor axis (a) and eccentricity (e).
/// @param a Semimajor axis (a).
/// @param ec Eccentricity (e).
/// @return Semiminor axis (b).
template <class T>
T semiminor_axis(T a, T ec);

/// Derives the semi-latus rectum (l) of an orbit, given the semimajor axis (a) and eccentricity (e).
/// @param a Semimajor axis (a).
/// @param ec Eccentricity (e).
/// @return Semi-latus rectum (l).
template <class T>
T semilatus_rectum(T a, T ec);

template <class T>
T period(T a, T gm)
{
	return math::two_pi<T> * std::sqrt((a * a * a) / gm);
}

template <class T>
T mean_motion(T a, T gm)
{
	return std::sqrt((a * a * a) / gm);
}

template <class T>
T mean_motion(T t)
{
	return math::two_pi<T> / t;
}

template <class T>
T argument_periapsis(T om, T lp)
{
	return lp - om;
}

template <class T>
T longitude_periapsis(T om, T w)
{
	return w + om;
}

template <class T>
T semiminor_axis(T a, T ec)
{
	return a * std::sqrt(T(1) - ec * ec);
}

template <class T>
T semilatus_rectum(T a, T ec)
{
	return a * (T(1) - ec * ec);
}

} // namespace orbit
} // namespace physics

#endif // ANTKEEPER_PHYSICS_ORBIT_ELEMENTS_HPP
