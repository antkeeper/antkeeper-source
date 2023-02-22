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

#ifndef ANTKEEPER_GEOM_SPHERICAL_HPP
#define ANTKEEPER_GEOM_SPHERICAL_HPP

#include <engine/utility/fundamental-types.hpp>
#include <cmath>

namespace geom {

/// Functions which operate on spherical coordinates.
namespace spherical {

/**
 * Converts spherical coordinates to Cartesian (rectangular) coordinates.
 *
 * @param v Spherical coordinates, in the ISO order of radial distance, polar angle (radians), and azimuthal angle (radians).
 * @return Cartesian coordinates.
 *
 * @see geom::coordinates::cartesian
 */
template <class T>
math::vector3<T> to_cartesian(const math::vector3<T>& v);

template <class T>
math::vector3<T> to_cartesian(const math::vector3<T>& v)
{
	const T x = v[0] * std::cos(v[1]);
	
	return math::vector3<T>
	{
		x * std::cos(v[2]),
		x * std::sin(v[2]),
		v[0] * std::sin(v[1])
	};
}

} // namespace spherical
} // namespace geom

#endif // ANTKEEPER_GEOM_SPHERICAL_HPP
