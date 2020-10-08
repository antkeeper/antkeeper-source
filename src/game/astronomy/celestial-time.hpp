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

#ifndef ANTKEEPER_CELESTIAL_TIME_HPP
#define ANTKEEPER_CELESTIAL_TIME_HPP

namespace ast
{

/**
 * Calculates the Julian date from a universal time.
 *
 * @param year Gregorian year, with 1 BC as 0.
 * @param month Gregorian month, on [1, 12].
 * @param day Gregorian day, on [1, 31].
 * @param hour Hour, on [0, 23].
 * @param minute Minute, on [0, 59].
 * @param second Second on [0, 60).
 * @return Julian date with fractional day.
 */
double ut_to_jd(int year, int month, int day, int hour, int minute, double second);

/**
 * Calculates the Greenwich mean sidereal time (GMST) from a Julian date.
 *
 * @param jd Julian date.
 * @return GMST, in radians.
 */
double jd_to_gmst(double jd);

/**
 * Calculates local mean sidereal time (LMST) from a Julian date.
 *
 * @param jd Julian date.
 * @param longitude Longitude of the observer, in radians.
 */
double jd_to_lmst(double jd, double longitude);

/**
 * Calculates local mean sidereal time (LMST) from Greenwich mean sidereal time (GMST).
 *
 * @param gmst Greenwich mean sidereal time, in radians.
 * @param longitude Longitude of the observer, in radians.
 * @return Local mean sidereal time, in radians.
 */
double gmst_to_lmst(double gmst, double longitude);

} // namespace ast

#endif // ANTKEEPER_CELESTIAL_TIME_HPP
