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

#include "game/systems/orbit-system.hpp"
#include <engine/physics/orbit/orbit.hpp>


orbit_system::orbit_system(entity::registry& registry):
	updatable_system(registry),
	ephemeris(nullptr),
	time(0.0),
	time_scale(1.0)
{
	registry.on_construct<::orbit_component>().connect<&orbit_system::on_orbit_construct>(this);
	registry.on_update<::orbit_component>().connect<&orbit_system::on_orbit_update>(this);
}

orbit_system::~orbit_system()
{
	registry.on_construct<::orbit_component>().disconnect<&orbit_system::on_orbit_construct>(this);
	registry.on_update<::orbit_component>().disconnect<&orbit_system::on_orbit_update>(this);
}

void orbit_system::update(float t, float dt)
{
	// Add scaled timestep to current time
	set_time(time + dt * time_scale);
	
	if (!ephemeris)
		return;
	
	// Calculate positions of ephemeris items, in meters
	for (int i: ephemeris_indices)
		positions[i] = ephemeris->trajectories[i].position(time) * 1000.0;
	
	// Propagate orbits
	registry.view<orbit_component>().each(
	[&](entity::id entity_eid, auto& orbit)
	{
		orbit.position = positions[orbit.ephemeris_index] * orbit.scale;
		
		entity::id parent_id = orbit.parent;
		while (parent_id != entt::null)
		{
			const orbit_component& parent_orbit = registry.get<orbit_component>(parent_id);
			orbit.position += positions[parent_orbit.ephemeris_index] * parent_orbit.scale;
			parent_id = parent_orbit.parent;
		}
	});
}

void orbit_system::set_ephemeris(std::shared_ptr<physics::orbit::ephemeris<double>> ephemeris)
{
	this->ephemeris = ephemeris;
	positions.resize((ephemeris) ? ephemeris->trajectories.size() : 0);
}

void orbit_system::set_time(double time)
{
	this->time = time;
}

void orbit_system::set_time_scale(double scale)
{
	time_scale = scale;
}

void orbit_system::on_orbit_construct(entity::registry& registry, entity::id entity_id)
{
	const ::orbit_component& component = registry.get<::orbit_component>(entity_id);
	ephemeris_indices.insert(component.ephemeris_index);
}

void orbit_system::on_orbit_update(entity::registry& registry, entity::id entity_id)
{
	const ::orbit_component& component = registry.get<::orbit_component>(entity_id);
	ephemeris_indices.insert(component.ephemeris_index);
}

