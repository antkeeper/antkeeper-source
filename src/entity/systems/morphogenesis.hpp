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

#ifndef ANTKEEPER_ENTITY_SYSTEM_MORPHOGENESIS_HPP
#define ANTKEEPER_ENTITY_SYSTEM_MORPHOGENESIS_HPP

#include "entity/systems/updatable.hpp"

namespace entity {
namespace system {

/**
 * Constructs the forms of organisms from their genetic codes.
 *
 * Algorithm:
 *
 * 1. Consider inital egg cell as a sphere at the origin.
 * 2. Use the marching cubes algorithm to generate a mesh from the isosurface formed by the sum of all cells.
 * 3. Generate evenly-distributed points across surface area of the mesh (Poisson sample?).
 * 4. For each point, evaluate morphogenic genes to determine type, direction, and color of the next cell.
 * 5. Create new cells given the output of step 4.
 * 6. Go to step 2 and repeat.
 */
class morphogenesis:
	public updatable
{
public:
	morphogenesis(entity::registry& registry);
	
	/**
	 * Scales then adds the timestep `dt` to the current time, then recalculates the positions of morphogenesising bodies.
	 *
	 * @param t Time, in seconds.
	 * @param dt Delta time, in seconds.
	 */
	virtual void update(double t, double dt);
	
private:
};

} // namespace system
} // namespace entity

#endif // ANTKEEPER_ENTITY_SYSTEM_MORPHOGENESIS_HPP
