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

#include "ecs/systems/astronomy-system.hpp"
#include "astro/coordinates.hpp"
#include "astro/apparent-size.hpp"
#include "ecs/components/celestial-body-component.hpp"
#include "ecs/components/transform-component.hpp"
#include "renderer/passes/sky-pass.hpp"

namespace ecs {

static constexpr double seconds_per_day = 24.0 * 60.0 * 60.0;

astronomy_system::astronomy_system(ecs::registry& registry):
	entity_system(registry),
	universal_time(0.0),
	days_per_timestep(1.0 / seconds_per_day),
	observer_location{0.0, 0.0, 0.0},
	lst(0.0),
	obliquity(0.0),
	sky_pass(nullptr),
	sun(entt::null),
	moon(entt::null)
{}

void astronomy_system::update(double t, double dt)
{
	// Add scaled timestep to current time
	set_universal_time(universal_time + dt * days_per_timestep);
	
	// Update horizontal (topocentric) positions of intrasolar celestial bodies
	registry.view<celestial_body_component, transform_component>().each(
	[&](ecs::entity entity, auto& body, auto& transform)
	{
		// Transform orbital position from ecliptic space to horizontal space
		double3 horizontal = ecliptic_to_horizontal * body.orbital_state.r;
		
		// Subtract observer's radial distance (planet radius + observer's altitude)
		horizontal.z -= observer_location[0];
		
		// Convert rectangular horizontal coordinates to spherical
		double3 spherical = astro::rectangular_to_spherical(horizontal);
		spherical.z -= math::pi<double>;
		
		// Find angular radius
		double angular_radius = astro::find_angular_radius(body.radius, spherical.x);
		
		// Transform into local right-handed coordinates
		double3 translation = astro::horizontal_to_right_handed * horizontal;
		double3x3 rotation = astro::horizontal_to_right_handed * ecliptic_to_horizontal;
		
		// Set local transform of transform component
		transform.local.translation = math::type_cast<float>(translation);
		transform.local.rotation = math::type_cast<float>(math::quaternion_cast(rotation));
		transform.local.scale = math::type_cast<float>(double3{body.radius, body.radius, body.radius});
	});
	
	if (sky_pass)
	{
		sky_pass->set_horizon_color({0, 0, 0});
		sky_pass->set_zenith_color({1, 1, 1});
		sky_pass->set_time_of_day(static_cast<float>(universal_time * 60.0 * 60.0));
		//sky_pass->set_observer_location(location[0], location[1], location[2]);
		sky_pass->set_julian_day(static_cast<float>(universal_time));
	}
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

void astronomy_system::set_sky_pass(::sky_pass* pass)
{
	this->sky_pass = pass;
}

void astronomy_system::set_sun(ecs::entity entity)
{
	sun = entity;
}

void astronomy_system::set_moon(ecs::entity entity)
{
	moon = entity;
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
	ecliptic_to_horizontal = astro::ecliptic_to_horizontal(obliquity, observer_location[1], lst);
}

} // namespace ecs
