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

#ifndef ANTKEEPER_PHYSICS_ATMOSPHERE_HPP
#define ANTKEEPER_PHYSICS_ATMOSPHERE_HPP

#include "physics/constants.hpp"
#include <cmath>

namespace physics {

/// Atmosphere-related functions.
namespace atmosphere {

/**
 * Calculates the density of exponentially-distributed atmospheric particles at a given altitude.
 *
 * @param d0 Density at sea level.
 * @param z Height above sea level.
 * @param sh Scale height of the particle type.
 * @return Particle density at altitude.
 *
 * @see https://en.wikipedia.org/wiki/Scale_height
 */
T density(T d0, T z, T sh)
{
	return d0 * std::exp(-z / sh);
}

} // namespace atmosphere

} // namespace physics

#endif // ANTKEEPER_PHYSICS_ATMOSPHERE_HPP
