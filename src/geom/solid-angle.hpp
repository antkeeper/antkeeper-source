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

#ifndef ANTKEEPER_GEOM_SOLID_ANGLE_HPP
#define ANTKEEPER_GEOM_SOLID_ANGLE_HPP

#include "math/numbers.hpp"
#include <cmath>

namespace geom {

/// Solid angle functions.
namespace solid_angle {

/**
 * Calculates the solid angle of a cone.
 * 
 * @param theta Apex angle of the cone, in radians.
 *
 * @return Solid angle of the cone, in steradians.
 */
template <class T>
T cone(T theta)
{
	return math::two_pi<T> * (T(1) - std::cos(theta));
}

} // namespace solid_angle
} // namespace geom

#endif // ANTKEEPER_GEOM_SOLID_ANGLE_HPP
