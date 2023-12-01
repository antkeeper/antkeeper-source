// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_TIME_UT1_HPP
#define ANTKEEPER_PHYSICS_TIME_UT1_HPP

#include <engine/math/numbers.hpp>

namespace physics {
namespace time {

/// UT1 universal time.
namespace ut1 {

/**
 * Converts UT1 to JD.
 *
 * @param t UT1 time.
 * @return JD time.
 */
template <class T>
T to_jd(T t)
{
	return t + T(2451545);
}

/**
 * Calculates the Earth Rotation Angle (ERA) at a given UT1 date.
 *
 * @param t J2000 UT1 date.
 * @return ERA at the given date, in radians.
 */
template <class T>
T era(T t)
{
	return math::two_pi<T> * (T(0.7790572732640) + T(1.00273781191135448) * t);
}

} // namespace ut1

} // namespace time
} // namespace physics

#endif // ANTKEEPER_PHYSICS_TIME_UT1_HPP
