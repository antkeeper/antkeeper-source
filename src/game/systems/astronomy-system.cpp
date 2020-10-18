/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "game/systems/astronomy-system.hpp"
#include "game/astronomy/celestial-coordinates.hpp"
#include "game/astronomy/celestial-mechanics.hpp"
#include "game/astronomy/celestial-time.hpp"
#include "game/astronomy/astronomical-constants.hpp"
#include "game/components/orbit-component.hpp"
#include "game/components/transform-component.hpp"

using namespace ecs;

static constexpr double seconds_per_day = 24.0 * 60.0 * 60.0;

astronomy_system::astronomy_system(entt::registry& registry):
	entity_system(registry),
	universal_time(0.0),
	days_per_timestep(1.0 / seconds_per_day),
	observer_location{0.0, 0.0, 0.0},
	lst(0.0),
	obliquity(0.0),
	ke_tolerance(1e-6),
	ke_iterations(10)
{}

void astronomy_system::update(double t, double dt)
{
	const double dt_days = dt * days_per_timestep;
	
	// Add scaled timestep to current time
	set_universal_time(universal_time + dt_days);
	
	// Update horizontal (topocentric) positions of orbiting bodies
	registry.view<orbit_component, transform_component>().each(
	[&](auto entity, auto& orbit, auto& transform)
	{
		ast::orbital_elements orbital_elements;
		orbital_elements.a = orbit.a + orbit.d_a * universal_time;
		orbital_elements.ec = orbit.ec + orbit.d_ec * universal_time;
		orbital_elements.w = orbit.w + orbit.d_w * universal_time;
		orbital_elements.ma = math::wrap_radians(orbit.ma + orbit.d_ma * universal_time);
		orbital_elements.i = orbit.i + orbit.d_i * universal_time;
		orbital_elements.om = math::wrap_radians(orbit.om + orbit.d_om * universal_time);
		
		// Calculate ecliptic orbital position
		double3 ecliptic = ast::orbital_elements_to_ecliptic(orbital_elements, ke_tolerance, ke_iterations);
			
		// Transform orbital position from ecliptic space to horizontal space
		double3 horizontal = ecliptic_to_horizontal * ecliptic;
		
		// Subtract observer's radial distance (planet radius + observer's altitude)
		horizontal.z -= observer_location[0];
		
		// Calculate azimuth and elevation
		double3 spherical = ast::rectangular_to_spherical(horizontal);
		double2 az_el = {spherical.z - math::pi<double>, spherical.y};
		
		// Transform into local right-handed coordinates
		double3 translation = ast::horizontal_to_right_handed * horizontal;
		double3x3 rotation = ast::horizontal_to_right_handed * ecliptic_to_horizontal;
		
		transform.local.translation = math::type_cast<float>(translation);
		transform.local.rotation = math::type_cast<float>(math::quaternion_cast(rotation));
	});
}

void astronomy_system::set_universal_time(double time)
{
	universal_time = time;
	update_axial_rotation();
}

void astronomy_system::set_time_scale(double scale)
{
	days_per_timestep = scale / seconds_per_day;
}

void astronomy_system::set_observer_location(const double3& location)
{
	observer_location = location;
	update_sidereal_time();
}

void astronomy_system::set_obliquity(double angle)
{
	obliquity = angle;
	update_ecliptic_to_horizontal();
}

void astronomy_system::set_axial_rotation_speed(double speed)
{
	axial_rotation_speed = speed;
}

void astronomy_system::set_axial_rotation_at_epoch(double angle)
{
	axial_rotation_at_epoch = angle;
	update_axial_rotation();
}

void astronomy_system::update_axial_rotation()
{
	axial_rotation = math::wrap_radians<double>(axial_rotation_at_epoch + universal_time * axial_rotation_speed);
	update_sidereal_time();
}

void astronomy_system::update_sidereal_time()
{
	lst = math::wrap_radians<double>(axial_rotation + observer_location[2]);
	update_ecliptic_to_horizontal();
}

void astronomy_system::update_ecliptic_to_horizontal()
{
	ecliptic_to_horizontal = ast::ecliptic_to_horizontal(obliquity, observer_location[1], lst);
}
