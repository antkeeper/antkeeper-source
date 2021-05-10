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
#include "coordinates/coordinates.hpp"
#include "astro/apparent-size.hpp"
#include "ecs/components/celestial-body-component.hpp"
#include "ecs/components/transform-component.hpp"
#include "renderer/passes/sky-pass.hpp"
#include "color/color.hpp"
#include <iostream>

namespace ecs {

static constexpr double seconds_per_day = 24.0 * 60.0 * 60.0;

astronomy_system::astronomy_system(ecs::registry& registry):
	entity_system(registry),
	universal_time(0.0),
	days_per_timestep(1.0 / seconds_per_day),
	observer_location{0.0, 0.0, 0.0},
	lst(0.0),
	obliquity(0.0),
	axial_rotation(0.0),
	axial_rotation_at_epoch(0.0),
	axial_rotation_speed(0.0),
	sky_pass(nullptr),
	sun_light(nullptr)
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
		double3 spherical = coordinates::rectangular::to_spherical(horizontal);
		
		// Find angular radius
		double angular_radius = astro::find_angular_radius(body.radius, spherical[0]);
		
		// Transform into local coordinates
		const double3x3 horizontal_to_local =
		{
			0.0, 0.0, -1.0,
			1.0, 0.0, 0.0,
			0.0, 1.0, 0.0
		};
		
		double3 translation = horizontal_to_local * horizontal;
		double3x3 rotation = horizontal_to_local * ecliptic_to_horizontal;
		
		// Set local transform of transform component
		transform.local.translation = math::type_cast<float>(translation);
		transform.local.rotation = math::type_cast<float>(math::quaternion_cast(rotation));
		transform.local.scale = math::type_cast<float>(double3{body.radius, body.radius, body.radius});
		
		if (sun_light != nullptr)
		{
			math::quaternion<float> sun_azimuth_rotation = math::angle_axis(static_cast<float>(spherical.z), float3{0, 1, 0});
			math::quaternion<float> sun_elevation_rotation = math::angle_axis(static_cast<float>(spherical.y), float3{-1, 0, 0});
			math::quaternion<float> sun_az_el_rotation = math::normalize(sun_azimuth_rotation * sun_elevation_rotation);
			
			//sun_az_el_rotation = math::angle_axis((float)universal_time * math::two_pi<float>, float3{1, 0, 0});
			
			//
			//sun_light->look_at({0, 0, 0}, {0, -1, 0}, {0, 0, 1});
			
			// Set sun color
			float cct = 3000.0f + std::sin(spherical.y) * 5000.0f;
			float3 color_xyz = color::cct::to_xyz(cct);
			float3 color_acescg = color::xyz::to_acescg(color_xyz);
			
			sun_light->set_color(color_acescg);

			// Set sun intensity (in lux)
			float intensity = std::max(0.0, std::sin(spherical.y) * 108000.0f);
			sun_light->set_intensity(intensity);
			
			
			sun_light->set_translation({0, 500, 0});
			//sun_light->set_rotation(math::look_rotation(math::normalize(transform.local.translation), {0, 1, 0}));
			sun_light->set_rotation(sun_az_el_rotation);
			//sun_light->set_rotation(sun_elevation_rotation);
			
			if (this->sky_pass)
			{
				this->sky_pass->set_sun_coordinates(sun_az_el_rotation * float3{0, 0, -1}, {static_cast<float>(spherical.z), static_cast<float>(spherical.y)});
			}
		}
	});
	
	if (sky_pass)
	{
		// Calculate local time
		double time_correction = observer_location[2] / (math::two_pi<double> / 24.0);
		double local_jd = universal_time + time_correction / 24.0 - 0.5;
		double local_time = (local_jd - std::floor(local_jd)) * 24.0;
		
		sky_pass->set_time_of_day(local_time);
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
	update_axial_rotation();
}

void astronomy_system::set_axial_rotation_at_epoch(double angle)
{
	axial_rotation_at_epoch = angle;
	update_axial_rotation();
}

void astronomy_system::set_sky_pass(::sky_pass* pass)
{
	sky_pass = pass;
}

void astronomy_system::set_sun_light(scene::directional_light* light)
{
	sun_light = light;
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
	ecliptic_to_horizontal = coordinates::rectangular::ecliptic::to_horizontal(obliquity, observer_location[1], lst);
}

} // namespace ecs
