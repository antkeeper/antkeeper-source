// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <entt/entt.hpp>
#include "game/systems/orbit-system.hpp"
#include "game/utility/time.hpp"
import engine.physics.orbit;
import engine.physics.time;

orbit_system::orbit_system(entity::registry& registry):
	m_registry(registry)
{
	m_registry.on_construct<::orbit_component>().connect<&orbit_system::on_orbit_construct>(this);
	m_registry.on_update<::orbit_component>().connect<&orbit_system::on_orbit_update>(this);
}

orbit_system::~orbit_system()
{
	m_registry.on_construct<::orbit_component>().disconnect<&orbit_system::on_orbit_construct>(this);
	m_registry.on_update<::orbit_component>().disconnect<&orbit_system::on_orbit_update>(this);
}

void orbit_system::fixed_update(entity::registry& registry, float, float dt)
{
	// Scale timestep
	const auto time_scale = get_time_scale(registry);
	const auto astronomical_time_scale = time_scale / physics::time::seconds_per_day<double>;

	// Add scaled timestep to current time
	set_time(m_time + dt * astronomical_time_scale);
	
	if (!m_ephemeris)
	{
		return;
	}
	
	// Calculate positions of ephemeris items, in meters
	for (auto i: m_ephemeris_indices)
	{
		m_positions[i] = m_ephemeris->trajectories[i].position(m_time) * 1000.0;
	}
	
	// Propagate orbits
	registry.view<orbit_component>().each
	(
		[&](entity::id, auto& orbit)
		{
			orbit.position = m_positions[orbit.ephemeris_index] * orbit.scale;
			
			entity::id parent_id = orbit.parent;
			while (parent_id != entt::null)
			{
				const orbit_component& parent_orbit = registry.get<orbit_component>(parent_id);
				orbit.position += m_positions[parent_orbit.ephemeris_index] * parent_orbit.scale;
				parent_id = parent_orbit.parent;
			}
		}
	);
}

void orbit_system::set_ephemeris(std::shared_ptr<physics::orbit::ephemeris<double>> ephemeris)
{
	m_ephemeris = ephemeris;
	m_positions.resize(m_ephemeris ? m_ephemeris->trajectories.size() : 0);
}

void orbit_system::set_time(double time)
{
	m_time = time;
}

void orbit_system::on_orbit_construct(entity::registry& registry, entity::id entity_id)
{
	const ::orbit_component& component = registry.get<::orbit_component>(entity_id);
	m_ephemeris_indices.insert(component.ephemeris_index);
}

void orbit_system::on_orbit_update(entity::registry& registry, entity::id entity_id)
{
	const ::orbit_component& component = registry.get<::orbit_component>(entity_id);
	m_ephemeris_indices.insert(component.ephemeris_index);
}
