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
#include "entity/components/orbit.hpp"
#include "entity/id.hpp"
#include "physics/orbit/orbit.hpp"

namespace entity {
namespace system {

orbit::orbit(entity::registry& registry):
	updatable(registry),
	universal_time(0.0),
	time_scale(1.0),
	ke_iterations(10),
	ke_tolerance(1e-6)
{}

void orbit::update(double t, double dt)
{
	// Add scaled timestep to current time
	set_universal_time(universal_time + dt * time_scale);
	
	// Update the orbital state of orbiting bodies
	registry.view<component::orbit>().each(
	[&](entity::id entity_id, auto& orbit)
	{
		// Calculate semi-minor axis (b)
		const double b = physics::orbit::derive_semiminor_axis(orbit.elements.a, orbit.elements.e);
		
		// Solve Kepler's equation for eccentric anomaly (E)
		const double ea = physics::orbit::kepler_ea(orbit.elements.e, orbit.elements.ta, ke_iterations, ke_tolerance);
		
		// Calculate radial distance and true anomaly (nu)
		const double xv = orbit.elements.a * (std::cos(ea) - orbit.elements.e);
		const double yv = b * std::sin(ea);
		const double distance = std::sqrt(xv * xv + yv * yv);
		const double ta = std::atan2(yv, xv);
		
		// Calculate Cartesian position (r) in perifocal space
		const math::vector3<double> r_perifocal = math::quaternion<double>::rotate_z(ta) * math::vector3<double>{distance, 0, 0};
		
		/// @TODO Calculate Cartesian velocity (v) in perifocal space
		//const math::vector3<double> v_perifocal = ...
		
		// Construct perifocal to inertial reference frame
		const physics::frame<double> perifocal_to_inertial = physics::orbit::inertial::to_perifocal
		(
			{0, 0, 0},
			orbit.elements.raan,
			orbit.elements.i,
			orbit.elements.w
		).inverse();
		
		// Transform orbital state vectors from perifocal space to the parent inertial space
		const math::vector3<double> r_inertial = perifocal_to_inertial.transform(r_perifocal);
		//const math::vector3<double> v_inertial = perifocal_frame.transform(v_perifocal);
		
		// Update orbital state of component
		orbit.state.r = r_inertial;
		//orbit.state.v = v_inertial;
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

} // namespace system
} // namespace entity
