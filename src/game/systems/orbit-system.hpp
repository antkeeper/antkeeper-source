// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ORBIT_SYSTEM_HPP
#define ANTKEEPER_GAME_ORBIT_SYSTEM_HPP

#include "game/components/orbit-component.hpp"
#include "game/systems/fixed-update-system.hpp"
#include <engine/physics/orbit/ephemeris.hpp>
#include <engine/math/vector.hpp>
#include <engine/entity/id.hpp>
#include <unordered_set>

using namespace engine;

/// Updates the Cartesian position and velocity of orbiting bodies given their Keplerian orbital elements and the current time.
class orbit_system:
	public fixed_update_system
{
public:
	explicit orbit_system(entity::registry& registry);
	~orbit_system() override;
	void fixed_update(entity::registry& registry, float t, float dt) override;
	
	/// Sets the current time.
	/// @param time Time, in days.
	void set_time(double time);
	
	/// Sets the ephemeris used to calculate orbital positions.
	/// @param ephemeris Ephemeris.
	void set_ephemeris(std::shared_ptr<physics::orbit::ephemeris<double>> ephemeris);
	
private:
	void on_orbit_construct(entity::registry& registry, entity::id entity_id);
	void on_orbit_update(entity::registry& registry, entity::id entity_id);
	
	entity::registry& m_registry;
	std::shared_ptr<physics::orbit::ephemeris<double>> m_ephemeris;
	double m_time{0.0};
	std::vector<math::dvec3> m_positions;
	std::unordered_set<int> m_ephemeris_indices;
};

#endif // ANTKEEPER_GAME_ORBIT_SYSTEM_HPP
