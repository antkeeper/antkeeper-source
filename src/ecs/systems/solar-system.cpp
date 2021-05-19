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

#include "ecs/systems/solar-system.hpp"
#include "ecs/components/celestial-body-component.hpp"
#include "ecs/entity.hpp"

namespace ecs {

static constexpr double seconds_per_day = 24.0 * 60.0 * 60.0;

solar_system::solar_system(ecs::registry& registry):
	entity_system(registry),
	universal_time(0.0),
	days_per_timestep(1.0 / seconds_per_day),
	ke_tolerance(1e-6),
	ke_iterations(10)
{}

void solar_system::update(double t, double dt)
{
	// Add scaled timestep to current time
	set_universal_time(universal_time + dt * days_per_timestep);
	
	// Update orbital state of intrasolar celestial bodies
	registry.view<celestial_body_component>().each(
	[&](ecs::entity entity, auto& body)
	{
		auto elements = body.orbital_elements;
		elements.a += body.orbital_rate.a * universal_time;
		elements.e += body.orbital_rate.e * universal_time;
		elements.w += body.orbital_rate.w * universal_time;
		elements.ta += body.orbital_rate.ta * universal_time;
		elements.i += body.orbital_rate.i * universal_time;
		elements.raan += body.orbital_rate.raan * universal_time;
		
		// Calculate ecliptic orbital position
		//body.orbital_state.r = astro::orbital_elements_to_ecliptic(elements, ke_tolerance, ke_iterations);
	});
}

void solar_system::set_universal_time(double time)
{
	universal_time = time;
}

void solar_system::set_time_scale(double scale)
{
	days_per_timestep = scale / seconds_per_day;
}

} // namespace ecs
