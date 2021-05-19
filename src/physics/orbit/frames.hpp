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

#ifndef ANTKEEPER_PHYSICS_ORBIT_FRAMES_HPP
#define ANTKEEPER_PHYSICS_ORBIT_FRAMES_HPP

#include "physics/frame.hpp"

namespace physics {
namespace orbit {

/// Inertial right-handed coordinate system
namespace inertial {

	/**
	 * Constucts a reference frame which transforms coordinates from inertial space into perifocal space.
	 *
	 * @param focus Cartesian coordinates of the focus of the orbit, in the parent space.
	 * @param raan Right ascension of the ascending node (OMEGA), in radians.
	 * @param i Orbital inclination (i), in radians.
	 * @param w Argument of periapsis (omega), in radians.
	 * @return Perifocal frame.
	 */
	template <typename T>
	physics::frame<T> to_perifocal(const math::vector3<T>& focus, T raan, T i, T w)
	{
		const math::quaternion<T> rotation = math::normalize
		(
			math::quaternion<T>::rotate_z(raan) *
				math::quaternion<T>::rotate_x(i) *
				math::quaternion<T>::rotate_z(w)
		);
		
		return physics::frame<T>{focus, rotation}.inverse();
	}
	
	/**
	 * Constructs a reference frame which transforms coordinates from inertial space to body-centered inertial space.
	 *
	 * @param r Cartesian position vector (r) of the center of the body, in inertial space.
	 * @param i Orbital inclination (i), in radians.
	 * @param axial_tilt Angle between the body's rotational axis and its orbital axis, in radians.
	 * @return Body-centered inertial frame.
	 */
	template <typename T>
	physics::frame<T> to_bci(const math::vector3<T>& r, T i, T axial_tilt)
	{
		return physics::frame<T>{r, math::quaternion<T>::rotate_x(-axial_tilt - i)}.inverse();
	}
	
	/**
	 * Constructs a reference frame which transforms coordinates from inertial space to body-centered, body-fixed space.
	 *
	 * @param r Cartesian position vector (r) of the center of the body, in inertial space.
	 * @param i Orbital inclination (i), in radians.
	 * @param axial_tilt Angle between the orbiting body's rotational axis and its orbital axis, in radians.
	 * @param axial_rotation Angle of rotation about the orbiting body's rotational axis, in radians.
	 */
	template <typename T>
	frame<T> to_bcbf(const math::vector3<T>& r, T i, T axial_tilt, T axial_rotation)
	{
		const math::quaternion<T> rotation = math::normalize
		(
			math::quaternion<T>::rotate_x(-axial_tilt - i) *
				math::quaternion<T>::rotate_z(axial_rotation)
		);
		
		return physics::frame<T>{r, rotation}.inverse();
	}

} // namespace inertial

/// Perifocal right-handed coordinate system in which the x-axis points toward the periapsis of the orbit, the y-axis has a true anomaly of 90 degrees past the periapsis, and the z-axis is the angular momentum vector, which is orthogonal to the orbital plane.
namespace perifocal {



} // namespace perifocal

/// Non-inertial body-centered, body-fixed right-handed coordinate system. The x-axis is orthogonal to the intersection of the prime meridian and the equator. The z-axis points toward the positive pole. The y-axis is right-hand orthogonal to the xz-plane.
namespace bcbf {

	/**
	 * Constructs a reference frame which transforms coordinates from BCBF space to topocentric space.
	 *
	 * @param distance Radial distance of the observer from the center of the body.
	 * @param latitude Latitude of the observer, in radians.
	 * @param longitude Longitude of the obserer, in radians.
	 * @return Topocentric frame.
	 */
	template <typename T>
	physics::frame<T> to_topocentric(T distance, T latitude, T longitude)
	{
		const math::vector3<T> translation = {T(0), T(0), distance};
		
		const math::quaternion<T> rotation = math::normalize
		(
			math::quaternion<T>::rotate_z(longitude) *
				math::quaternion<T>::rotate_y(math::half_pi<T> - latitude)
		);
		
		return physics::frame<T>{rotation * translation, rotation}.inverse();
	}

} // namespace bcbf

/// Non-inertial topocentric right-handed coordinate system. Topocentric frames are constructed as south-east-zenith (SEZ) frames; the x-axis points south, the y-axis points east, and the z-axis points toward the zenith (orthogonal to reference spheroid).
namespace topocentric {
	
} // namespace topocentric

} // namespace orbit
} // namespace physics

#endif // ANTKEEPER_PHYSICS_ORBIT_FRAMES_HPP
