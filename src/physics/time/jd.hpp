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

#ifndef ANTKEEPER_PHYSICS_TIME_JD_HPP
#define ANTKEEPER_PHYSICS_TIME_JD_HPP

namespace physics {
namespace time {

// Julian day (JD)
namespace jd {

/**
 * Converts JD to UT1
 *
 * @param t JD time.
 * @return UT1 time.
 */
template <class T>
T to_ut1(T t)
{
	return t - T(2451545);
}

} // namespace jd

} // namespace time
} // namespace physics

#endif // ANTKEEPER_PHYSICS_TIME_JD_HPP
