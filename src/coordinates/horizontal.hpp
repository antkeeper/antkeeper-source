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

#ifndef ANTKEEPER_COORDINATES_HORIZONTAL_HPP
#define ANTKEEPER_COORDINATES_HORIZONTAL_HPP

#include "coordinates/rectangular.hpp"
#include "coordinates/spherical.hpp"
#include "utility/fundamental-types.hpp"
#include <cmath>

namespace coordinates {

namespace rectangular {

	/// Rectangular local horizontal coordinate system in which the x-axis points north, the y-axis points east, and the z-axis points to the vertical.
	namespace horizontal {

		/**
		 * Produces a matrix which rotates rectangular coordinates from local horizontal space into equatorial space.
		 *
		 * @param lat Observer's latitude, in radians.
		 * @param lst Local sidereal time, in radians.
		 * @return Rotation matrix.
		 *
		 * @see coordinates::equatorial
		 */
		template <class T>
		math::matrix3<T> to_equatorial(T lat, T lst);
		
		/**
		 * Rotates rectangular coordinates from local horizontal space into equatorial space.
		 *
		 * @param v Rectangular coordinates in local horizontal space.
		 * @param lat Observer's latitude, in radians.
		 * @param lst Local sidereal time, in radians.
		 * @return Rectangular coordinates in equatorial space.
		 *
		 * @see coordinates::equatorial
		 */
		template <class T>
		math::vector3<T> to_equatorial(const math::vector3<T>& v, T lat, T lst);

		/**
		 * Produces a matrix which rotates rectangular coordinates from local horizontal space into ecliptic space.
		 *
		 * @param ecl Obliquity of the ecliptic, in radians.
		 * @param lat Observer's latitude, in radians.
		 * @param lst Local sidereal time, in radians.
		 * @return Rotation matrix.
		 *
		 * @see coordinates::ecliptic
		 */
		template <class T>
		math::matrix3<T> to_ecliptic(T ecl, T lat, T lst);
		
		/**
		 * Rotates rectangular coordinates from local horizontal space into ecliptic space.
		 *
		 * @param v Rectangular coordinates in local horizontal space.
		 * @param ecl Obliquity of the ecliptic, in radians.
		 * @param lat Observer's latitude, in radians.
		 * @param lst Local sidereal time, in radians.
		 * @return Rectangular coordinates in ecliptic space.
		 *
		 * @see coordinates::ecliptic
		 */
		template <class T>
		math::vector3<T> to_ecliptic(const math::vector3<T>& v, T ecl, T lat, T lst);

	} // namespace horizontal
	
} // namespace rectangular

namespace spherical {

	/// Spherical local horizontal coordinate system.
	namespace horizontal {

		/**
		 * Rotates spherical coordinates from local horizontal space into equatorial space.
		 *
		 * @param v Spherical coordinates in local horizontal space, in the ISO order of radial distance, altitude (radians), and azimuth (radians).
		 * @param lat Observer's latitude, in radians.
		 * @param lst Local sidereal time, in radians.
		 * @return Spherical coordinates in equatorial space, in the ISO order of radial distance, declination (radians), and right ascension (radians).
		 *
		 * @see coordinates::equatorial
		 */
		template <class T>
		math::vector3<T> to_equatorial(const math::vector3<T>& v, T lat, T lst);
		
		/**
		 * Rotates spherical coordinates from local horizontal space into ecliptic space.
		 *
		 * @param v Spherical coordinates in local horizontal space, in the ISO order of radial distance, altitude (radians), and azimuth (radians).
		 * @param ecl Obliquity of the ecliptic, in radians.
		 * @param lat Observer's latitude, in radians.
		 * @param lst Local sidereal time, in radians.
		 * @return Spherical coordinates in ecliptic space, in the ISO order of radial distance, ecliptic latitude (radians), and ecliptic longitude (radians).
		 *
		 * @see coordinates::ecliptic
		 */
		template <class T>
		math::vector3<T> to_ecliptic(const math::vector3<T>& v, T ecl, T lat, T lst);

	} // namespace horizontal
	
} // namespace spherical

namespace rectangular {
	
	namespace horizontal {

		template <class T>
		math::matrix3<T> to_equatorial(T lat, T lst)
		{
			const T c_lat = std::cos(lat);
			const T s_lat = std::sin(lat);
			const T c_lst = std::cos(lst);
			const T s_lst = std::sin(lst);
			
			return math::matrix3<T>
			{
				c_lst * s_lat, s_lst * s_lat, -c_lat,
				s_lst, c_lst, T(0.0),
				c_lst * c_lat, s_lst * c_lat, s_lat
			};
		}
		
		template <class T>
		math::vector3<T> to_equatorial(const math::vector3<T>& v, T lat, T lst)
		{
			return to_equatorial<T>(lat, lst) * v;
		}
		
		template <class T>
		math::matrix3<T> to_ecliptic(T ecl, T lat, T lst)
		{
			const T c_ecl = std::cos(ecl);
			const T s_ecl = std::sin(ecl);
			const T c_lat = std::cos(lat);
			const T s_lat = std::sin(lat);
			const T c_lst = std::cos(lst);
			const T s_lst = std::sin(lst);
			
			return math::matrix3<T>
			{
				s_lat * c_lst, s_lat * s_lst * c_ecl - c_lat * s_ecl, s_lat * s_lst * -s_ecl - c_lat * c_ecl,
				s_lst, -c_lst * c_ecl, c_lst * s_ecl,
				c_lat * c_lst, c_lat * s_lst * c_ecl + s_lat * s_ecl, c_lat * s_lst * -s_ecl + s_lat * c_ecl
			};
		}
		
		template <class T>
		math::vector3<T> to_ecliptic(const math::vector3<T>& v, T ecl, T lat, T lst)
		{
			return to_ecliptic<T>(ecl, lat, lst) * v;
		}

	} // namespace horizontal
	
} // namespace rectangular

namespace spherical {
	
	namespace horizontal {

		template <class T>
		math::vector3<T> to_equatorial(const math::vector3<T>& v, T lat, T lst)
		{
			return coordinates::rectangular::to_spherical<T>
			(
				coordinates::rectangular::horizontal::to_equatorial<T>
				(
					coordinates::spherical::to_rectangular<T>(v),
					lat,
					lst
				)
			);
		}
		
		template <class T>
		math::vector3<T> to_ecliptic(const math::vector3<T>& v, T ecl, T lat, T lst)
		{
			return coordinates::rectangular::to_spherical<T>
			(
				coordinates::rectangular::horizontal::to_ecliptic<T>
				(
					coordinates::spherical::to_rectangular<T>(v),
					ecl,
					lat,
					lst
				)
			);
		}
	
	} // namespace horizontal

} // namespace spherical

} // namespace coordinates

#endif // ANTKEEPER_COORDINATES_HORIZONTAL_HPP
