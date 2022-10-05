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

#ifndef ANTKEEPER_GAME_SYSTEM_SOLAR_HPP
#define ANTKEEPER_GAME_SYSTEM_SOLAR_HPP

#include "game/system/updatable.hpp"
#include "utility/fundamental-types.hpp"
#include "entity/id.hpp"
#include "game/component/orbit.hpp"
#include "physics/orbit/ephemeris.hpp"
#include <unordered_set>

namespace game {
namespace system {

/**
 * Updates the Cartesian position and velocity of orbiting bodies given their Keplerian orbital elements and the current time.
 */
class orbit:
	public updatable
{
public:
	orbit(entity::registry& registry);
	~orbit();
	
	/**
	 * Scales then adds the timestep `dt` to the current time, then recalculates the positions of orbiting bodies.
	 *
	 * @param t Time, in seconds.
	 * @param dt Delta time, in seconds.
	 */
	virtual void update(double t, double dt);
	
	/**
	 * Sets the current time.
	 *
	 * @param time Time, in days.
	 */
	void set_time(double time);
	
	/**
	 * Sets the factor by which the timestep `dt` will be scaled before being added to the current time.
	 *
	 * @param scale Factor by which to scale the timestep.
	 */
	void set_time_scale(double scale);
	
	/**
	 * Sets the ephemeris used to calculate orbital positions.
	 *
	 * @param ephemeris Ephemeris.
	 */
	void set_ephemeris(const physics::orbit::ephemeris<double>* ephemeris);
	
private:
	void on_orbit_construct(entity::registry& registry, entity::id entity_id, game::component::orbit& component);
	void on_orbit_replace(entity::registry& registry, entity::id entity_id, game::component::orbit& component);
	
	const physics::orbit::ephemeris<double>* ephemeris;
	double time;
	double time_scale;
	std::vector<double3> positions;
	std::unordered_set<int> ephemeris_indices;
};

} // namespace system
} // namespace game

#endif // ANTKEEPER_GAME_SYSTEM_SOLAR_HPP
