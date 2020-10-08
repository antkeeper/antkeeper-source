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

#include "celestial-time.hpp"
#include "math/angles.hpp"
#include <cmath>

namespace ast
{

double ut_to_jd(int year, int month, int day, int hour, int minute, double second)
{
	if (month < 3)
	{
		month += 12;
		year -= 1;
	}
	
	const signed long Y = year;
	const signed long M = month;
	const signed long D = day;	
	const signed long JDN = (1461*(Y+4800+(M-14)/12))/4+(367*(M-2-12*((M-14)/12)))/12-(3*((Y+4900+(M-14)/12)/100))/4+D-32075;
	
	const double h = static_cast<double>(hour - 12) / 24.0;
	const double m = static_cast<double>(minute) / 1440.0;
	const double s = second / 86400.0;
	
	return static_cast<double>(JDN) + h + m + s;
}

double jd_to_gmst(double jd)
{
	return math::wrap_radians<double>(4.894961212 + 6.300388098 * (jd - 2451545.0));
}

double jd_to_lmst(double jd, double longitude)
{
	return gmst_to_lmst(jd_to_gmst(jd), longitude);
}

double gmst_to_lmst(double gmst, double longitude)
{
	return gmst + longitude;
}

} // namespace ast
