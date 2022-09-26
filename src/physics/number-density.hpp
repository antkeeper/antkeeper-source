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

#ifndef ANTKEEPER_PHYSICS_NUMBER_DENSITY_HPP
#define ANTKEEPER_PHYSICS_NUMBER_DENSITY_HPP

#include "physics/constants.hpp"

namespace physics {

/**
 * Calculates the number density of a substance.
 *
 * @param c Molar concentration, in mol/m-3.
 * @return Number density, in m-3.
 *
 * @see https://en.wikipedia.org/wiki/Number_density
 */
template <class T>
T number_density(T c)
{
	return physics::constants::avogadro<T> * c;
}

} // namespace physics

#endif // ANTKEEPER_PHYSICS_NUMBER_DENSITY_HPP
