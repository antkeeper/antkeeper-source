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

#include "entity/systems/orbit.hpp"
#include "physics/orbit/orbit.hpp"
#include <iostream>

namespace entity {
namespace system {

orbit::orbit(entity::registry& registry):
	updatable(registry),
	ephemeris(nullptr),
	time(0.0),
	time_scale(1.0)
{
	registry.on_construct<entity::component::orbit>().connect<&orbit::on_orbit_construct>(this);
	registry.on_replace<entity::component::orbit>().connect<&orbit::on_orbit_replace>(this);
}

orbit::~orbit()
{
	registry.on_construct<entity::component::orbit>().disconnect<&orbit::on_orbit_construct>(this);
	registry.on_replace<entity::component::orbit>().disconnect<&orbit::on_orbit_replace>(this);
}

void orbit::update(double t, double dt)
{
	// Add scaled timestep to current time
	set_time(time + dt * time_scale);
	
	if (!ephemeris)
		return;
	
	// Calculate positions of ephemeris items, in meters
	for (int i: ephemeris_indices)
		positions[i] = (*ephemeris)[i].position(time) * 1000.0;
	
	// Propagate orbits
	registry.view<component::orbit>().each(
	[&](entity::id entity_eid, auto& orbit)
	{
		orbit.position = positions[orbit.ephemeris_index] * orbit.scale;
		
		entity::id parent_id = orbit.parent;
		while (parent_id != entt::null)
		{
			const component::orbit& parent_orbit = registry.get<component::orbit>(parent_id);
			orbit.position += positions[parent_orbit.ephemeris_index] * parent_orbit.scale;
			parent_id = parent_orbit.parent;
		}
	});
}

void orbit::set_ephemeris(const physics::orbit::ephemeris<double>* ephemeris)
{
	this->ephemeris = ephemeris;
	positions.resize((ephemeris) ? ephemeris->size() : 0);
}

void orbit::set_time(double time)
{
	this->time = time;
}

void orbit::set_time_scale(double scale)
{
	time_scale = scale;
}

void orbit::on_orbit_construct(entity::registry& registry, entity::id entity_id, entity::component::orbit& component)
{
	ephemeris_indices.insert(component.ephemeris_index);
}

void orbit::on_orbit_replace(entity::registry& registry, entity::id entity_id, entity::component::orbit& component)
{
	ephemeris_indices.insert(component.ephemeris_index);
}

} // namespace system
} // namespace entity
