// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ORBIT_SYSTEM_HPP
#define ANTKEEPER_GAME_ORBIT_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include <engine/math/vector.hpp>
#include <engine/entity/id.hpp>
#include "game/components/orbit-component.hpp"
#include <engine/physics/orbit/ephemeris.hpp>
#include <unordered_set>

/**
 * Updates the Cartesian position and velocity of orbiting bodies given their Keplerian orbital elements and the current time.
 */
class orbit_system:
	public updatable_system
{
public:
	explicit orbit_system(entity::registry& registry);
	~orbit_system() override;
	
	/**
	 * Scales then adds the timestep `dt` to the current time, then recalculates the positions of orbiting bodies.
	 *
	 * @param t Time, in seconds.
	 * @param dt Delta time, in seconds.
	 */
	virtual void update(float t, float dt);
	
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
	void set_ephemeris(std::shared_ptr<physics::orbit::ephemeris<double>> ephemeris);
	
private:
	void on_orbit_construct(entity::registry& registry, entity::id entity_id);
	void on_orbit_update(entity::registry& registry, entity::id entity_id);
	
	std::shared_ptr<physics::orbit::ephemeris<double>> m_ephemeris;
	double m_time;
	double m_time_scale;
	std::vector<math::dvec3> m_positions;
	std::unordered_set<int> m_ephemeris_indices;
};

#endif // ANTKEEPER_GAME_ORBIT_SYSTEM_HPP
