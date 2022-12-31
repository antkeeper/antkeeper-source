/*
 * Copyright (C) 2023  Christopher J. Howard
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

#ifndef ANTKEEPER_GEOM_CARTESIAN_HPP
#define ANTKEEPER_GEOM_CARTESIAN_HPP

#include "utility/fundamental-types.hpp"
#include <cmath>

namespace geom {

/// Functions which operate on Cartesian (rectangular) coordinates.
namespace cartesian {

/**
 * Converts Cartesian (rectangular) coordinates to spherical coordinates.
 *
 * @param v Cartesian coordinates.
 * @return Spherical coordinates, in the ISO order of radial distance, polar angle (radians), and azimuthal angle (radians).
 *
 * @see geom::coordinates::spherical
 */
template <class T>
math::vector3<T> to_spherical(const math::vector3<T>& v);

template <class T>
math::vector3<T> to_spherical(const math::vector3<T>& v)
{
	const T xx_yy = v.x() * v.x() + v.y() * v.y();
	
	return math::vector3<T>
	{
		std::sqrt(xx_yy + v.z() * v.z()),
		std::atan2(v.z(), std::sqrt(xx_yy)),
		std::atan2(v.y(), v.x())
	};
}

} // namespace cartesian
} // namespace geom

#endif // ANTKEEPER_GEOM_CARTESIAN_HPP
