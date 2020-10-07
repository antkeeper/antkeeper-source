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

#include "celestial-coordinates.hpp"
#include <cmath>

namespace ast
{

double3 rectangular_to_spherical(const double3& rectangular)
{
	const double xx_yy = rectangular.x * rectangular.x + rectangular.y * rectangular.y;
	
	return double3
	{
		std::sqrt(xx_yy + rectangular.z * rectangular.z),
		std::atan2(rectangular.z, std::sqrt(xx_yy)),
		std::atan2(rectangular.y, rectangular.x)
	};
}

double3 spherical_to_rectangular(const double3& spherical)
{
	return double3
	{
		spherical[0] * std::sin(spherical[1]) * std::cos(spherical[2]),
		spherical[0] * std::sin(spherical[1]) * std::sin(spherical[2]),
		spherical[0] * std::cos(spherical[1]),
	};
}

double3x3 ecliptic_to_equatorial(double ecl)
{
	const double c_ecl = std::cos(ecl);
	const double s_ecl = std::sin(ecl);
	
	return double3x3
	{
		1.0, 0.0, 0.0,
		0.0, c_ecl, s_ecl,
		0.0, -s_ecl, c_ecl
	};
}

double3x3 ecliptic_to_horizontal(double ecl, double lat, double lst)
{
	const double c_ecl = std::cos(ecl);
	const double s_ecl = std::sin(ecl);
	const double c_lat = std::cos(lat);
	const double s_lat = std::sin(lat);
	const double c_lst = std::cos(lst);
	const double s_lst = std::sin(lst);
	
	return double3x3
	{
		s_lat * c_lst, s_lst, c_lat * c_lst,
		s_lat * s_lst * c_ecl - c_lat * s_ecl, -c_lst * c_ecl, c_lat * s_lst * c_ecl + s_lat * s_ecl,
		s_lat * s_lst * -s_ecl - c_lat * c_ecl,  c_lst * s_ecl, c_lat * s_lst * -s_ecl + s_lat * c_ecl
	};
}

double3x3 equatorial_to_ecliptic(double ecl)
{
	const double c_ecl = std::cos(ecl);
	const double s_ecl = std::sin(ecl);
	
	return double3x3
	{
		1.0, 0.0, 0.0,
		0.0, c_ecl, -s_ecl,
		0.0, s_ecl, c_ecl
	};
}

double3x3 equatorial_to_horizontal(double lat, double lst)
{
	const double c_lat = std::cos(lat);
	const double s_lat = std::sin(lat);
	const double c_lst = std::cos(lst);
	const double s_lst = std::sin(lst);
	
	return double3x3
	{
		s_lat * c_lst, s_lst, c_lat * c_lst,
		s_lat * s_lst, -c_lst, c_lat * s_lst,
		-c_lat, 0.0, s_lat
	};
}

double3x3 horizontal_to_equatorial(double lat, double lst)
{
	const double c_lat = std::cos(lat);
	const double s_lat = std::sin(lat);
	const double c_lst = std::cos(lst);
	const double s_lst = std::sin(lst);
	
	return double3x3
	{
		c_lst * s_lat, s_lst * s_lat, -c_lat,
		s_lst, c_lst, 0.0,
		c_lst * c_lat, s_lst * c_lat, s_lat
	};
}

double3x3 horizontal_to_ecliptic(double ecl, double lat, double lst)
{
	const double c_ecl = std::cos(ecl);
	const double s_ecl = std::sin(ecl);
	const double c_lat = std::cos(lat);
	const double s_lat = std::sin(lat);
	const double c_lst = std::cos(lst);
	const double s_lst = std::sin(lst);
	
	return double3x3
	{
		s_lat * c_lst, s_lat * s_lst * c_ecl - c_lat * s_ecl, s_lat * s_lst * -s_ecl - c_lat * c_ecl,
		s_lst, -c_lst * c_ecl, c_lst * s_ecl,
		c_lat * c_lst, c_lat * s_lst * c_ecl + s_lat * s_ecl, c_lat * s_lst * -s_ecl + s_lat * c_ecl
	};
}

} // namespace ast
