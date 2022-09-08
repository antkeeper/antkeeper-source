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

#ifndef ANTKEEPER_PHYSICS_TIME_GREGORIAN_HPP
#define ANTKEEPER_PHYSICS_TIME_GREGORIAN_HPP

#include "physics/time/jd.hpp"

namespace physics {
namespace time {

// Gregorian calender time.
namespace gregorian {

/**
 * Calculates the JD time from a Gregorian date and time. Valid for all dates after November 23, −4713.
 * 
 * @param year Astronomical year numbering. 1 BC is `0`, 2 BC is `-1`.
 * @param month Month number on `[1, 12]`.
 * @param day Day number on `[1, 31]`.
 * @param hour Hour number on `[0, 23]`.
 * @param minute Minute number on `[0, 59]`.
 * @param second Fractional second on `[0.0, 60.0)`.
 * @param utc UTC offset.
 *
 * @return JD time.
 *
 * @see L. E. Doggett, Ch. 12, "Calendars", p. 606, in Seidelmann 1992
 */
template <class T>
T to_jd(int year, int month, int day, int hour, int minute, T second, T utc)
{
	T jdn = static_cast<T>((1461 * (year + 4800 + (month - 14) / 12)) / 4 + (367 * (month - 2 - 12 * ((month - 14) / 12))) / 12 - (3 * ((year + 4900 + (month - 14) / 12) / 100)) / 4 + day - 32075);
	return jdn + static_cast<T>(hour - 12) / T(24) + static_cast<T>(minute) / T(1440) + static_cast<T>(second) / T(86400) + utc / T(24);
}

/**
 * Calculates the UT1 time from a Gregorian date and time. Valid for all dates after November 23, −4713.
 * 
 * @param year Astronomical year numbering. 1 BC is `0`, 2 BC is `-1`.
 * @param month Month number on `[1, 12]`.
 * @param day Day number on `[1, 31]`.
 * @param hour Hour number on `[0, 23]`.
 * @param minute Minute number on `[0, 59]`.
 * @param second Fractional second on `[0.0, 60.0)`.
 * @param utc UTC offset.
 *
 * @return UT1 time.
 */
template <class T>
T to_ut1(int year, int month, int day, int hour, int minute, T second, T utc)
{
	return physics::time::jd::to_ut1<T>(to_jd<T>(year, month, day, hour, minute, second, utc));
}

} // namespace gregorian

} // namespace time
} // namespace physics

#endif // ANTKEEPER_PHYSICS_TIME_GREGORIAN_HPP
