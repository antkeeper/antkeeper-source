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

#ifndef ANTKEEPER_PHYSICS_TIME_UT1_HPP
#define ANTKEEPER_PHYSICS_TIME_UT1_HPP

#include "math/constants.hpp"

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
