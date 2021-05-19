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
#include "astro/apparent-size.hpp"
#include "ecs/components/celestial-body-component.hpp"
#include "ecs/components/transform-component.hpp"
#include "renderer/passes/sky-pass.hpp"
#include "color/color.hpp"
#include "physics/orbit/orbit.hpp"
#include "geom/cartesian.hpp"
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
	
	set_universal_time(0.0);
	
	// Update horizontal (topocentric) positions of intrasolar celestial bodies
	registry.view<celestial_body_component, transform_component>().each(
	[&](ecs::entity entity, auto& body, auto& transform)
	{
		double time_correction = observer_location[2] / (math::two_pi<double> / 24.0);
		double local_jd = universal_time + time_correction / 24.0 - 0.5;
		double local_time = (local_jd - std::floor(local_jd)) * 24.0;
		double local_lst = local_time / 24.0f * math::two_pi<float>;
		
		// Transform orbital position from ecliptic space to horizontal space
		//double3 horizontal = ecliptic_to_horizontal * body.orbital_state.r;
		double3 horizontal = ecliptic_to_horizontal * double3{1, 0, 0};
		
		// Subtract observer's radial distance (planet radius + observer's altitude)
		//horizontal.z -= observer_location[0];
		
		// Convert Cartesian horizontal coordinates to spherical
		double3 spherical = geom::cartesian::to_spherical(horizontal);
		
		// Find angular radius
		double angular_radius = astro::find_angular_radius(body.radius, spherical[0]);
		
		// Transform into local coordinates
		const double3x3 horizontal_to_local = math::rotate_x(-math::half_pi<double>) * math::rotate_z(-math::half_pi<double>);
		
		double3 translation = horizontal_to_local * horizontal;
		double3x3 rotation = horizontal_to_local * ecliptic_to_horizontal;
		
		
		// Set local transform of transform component
		transform.local.translation = math::type_cast<float>(translation);
		transform.local.rotation = math::normalize(math::type_cast<float>(math::quaternion_cast(rotation)));
		transform.local.scale = math::type_cast<float>(double3{body.radius, body.radius, body.radius});
		
		if (sun_light != nullptr)
		{
			const double universal_time_cy = universal_time * 2.7397e-5;
			const double3 solar_system_barycenter = {0, 0, 0};
						
			physics::orbit::elements<double> earth_elements;
			earth_elements.a = 1.00000261 + 0.00000562 * universal_time_cy;
			earth_elements.e = 0.01671123 + -0.00004392 * universal_time_cy;
			
			earth_elements.i = math::radians(-0.00001531) + math::radians(-0.01294668) * universal_time_cy;
			earth_elements.raan = 0.0;
			
			const double earth_elements_mean_longitude = math::radians(100.46457166) + math::radians(35999.37244981) * universal_time_cy;
			const double earth_elements_longitude_perihelion = math::radians(102.93768193) + math::radians(0.32327364) * universal_time_cy;
			
			earth_elements.w = earth_elements_longitude_perihelion - earth_elements.raan;
			earth_elements.ta = earth_elements_mean_longitude - earth_elements_longitude_perihelion;
			
			
			// Calculate semi-minor axis, b
			double b = physics::orbit::derive_semiminor_axis(earth_elements.a, earth_elements.e);
			
			// Solve Kepler's equation for eccentric anomaly (E)
			double ea = physics::orbit::kepler_ea(earth_elements.e, earth_elements.ta, 10, 1e-6);
			
			// Calculate radial distance, r; and true anomaly, v
			double xv = earth_elements.a * (std::cos(ea) - earth_elements.e);
			double yv = b * std::sin(ea);
			double r = std::sqrt(xv * xv + yv * yv);
			double ta = std::atan2(yv, xv);
			
			// Position of the body in perifocal space
			const math::vector3<double> earth_position_pqw = math::quaternion<double>::rotate_z(ta) * math::vector3<double>{r, 0, 0};
			
			
			const double earth_axial_tilt = math::radians(23.45);
			const double earth_axial_rotation = math::two_pi<double> * (0.7790572732640 + 1.00273781191135448 * universal_time);
			const double earth_radius_au = 4.2635e-5;
			
			const double observer_altitude = earth_radius_au;
			const double observer_latitude = math::radians(0.0);
			const double observer_longitude = math::radians(0.0);
			
			const physics::frame<double> earth_inertial_to_pqw = physics::orbit::inertial::to_perifocal(solar_system_barycenter, earth_elements.raan, earth_elements.i, earth_elements.w);
			const math::vector3<double> earth_position_inertial = earth_inertial_to_pqw.inverse() * earth_position_pqw;
			
			const math::vector3<double> sun_position_intertial = math::vector3<double>{0, 0, 0};
			
			const physics::frame<double> earth_inertial_to_bci = physics::orbit::inertial::to_bci(earth_position_inertial, earth_elements.i, earth_axial_tilt);
			const physics::frame<double> earth_inertial_to_bcbf = physics::orbit::inertial::to_bcbf(earth_position_inertial, earth_elements.i, earth_axial_tilt, earth_axial_rotation);
			const physics::frame<double> earth_bcbf_to_topo = physics::orbit::bcbf::to_topocentric(observer_altitude, observer_latitude, observer_longitude);
			
			const math::vector3<double> sun_position_earth_bci = earth_inertial_to_bci * sun_position_intertial;
			const math::vector3<double> sun_position_earth_bcbf = earth_inertial_to_bcbf * sun_position_intertial;
			const math::vector3<double> sun_position_earth_topo = earth_bcbf_to_topo * sun_position_earth_bcbf;
			
			const math::vector3<double> sun_radec = geom::cartesian::to_spherical(sun_position_earth_bci);
			const math::vector3<double> sun_azel = geom::cartesian::to_spherical(sun_position_earth_topo);
			
			const double sun_az = sun_azel.z;
			const double sun_el = sun_azel.y;
			
			double sun_ra = sun_radec.z;
			const double sun_dec = sun_radec.y;
			
			if (sun_ra < 0.0)
				sun_ra += math::two_pi<double>;
			
			std::cout << "ra: " << (sun_ra / math::two_pi<double> * 24.0) << "; dec: " << math::degrees(sun_dec) << std::endl;
			std::cout << "az: " << math::degrees(math::pi<double> - sun_az) << "; el: " << math::degrees(sun_el) << std::endl;
			
			
			float az = spherical.z;
			float el = spherical.y;
			
			if (az < 0.0f)
				az += math::two_pi<float>;
			
			//std::cout << "local: " << translation << std::endl;
			//std::cout << "az: " << math::degrees(az) << "; ";
			//std::cout << "el: " << math::degrees(el) << std::endl;
			
			math::quaternion<float> sun_azimuth_rotation = math::angle_axis(static_cast<float>(spherical.z), float3{0, 1, 0});
			math::quaternion<float> sun_elevation_rotation = math::angle_axis(static_cast<float>(spherical.y), float3{1, 0, 0});
			math::quaternion<float> sun_az_el_rotation = math::normalize(sun_azimuth_rotation * sun_elevation_rotation);
			
			// Set sun color
			float cct = 3000.0f + std::sin(spherical.y) * 5000.0f;
			float3 color_xyz = color::cct::to_xyz(cct);
			float3 color_acescg = color::xyz::to_acescg(color_xyz);
			
			sun_light->set_color(color_acescg);

			// Set sun intensity (in lux)
			float intensity = std::max(0.0, std::sin(spherical.y) * 108000.0f);
			sun_light->set_intensity(intensity);
			
			
			//sun_light->set_translation({0, 500, 0});
			sun_light->set_translation(transform.local.translation);
			//sun_light->set_rotation(transform.local.rotation);
			//sun_light->set_rotation(sun_az_el_rotation);
			//sun_light->set_rotation(sun_elevation_rotation);
			sun_light->set_rotation(math::look_rotation(math::normalize(-transform.local.translation), {0, 0, -1}));
			
			if (this->sky_pass)
			{
				this->sky_pass->set_sun_coordinates(transform.local.rotation * float3{0, 0, -1}, {static_cast<float>(spherical.z), static_cast<float>(spherical.y)});
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
	//ecliptic_to_horizontal = coordinates::rectangular::ecliptic::to_horizontal(obliquity, observer_location[1], lst);
}

} // namespace ecs
