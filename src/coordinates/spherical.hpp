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

#ifndef ANTKEEPER_COORDINATES_SPHERICAL_HPP
#define ANTKEEPER_COORDINATES_SPHERICAL_HPP

#include "utility/fundamental-types.hpp"
#include <cmath>

namespace coordinates {

/// Spherical coordinate systems.
namespace spherical {

/**
 * Converts spherical coordinates to rectangular coordinates.
 *
 * @param v Spherical coordinates, in the ISO order of radial distance, polar angle (radians), and azimuthal angle (radians).
 * @return Rectangular coordinates.
 *
 * @see coordinates::rectangular
 */
template <class T>
math::vector3<T> to_rectangular(const math::vector3<T>& v);

template <class T>
math::vector3<T> to_rectangular(const math::vector3<T>& v)
{
	const T x = v[0] * std::cos(v[1]);
	
	return math::vector3<T>
	{
		x * std::cos(v[2]),
		x * std::sin(v[2]),
		v[0] * std::sin(v[1]),
	};
}

} // namespace spherical
} // namespace coordinates

#endif // ANTKEEPER_COORDINATES_SPHERICAL_HPP
