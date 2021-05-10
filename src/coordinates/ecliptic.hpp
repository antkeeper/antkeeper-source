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

#ifndef ANTKEEPER_COORDINATES_ECLIPTIC_HPP
#define ANTKEEPER_COORDINATES_ECLIPTIC_HPP

#include "coordinates/rectangular.hpp"
#include "coordinates/spherical.hpp"
#include "utility/fundamental-types.hpp"
#include <cmath>

namespace coordinates {

namespace rectangular {

	/// Rectangular coordinate system with the plane of the Earth's orbit as the fundamental plane. This is a right-handed coordinate system with the x-axis pointing to the vernal equinox, the y-axis pointing east, and the z-axis is the north orbital pole.
	namespace ecliptic {

		/**
		 * Produces a matrix which rotates rectangular coordinates from ecliptic space into equatorial space.
		 *
		 * @param ecl Obliquity of the ecliptic, in radians.
		 * @return Rotation matrix.
		 *
		 * @see coordinates::equatorial
		 */
		template <class T>
		math::matrix3<T> to_equatorial(T ecl);
		
		/**
		 * Rotates rectangular coordinates from ecliptic space into equatorial space.
		 *
		 * @param v Rectangular coordinates in ecliptic space.
		 * @param ecl Obliquity of the ecliptic, in radians.
		 * @return Rectangular coordinates in equatorial space.
		 *
		 * @note If more than one point is being rotated, it is recommended to use the rotation matrix directly.
		 *
		 * @see coordinates::ecliptic::rectangular::to_equatorial(T)
		 * @see coordinates::equatorial
		 */
		template <class T>
		math::vector3<T> to_equatorial(const math::vector3<T>& v, T ecl);
		
		/**
		 * Produces a matrix which rotates rectangular coordinates from ecliptic space into local horizontal space.
		 *
		 * @param ecl Obliquity of the ecliptic, in radians.
		 * @param lat Observer's latitude, in radians.
		 * @param lst Local sidereal time, in radians.
		 * @return Rotation matrix.
		 *
		 * @see coordinates::horizontal
		 */
		template <class T>
		math::matrix3<T> to_horizontal(T ecl, T lat, T lst);
		
		/**
		 * Rotates rectangular coordinates from ecliptic space into local horizontal space.
		 *
		 * @param v Rectangular coordinates in ecliptic space.
		 * @param ecl Obliquity of the ecliptic, in radians.
		 * @param lat Observer's latitude, in radians.
		 * @param lst Local sidereal time, in radians.
		 * @return Rectangular coordinates in local horizontal space.
		 *
		 * @note If more than one point is being rotated, it is recommended to use the rotation matrix directly.
		 *
		 * @see coordinates::ecliptic::rectangular::to_horizontal(T, T, T)
		 * @see coordinates::horizontal
		 */
		template <class T>
		math::vector3<T> to_horizontal(const math::vector3<T>& v, T ecl, T lat, T lst);

	} // namespace ecliptic

} // namespace rectangular

namespace spherical {

	/// Spherical equatorial coordinate system.
	namespace ecliptic {
		
		/**
		 * Rotates spherical coordinates from ecliptic space into equatorial space.
		 *
		 * @param v Spherical coordinates in ecliptic space, in the ISO order of radial distance, ecliptic latitude (radians), and ecliptic longitude (radians).
		 * @param ecl Obliquity of the ecliptic, in radians.
		 * @return Spherical coordinates in equatorial space, in the ISO order of radial distance, declination (radians), and right ascension (radians).
		 *
		 * @see coordinates::equatorial
		 */
		template <class T>
		math::vector3<T> to_equatorial(const math::vector3<T>& v, T ecl);
		
		/**
		 * Rotates spherical coordinates from ecliptic space into local horizontal space.
		 *
		 * @param v Spherical coordinates in ecliptic space, in the ISO order of radial distance, ecliptic latitude (radians), and ecliptic longitude (radians).
		 * @param ecl Obliquity of the ecliptic, in radians.
		 * @param lat Observer's latitude, in radians.
		 * @param lst Local sidereal time, in radians.
		 * @return Spherical coordinates in local horizontal space, in the ISO order of radial distance, altitude (radians), and azimuth (radians).
		 *
		 * @see coordinates::horizontal
		 */
		template <class T>
		math::vector3<T> to_horizontal(const math::vector3<T>& v, T ecl, T lat, T lst);

	} // namespace ecliptic

} // namespace spherical

namespace rectangular {
	
	namespace ecliptic {

		template <class T>
		math::matrix3<T> to_equatorial(T ecl)
		{
			const T c_ecl = std::cos(ecl);
			const T s_ecl = std::sin(ecl);
			
			return math::matrix3<T>
			{
				T(1.0), T(0.0), T(0.0),
				T(0.0), c_ecl, s_ecl,
				T(0.0), -s_ecl, c_ecl
			};
		}
		
		template <class T>
		math::vector3<T> to_equatorial(const math::vector3<T>& v, T ecl)
		{
			return to_equatorial<T>(ecl) * v;
		}
		
		template <class T>
		math::matrix3<T> to_horizontal(T ecl, T lat, T lst)
		{
			const T c_ecl = std::cos(ecl);
			const T s_ecl = std::sin(ecl);
			const T c_lat = std::cos(lat);
			const T s_lat = std::sin(lat);
			const T c_lst = std::cos(lst);
			const T s_lst = std::sin(lst);
			
			return math::matrix3<T>
			{
				s_lat * c_lst, s_lst, c_lat * c_lst,
				s_lat * s_lst * c_ecl - c_lat * s_ecl, -c_lst * c_ecl, c_lat * s_lst * c_ecl + s_lat * s_ecl,
				s_lat * s_lst * -s_ecl - c_lat * c_ecl,  c_lst * s_ecl, c_lat * s_lst * -s_ecl + s_lat * c_ecl
			};
		}

		template <class T>
		math::vector3<T> to_horizontal(const math::vector3<T>& v, T ecl, T lat, T lst)
		{
			return to_horizontal<T>(ecl, lat, lst) * v;
		}
	
	} // namespace ecliptic

} // namespace rectangular

namespace spherical {
	
	namespace ecliptic {

		template <class T>
		math::vector3<T> to_equatorial(const math::vector3<T>& v, T ecl)
		{
			return coordinates::rectangular::to_spherical<T>
			(
				coordinates::rectangular::ecliptic::to_equatorial<T>
				(
					coordinates::spherical::to_rectangular<T>(v),
					ecl
				)
			);
		}
		
		template <class T>
		math::vector3<T> to_horizontal(const math::vector3<T>& v, T ecl, T lat, T lst)
		{
			return coordinates::rectangular::to_spherical<T>
			(
				coordinates::rectangular::ecliptic::to_horizontal<T>
				(
					coordinates::spherical::to_rectangular<T>(v),
					ecl,
					lat,
					lst
				)
			);
		}
	
	} // namespace ecliptic

} // namespace spherical

} // namespace coordinates

#endif // ANTKEEPER_COORDINATES_ECLIPTIC_HPP
