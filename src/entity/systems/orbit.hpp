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

#ifndef ANTKEEPER_ENTITY_SYSTEM_SOLAR_HPP
#define ANTKEEPER_ENTITY_SYSTEM_SOLAR_HPP

#include "entity/systems/updatable.hpp"
#include "utility/fundamental-types.hpp"
#include "entity/id.hpp"
#include "entity/components/orbit.hpp"

namespace entity {
namespace system {

/**
 * Updates the Cartesian position and velocity of orbiting bodies given their Keplerian orbital elements and the current time.
 */
class orbit:
	public updatable
{
public:
	orbit(entity::registry& registry);
	
	/**
	 * Scales then adds the timestep `dt` to the current time, then recalculates the positions of orbiting bodies.
	 *
	 * @param t Time, in seconds.
	 * @param dt Delta time, in seconds.
	 */
	virtual void update(double t, double dt);
	
	/**
	 * Sets the current universal time.
	 *
	 * @param time Universal time, in days.
	 */
	void set_universal_time(double time);
	
	/**
	 * Sets the factor by which the timestep `dt` will be scaled before being added to the current universal time.
	 *
	 * @param scale Factor by which to scale the timestep.
	 */
	void set_time_scale(double scale);
	
private:
	void on_orbit_construct(entity::registry& registry, entity::id entity_id, entity::component::orbit& component);
	void on_orbit_replace(entity::registry& registry, entity::id entity_id, entity::component::orbit& component);
	
	double universal_time;
	double time_scale;
	std::size_t ke_iterations;
	double ke_tolerance;
};

} // namespace system
} // namespace entity

#endif // ANTKEEPER_ENTITY_SYSTEM_SOLAR_HPP
