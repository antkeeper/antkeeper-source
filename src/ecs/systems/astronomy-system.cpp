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
#include "ecs/components/orbit-component.hpp"
#include "ecs/components/blackbody-component.hpp"
#include "ecs/components/atmosphere-component.hpp"
#include "ecs/components/transform-component.hpp"
#include "geom/intersection.hpp"
#include "color/color.hpp"
#include "physics/orbit/orbit.hpp"
#include "physics/time/ut1.hpp"
#include "physics/light/blackbody.hpp"
#include "physics/light/photometry.hpp"
#include "physics/light/luminosity.hpp"
#include "geom/cartesian.hpp"
#include <iostream>

namespace ecs {

/**
 * Calculates the optical depth between two points.
 *
 * @param start Start point.
 * @param end End point.
 * @param sample_count Number of samples to take between the start and end points.
 * @param scale_height Scale height of the atmospheric particles to measure.
 */
template <class T>
T transmittance(math::vector3<T> start, math::vector3<T> end, std::size_t sample_count, T scale_height)
{
	const T inverse_scale_height = T(1) / -scale_height;
	
	math::vector3<T> direction = end - start;
	T distance = length(direction);
	direction /= distance;
	
	// Calculate the distance between each sample point
	T step_distance = distance / T(sample_count);
	
	// Sum the atmospheric particle densities at each sample point
	T total_density = 0.0;
	math::vector3<T> sample_point = start;
	for (std::size_t i = 0; i < sample_count; ++i)
	{
		// Determine altitude of sample point
		T altitude = length(sample_point);
		
		// Calculate atmospheric particle density at sample altitude
		T density = exp(altitude * inverse_scale_height);
		
		// Add density to the total density
		total_density += density;
		
		// Advance to next sample point
		sample_point += direction * step_distance;
	}
	
	// Scale the total density by the step distance
	return total_density * step_distance;
}

astronomy_system::astronomy_system(ecs::registry& registry):
	entity_system(registry),
	universal_time(0.0),
	time_scale(1.0),
	reference_body(entt::null),
	reference_body_axial_tilt(0.0),
	reference_body_axial_rotation(0.0),
	sun_light(nullptr),
	sky_pass(nullptr)
{}

void astronomy_system::update(double t, double dt)
{
	// Add scaled timestep to current time
	set_universal_time(universal_time + dt * time_scale);
	
	// Abort if reference body has not been set
	if (reference_body == entt::null)
		return;
	
	// Abort if reference body has no orbit component
	if (!registry.has<ecs::orbit_component>(reference_body))
		return;
	
	// Update axial rotation of reference body
	reference_body_axial_rotation = physics::time::ut1::era(universal_time);
	
	// Get orbit component of reference body
	const auto& reference_orbit = registry.get<ecs::orbit_component>(reference_body);
	
	/// Construct reference frame which transforms coordinates from inertial space to reference body BCBF space
	inertial_to_bcbf = physics::orbit::inertial::to_bcbf
	(
		reference_orbit.state.r,
		reference_orbit.elements.i,
		reference_body_axial_tilt,
		reference_body_axial_rotation
	);
	
	/// Construct reference frame which transforms coordinates from inertial space to reference body topocentric space
	inertial_to_topocentric = inertial_to_bcbf * bcbf_to_topocentric;
	
	// Set the transform component translations of orbiting bodies to their topocentric positions
	registry.view<orbit_component, transform_component>().each(
	[&](ecs::entity entity, auto& orbit, auto& transform)
	{
		// Transform Cartesian position vector (r) from inertial space to topocentric space
		const math::vector3<double> r_topocentric = inertial_to_topocentric * orbit.state.r;
		
		// Update local transform
		transform.local.translation = math::type_cast<float>(r_topocentric);
	});
	
	// Get atmosphere component of reference body, if any
	if (registry.has<ecs::atmosphere_component>(reference_body))
	{
		const ecs::atmosphere_component& atmosphere = registry.get<ecs::atmosphere_component>(reference_body);
	}
	
	if (sun_light != nullptr)
	{
		const math::vector3<double> sun_position_inertial = {0, 0, 0};
		const math::vector3<double> sun_forward_inertial = math::normalize(reference_orbit.state.r - sun_position_inertial);
		const math::vector3<double> sun_up_inertial = {0, 0, 1};
		
		// Transform sun position, forward, and up vectors into topocentric space
		const math::vector3<double> sun_position_topocentric = inertial_to_topocentric * sun_position_inertial;
		const math::vector3<double> sun_forward_topocentric = inertial_to_topocentric.rotation * sun_forward_inertial;
		const math::vector3<double> sun_up_topocentric = inertial_to_topocentric.rotation * sun_up_inertial;
		
		// Update sun light transform
		sun_light->set_translation(math::type_cast<float>(sun_position_topocentric));
		sun_light->set_rotation
		(
			math::look_rotation
			(
				math::type_cast<float>(sun_forward_topocentric),
				math::type_cast<float>(sun_up_topocentric)
			)
		);
		
		// Convert sun topocentric Cartesian coordinates to spherical coordinates
		math::vector3<double> sun_az_el = geom::cartesian::to_spherical(ezs_to_sez * sun_position_topocentric);
		sun_az_el.z = math::pi<double> - sun_az_el.z;
		
		//std::cout << "el: " << math::degrees(sun_az_el.y) << "; az: " << math::degrees(sun_az_el.z) << std::endl;
		
		// If the reference body has an atmosphere
		if (registry.has<ecs::atmosphere_component>(reference_body))
		{
			// Get the atmosphere component of the reference body
			const auto& atmosphere = registry.get<ecs::atmosphere_component>(reference_body);
			
			const double meters_per_au = 1.496e+11;
			const double earth_radius_au = 4.26352e-5;
			const double earth_radius_m = earth_radius_au * meters_per_au;
			const double observer_altitude_m = (observer_location[0] - earth_radius_au) * meters_per_au;
			
			// Altitude of observer in meters			
			geom::ray<double> sample_ray;
			sample_ray.origin = {0, observer_altitude_m, 0};
			sample_ray.direction = math::normalize(sun_position_topocentric);
			
			geom::sphere<double> exosphere;
			exosphere.center = {0, -earth_radius_m, 0};
			exosphere.radius = atmosphere.exosphere_radius;
			
			auto intersection_result = geom::ray_sphere_intersection(sample_ray, exosphere);
			
			if (std::get<0>(intersection_result))
			{
				double3 sample_start = sample_ray.origin;
				double3 sample_end = sample_ray.extrapolate(std::get<2>(intersection_result));
				
				double transmittance_rayleigh = transmittance(sample_start, sample_end, 16, atmosphere.rayleigh_scale_height);
				double transmittance_mie = transmittance(sample_start, sample_end, 16, atmosphere.mie_scale_height);
				
				// Calculate attenuation due to atmospheric scattering
				double3 scattering_attenuation =
					atmosphere.rayleigh_scattering_coefficients * transmittance_rayleigh +
					atmosphere.mie_scattering_coefficients * transmittance_mie;
				scattering_attenuation.x = std::exp(-scattering_attenuation.x);
				scattering_attenuation.y = std::exp(-scattering_attenuation.y);
				scattering_attenuation.z = std::exp(-scattering_attenuation.z);
				
				double scattering_mean = (scattering_attenuation.x + scattering_attenuation.y + scattering_attenuation.z) / 3.0;
				
				const double sun_temperature = 5777.0;
				const double sun_radius = 6.957e+8;
				const double sun_surface_area = 4.0 * math::pi<double> * sun_radius * sun_radius;
				
				// Calculate distance attenuation
				double sun_distance_m = math::length(sun_position_topocentric) * meters_per_au;
				double distance_attenuation = 1.0 / (sun_distance_m * sun_distance_m);
				
				double sun_luminous_flux = blackbody_luminous_flux(sun_temperature, sun_radius);
				double sun_luminous_intensity = sun_luminous_flux / (4.0 * math::pi<double>);
				double sun_illuminance = sun_luminous_intensity / (sun_distance_m * sun_distance_m);
				
				std::cout << "distance atten: " << distance_attenuation << std::endl;
				std::cout << "scatter atten: " << scattering_attenuation << std::endl;
				
				std::cout << "luminous flux: " << sun_luminous_flux << std::endl;
				std::cout << "luminous intensity: " << sun_luminous_intensity << std::endl;
				std::cout << "illuminance: " << sun_illuminance * scattering_mean << std::endl;
				
				
				// Calculate sun color
				double3 color_xyz = color::cct::to_xyz(sun_temperature);
				double3 color_acescg = color::xyz::to_acescg(color_xyz);
				
				sun_light->set_color(math::type_cast<float>(color_acescg * scattering_attenuation));
				
				sun_light->set_intensity(sun_illuminance);
			}
		}
	}
	
	if (sky_pass != nullptr)
	{
		sky_pass->set_topocentric_frame
		(
			physics::frame<float>
			{
				math::type_cast<float>(inertial_to_topocentric.translation),
				math::type_cast<float>(inertial_to_topocentric.rotation)
			}
		);
		
		sky_pass->set_sun_object(sun_light);
	}
}

void astronomy_system::set_universal_time(double time)
{
	universal_time = time;
}

void astronomy_system::set_time_scale(double scale)
{
	time_scale = scale;
}

void astronomy_system::set_reference_body(ecs::entity entity)
{
	reference_body = entity;
}

void astronomy_system::set_reference_body_axial_tilt(double angle)
{
	reference_body_axial_tilt = angle;
}

void astronomy_system::set_observer_location(const double3& location)
{
	observer_location = location;
	
	// Construct reference frame which transforms coordinates from SEZ to EZS
	sez_to_ezs = physics::frame<double>
	{
		{0, 0, 0},
		math::normalize
		(
			math::quaternion<double>::rotate_x(-math::half_pi<double>) *
				math::quaternion<double>::rotate_z(-math::half_pi<double>)
		)
	};
	
	// Construct reference frame which transforms coordinates from EZS to SEZ
	ezs_to_sez = sez_to_ezs.inverse();
	
	// Construct reference frame which transforms coordinates from BCBF space to topocentric space
	bcbf_to_topocentric = physics::orbit::bcbf::to_topocentric
	(
		observer_location[0], // Radial distance
		observer_location[1], // Latitude
		observer_location[2]  // Longitude
	) * sez_to_ezs;
}

void astronomy_system::set_sun_light(scene::directional_light* light)
{
	sun_light = light;
}

void astronomy_system::set_sky_pass(::sky_pass* pass)
{
	this->sky_pass = pass;
}

double astronomy_system::blackbody_luminous_flux(double t, double r)
{
	// Blackbody spectral power distribution function
	auto spd = [t](double x) -> double
	{
		// Convert nanometers to meters
		x *= double(1e-9);
		
		return physics::light::blackbody::spectral_radiance<double>(t, x, physics::constants::speed_of_light<double>);
	};
	
	// Luminous efficiency function (photopic)
	auto lef = [](double x) -> double
	{
		return physics::light::luminosity::photopic<double>(x);
	};
	
	// Construct range of spectral sample points
	std::vector<double> samples(10000);
	std::iota(samples.begin(), samples.end(), 10);
	
	// Calculate luminous efficiency
	const double efficiency = physics::light::luminous_efficiency<double>(spd, lef, samples.begin(), samples.end());
	
	// Calculate surface area of spherical blackbody
	const double a = double(4) * math::pi<double> * r * r;
	
	// Calculate radiant flux
	const double radiant_flux = physics::light::blackbody::radiant_flux(t, a);
	
	// Convert radiant flux to luminous flux
	const double luminous_flux = physics::light::watts_to_lumens<double>(radiant_flux, efficiency);
	
	return luminous_flux;
}

} // namespace ecs
