// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/orbit-system.hpp"
#include <engine/physics/orbit/orbit.hpp>

orbit_system::orbit_system(entity::registry& registry):
	updatable_system(registry),
	m_ephemeris(nullptr),
	m_time(0.0),
	m_time_scale(1.0)
{
	m_registry.on_construct<::orbit_component>().connect<&orbit_system::on_orbit_construct>(this);
	m_registry.on_update<::orbit_component>().connect<&orbit_system::on_orbit_update>(this);
}

orbit_system::~orbit_system()
{
	m_registry.on_construct<::orbit_component>().disconnect<&orbit_system::on_orbit_construct>(this);
	m_registry.on_update<::orbit_component>().disconnect<&orbit_system::on_orbit_update>(this);
}

void orbit_system::update([[maybe_unused]] float t, float dt)
{
	// Add scaled timestep to current time
	set_time(m_time + dt * m_time_scale);
	
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
	m_registry.view<orbit_component>().each
	(
		[&]([[maybe_unused]] entity::id entity_eid, auto& orbit)
		{
			orbit.position = m_positions[orbit.ephemeris_index] * orbit.scale;
			
			entity::id parent_id = orbit.parent;
			while (parent_id != entt::null)
			{
				const orbit_component& parent_orbit = m_registry.get<orbit_component>(parent_id);
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

void orbit_system::set_time_scale(double scale)
{
	m_time_scale = scale;
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
