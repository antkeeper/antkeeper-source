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

#ifndef ANTKEEPER_COORDINATES_EQUATORIAL_HPP
#define ANTKEEPER_COORDINATES_EQUATORIAL_HPP

#include "coordinates/rectangular.hpp"
#include "coordinates/spherical.hpp"
#include "utility/fundamental-types.hpp"
#include <cmath>

namespace coordinates {

namespace rectangular {

	/// Rectangular coordinate system with the Earth's equator as the fundamental plane. This is a right-handed coordinate system with the x-axis pointing to the vernal equinox, the y-axis pointing east, and the z-axis is the north celestial pole.
	namespace equatorial {

		/**
		 * Produces a matrix which rotates rectangular coordinates from equatorial space into ecliptic space.
		 *
		 * @param ecl Obliquity of the ecliptic, in radians.
		 * @return Rotation matrix.
		 *
		 * @see coordinates::rectangular::ecliptic
		 */
		template <class T>
		math::matrix3<T> to_ecliptic(T ecl);
		
		/**
		 * Rotates rectangular coordinates from equatorial space into ecliptic space.
		 *
		 * @param v Rectangular coordinates in equatorial space.
		 * @param ecl Obliquity of the ecliptic, in radians.
		 * @return Rectangular coordinates in ecliptic space.
		 *
		 * @see coordinates::rectangular::ecliptic
		 */
		template <class T>
		math::vector3<T> to_ecliptic(const math::vector3<T>& v, T ecl);
		
		/**
		 * Produces a matrix which rotates rectangular coordinates from equatorial space into local horizontal space.
		 *
		 * @param lat Observer's latitude, in radians.
		 * @param lst Local sidereal time, in radians.
		 * @return Rotation matrix.
		 *
		 * @see coordinates::rectangular::horizontal
		 */
		template <class T>
		math::matrix3<T> to_horizontal(T lat, T lst);
		
		/**
		 * Rotates rectangular coordinates from equatorial space into local horizontal space.
		 *
		 * @param v Rectangular coordinates in equatorial space.
		 * @param lat Observer's latitude, in radians.
		 * @param lst Local sidereal time, in radians.
		 * @return Rectangular coordinates in local horizontal space.
		 *
		 * @see coordinates::rectangular::horizontal
		 */
		template <class T>
		math::vector3<T> to_horizontal(const math::vector3<T>& v, T lat, T lst);

	} // namespace equatorial

} // namespace rectangular

namespace spherical {

	/// Spherical equatorial coordinate system.
	namespace equatorial {

		/**
		 * Rotates spherical coordinates from equatorial space into ecliptic space.
		 *
		 * @param v Spherical coordinates in equatorial space, in the ISO order of radial distance, declination (radians), and right ascension (radians).
		 * @param ecl Obliquity of the ecliptic, in radians.
		 * @return Spherical coordinates in ecliptic space, in the ISO order of radial distance, ecliptic latitude (radians), and ecliptic longitude (radians).
		 *
		 * @see coordinates::spherical::ecliptic
		 */
		template <class T>
		math::vector3<T> to_ecliptic(const math::vector3<T>& v, T ecl);
		
		/**
		 * Rotates spherical coordinates from equatorial space into local horizontal space.
		 *
		 * @param v Spherical coordinates in equatorial space, in the ISO order of radial distance, declination (radians), and right ascension (radians).
		 * @param lat Observer's latitude, in radians.
		 * @param lst Local sidereal time, in radians.
		 * @return Spherical coordinates in local horizontal space, in the ISO order of radial distance, altitude (radians), and azimuth (radians).
		 *
		 * @see coordinates::spherical::horizontal
		 */
		template <class T>
		math::vector3<T> to_horizontal(const math::vector3<T>& v, T lat, T lst);

	} // namespace equatorial

} // namespace spherical

namespace rectangular {
	
	namespace equatorial {

		template <class T>
		math::matrix3<T> to_ecliptic(T ecl)
		{
			const T c_ecl = std::cos(ecl);
			const T s_ecl = std::sin(ecl);
			
			return math::matrix3<T>
			{
				T(1.0), T(0.0), T(0.0),
				T(0.0), c_ecl, -s_ecl,
				T(0.0), s_ecl, c_ecl
			};
		}
		
		template <class T>
		math::vector3<T> to_ecliptic(const math::vector3<T>& v, T ecl)
		{
			return to_ecliptic(ecl) * v;
		}
		
		template <class T>
		math::matrix3<T> to_horizontal(T lat, T lst)
		{
			const T c_lat = std::cos(lat);
			const T s_lat = std::sin(lat);
			const T c_lst = std::cos(lst);
			const T s_lst = std::sin(lst);
			
			return math::matrix3<T>
			{
				s_lat * c_lst, s_lst, c_lat * c_lst,
				s_lat * s_lst, -c_lst, c_lat * s_lst,
				-c_lat, T(0.0), s_lat
			};
		}
		
		template <class T>
		math::vector3<T> to_horizontal(const math::vector3<T>& v, T lat, T lst)
		{
			return to_horizontal(lat, lst) * v;
		}
	
	} // namespace equatorial

} // namespace rectangular

namespace spherical {
	
	namespace equatorial {

		template <class T>
		math::vector3<T> to_ecliptic(const math::vector3<T>& v, T ecl)
		{
			return coordinates::rectangular::to_spherical<T>
			(
				coordinates::rectangular::equatorial::to_ecliptic<T>
				(
					coordinates::spherical::to_rectangular<T>(v),
					ecl
				)
			);
		}
		
		template <class T>
		math::vector3<T> to_horizontal(const math::vector3<T>& v, T lat, T lst)
		{
			return coordinates::rectangular::to_spherical<T>
			(
				coordinates::rectangular::equatorial::to_horizontal<T>
				(
					coordinates::spherical::to_rectangular<T>(v),
					lat,
					lst
				)
			);
		}
	
	} // namepace equatorial

} // namespace spherical

} // namespace coordinates

#endif // ANTKEEPER_COORDINATES_EQUATORIAL_HPP
