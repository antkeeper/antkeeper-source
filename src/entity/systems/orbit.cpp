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
	universal_time(0.0),
	time_scale(1.0),
	ke_iterations(10),
	ke_tolerance(1e-6)
{
	registry.on_construct<entity::component::orbit>().connect<&orbit::on_orbit_construct>(this);
	registry.on_replace<entity::component::orbit>().connect<&orbit::on_orbit_replace>(this);
}

void orbit::update(double t, double dt)
{
	// Add scaled timestep to current time
	set_universal_time(universal_time + dt * time_scale);
	
	// Propagate orbits
	registry.view<component::orbit>().each(
	[&](entity::id entity_id, auto& orbit)
	{
		// Determine mean anomaly at current time
		const double ma = orbit.elements.ma + orbit.mean_motion * this->universal_time;
		
		// Solve Kepler's equation for eccentric anomaly (E)
		const double ea = physics::orbit::anomaly::mean_to_eccentric(orbit.elements.ec, ma, ke_iterations, ke_tolerance);
		
		// Calculate Cartesian orbital position in the PQW frame
		math::vector3<double> pqw_position = physics::orbit::frame::pqw::cartesian(orbit.elements.ec, orbit.elements.a, ea, orbit.semiminor_axis);
		
		// Transform orbital position from PQW frame to BCI frame
		orbit.bci_position = orbit.pqw_to_bci.transform(pqw_position);
	});
	
	// Update orbital positions in the ICRF frame
	registry.view<component::orbit>().each(
	[&](entity::id entity_id, auto& orbit)
	{
		orbit.icrf_position = orbit.bci_position;
		
		entity::id parent = orbit.parent;
		while (parent != entt::null)
		{
			const component::orbit& parent_orbit = registry.get<component::orbit>(parent);
			orbit.icrf_position += parent_orbit.bci_position;
			parent = parent_orbit.parent;
		}
	});
}

void orbit::set_universal_time(double time)
{
	universal_time = time;
}

void orbit::set_time_scale(double scale)
{
	time_scale = scale;
}

void orbit::on_orbit_construct(entity::registry& registry, entity::id entity_id, entity::component::orbit& component)
{
	component.semiminor_axis = physics::orbit::semiminor_axis(component.elements.a, component.elements.ec);
	component.pqw_to_bci = physics::orbit::frame::pqw::to_bci
	(
		component.elements.om,
		component.elements.in,
		component.elements.w
	);
}

void orbit::on_orbit_replace(entity::registry& registry, entity::id entity_id, entity::component::orbit& component)
{
	component.semiminor_axis = physics::orbit::semiminor_axis(component.elements.a, component.elements.ec);
	component.pqw_to_bci = physics::orbit::frame::pqw::to_bci
	(
		component.elements.om,
		component.elements.in,
		component.elements.w
	);
}

} // namespace system
} // namespace entity
