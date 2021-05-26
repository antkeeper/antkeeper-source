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
 * Approximates the density of exponentially-distributed atmospheric particles between two points using the trapezoidal rule.
 *
 * @param a Start point.
 * @param b End point.
 * @param r Radius of the planet.
 * @param sh Scale height of the atmospheric particles.
 * @param n Number of samples.
 */
template <class T>
T optical_depth(const math::vector3<T>& a, const math::vector3<T>& b, T r, T sh, std::size_t n)
{
	T inverse_sh = T(-1) / sh;
	
	T h = math::length(b - a) / T(n);
	
	math::vector3<T> dy = (b - a) / T(n);
	math::vector3<T> y = a + dy;
	
	T f_x = std::exp((length(a) - r) * inverse_sh);
	T f_y = std::exp((length(y) - r) * inverse_sh);
	T sum = (f_x + f_y);
	
	for (std::size_t i = 1; i < n; ++i)
	{
		f_x = f_y;
		y += dy;
		f_y = std::exp((length(y) - r) * inverse_sh);
		sum += (f_x + f_y);
	}
	
	return sum / T(2) * h;
}

template <class T>
math::vector3<T> transmittance(T depth_r, T depth_m, T depth_o, const math::vector3<T>& beta_r, const math::vector3<T>& beta_m)
{
	math::vector3<T> transmittance_r = beta_r * depth_r;
	math::vector3<T> transmittance_m = beta_m * depth_m;
	math::vector3<T> transmittance_o = {0, 0, 0};
	
	math::vector3<T> t = transmittance_r + transmittance_m + transmittance_o;
	t.x = std::exp(-t.x);
	t.y = std::exp(-t.y);
	t.z = std::exp(-t.z);
	
	return t;
}

double calc_beta_r(double wavelength, double ior, double density)
{
	double wavelength2 = wavelength * wavelength;
	double ior2m1 = ior * ior - 1.0;
	double num = 8.0 * (math::pi<double> * math::pi<double> * math::pi<double>) * ior2m1 * ior2m1;
	double den = 3.0 * density * (wavelength2 * wavelength2);
	return num / den;
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
{
	registry.on_construct<ecs::blackbody_component>().connect<&astronomy_system::on_blackbody_construct>(this);
	registry.on_replace<ecs::blackbody_component>().connect<&astronomy_system::on_blackbody_replace>(this);
	
	registry.on_construct<ecs::atmosphere_component>().connect<&astronomy_system::on_atmosphere_construct>(this);
	registry.on_replace<ecs::atmosphere_component>().connect<&astronomy_system::on_atmosphere_replace>(this);
}

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
	
	// Update blackbody lighting
	registry.view<blackbody_component, orbit_component>().each(
	[&](ecs::entity entity, auto& blackbody, auto& orbit)
	{		
		// Calculate blackbody inertial basis
		double3 blackbody_forward_inertial = math::normalize(reference_orbit.state.r - orbit.state.r);
		double3 blackbody_up_inertial = {0, 0, 1};
		
		// Transform blackbody inertial position and basis into topocentric space
		double3 blackbody_position_topocentric = inertial_to_topocentric * orbit.state.r;
		double3 blackbody_forward_topocentric = inertial_to_topocentric.rotation * blackbody_forward_inertial;
		double3 blackbody_up_topocentric = inertial_to_topocentric.rotation * blackbody_up_inertial;
		
		// Calculate distance from observer to blackbody
		const double meters_per_au = 1.496e+11;
		double blackbody_distance = math::length(blackbody_position_topocentric) * meters_per_au;
		
		// Calculate blackbody illuminance according to distance
		double blackbody_illuminance = blackbody.luminous_intensity / (blackbody_distance * blackbody_distance);
		
		// Get blackbody color
		double3 blackbody_color = blackbody.color;
		
		// Get atmosphere component of reference body, if any
		if (this->registry.has<ecs::atmosphere_component>(reference_body))
		{
			const ecs::atmosphere_component& atmosphere = this->registry.get<ecs::atmosphere_component>(reference_body);
			
			const double earth_radius_au = 4.26352e-5;
			const double earth_radius_m = earth_radius_au * meters_per_au;
			
			// Altitude of observer in meters	
			geom::ray<double> sample_ray;
			sample_ray.origin = {0, observer_location[0] * meters_per_au, 0};
			sample_ray.direction = math::normalize(blackbody_position_topocentric);
			
			geom::sphere<double> exosphere;
			exosphere.center = {0, 0, 0};
			exosphere.radius = earth_radius_m + atmosphere.exosphere_altitude;
			
			auto intersection_result = geom::ray_sphere_intersection(sample_ray, exosphere);
			
			if (std::get<0>(intersection_result))
			{
				double3 sample_start = sample_ray.origin;
				double3 sample_end = sample_ray.extrapolate(std::get<2>(intersection_result));
				
				double optical_depth_r = optical_depth(sample_start, sample_end, earth_radius_m, atmosphere.rayleigh_scale_height, 32);
				double optical_depth_k = optical_depth(sample_start, sample_end, earth_radius_m, atmosphere.mie_scale_height, 32);
				double optical_depth_o = 0.0;
				
				double3 attenuation = transmittance(optical_depth_r, optical_depth_k, optical_depth_o, atmosphere.rayleigh_scattering_coefficients, atmosphere.mie_scattering_coefficients);
				
				// Attenuate blackbody color
				blackbody_color *= attenuation;
			}
		}
		
		if (sun_light != nullptr)
		{
			// Update blackbody light transform
			sun_light->set_translation(math::type_cast<float>(blackbody_position_topocentric));
			sun_light->set_rotation
			(
				math::look_rotation
				(
					math::type_cast<float>(blackbody_forward_topocentric),
					math::type_cast<float>(blackbody_up_topocentric)
				)
			);
			
			// Update blackbody light color and intensity
			sun_light->set_color(math::type_cast<float>(blackbody_color));	
			sun_light->set_intensity(static_cast<float>(blackbody_illuminance));
			
			// Pass blackbody params to sky pas
			if (this->sky_pass)
			{
				this->sky_pass->set_sun_object(sun_light);
				this->sky_pass->set_sun_color(math::type_cast<float>(blackbody.color * blackbody_illuminance));
			}
		}
	});
	
	// Update sky pass topocentric frame
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

void astronomy_system::on_blackbody_construct(ecs::registry& registry, ecs::entity entity, ecs::blackbody_component& blackbody)
{
	on_blackbody_replace(registry, entity, blackbody);
}

void astronomy_system::on_blackbody_replace(ecs::registry& registry, ecs::entity entity, ecs::blackbody_component& blackbody)
{
	// Calculate surface area of spherical blackbody
	const double surface_area = double(4) * math::pi<double> * blackbody.radius * blackbody.radius;
	
	// Calculate radiant flux
	blackbody.radiant_flux = physics::light::blackbody::radiant_flux(blackbody.temperature, surface_area);
	
	// Blackbody spectral power distribution function
	auto spd = [blackbody](double x) -> double
	{
		// Convert nanometers to meters
		x *= double(1e-9);
		
		return physics::light::blackbody::spectral_radiance<double>(blackbody.temperature, x, physics::constants::speed_of_light<double>);
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
	
	// Convert radiant flux to luminous flux
	blackbody.luminous_flux = physics::light::watts_to_lumens<double>(blackbody.radiant_flux, efficiency);
	
	// Calculate luminous intensity from luminous flux
	blackbody.luminous_intensity = blackbody.luminous_flux / (4.0 * math::pi<double>);
	
	// Calculate blackbody color from temperature
	double3 color_xyz = color::cct::to_xyz(blackbody.temperature);
	blackbody.color = color::xyz::to_acescg(color_xyz);
}

void astronomy_system::on_atmosphere_construct(ecs::registry& registry, ecs::entity entity, ecs::atmosphere_component& atmosphere)
{
	on_atmosphere_replace(registry, entity, atmosphere);
}

void astronomy_system::on_atmosphere_replace(ecs::registry& registry, ecs::entity entity, ecs::atmosphere_component& atmosphere)
{
	// ACEScg wavelengths determined by matching wavelengths to XYZ, transforming XYZ to ACEScg, then selecting the max wavelengths for R, G, and B.
	const double3 acescg_wavelengths_nm = {600.0, 540.0, 450.0};
	const double3 acescg_wavelengths_m = acescg_wavelengths_nm * 1.0e-9;
	
	// Calculate Rayleigh scattering coefficients
	const double air_ior = 1.0003;
	const double molecular_density = 2.545e25;
	double3 beta_r;
	atmosphere.rayleigh_scattering_coefficients =
	{
		calc_beta_r(acescg_wavelengths_m.x, air_ior, molecular_density),
		calc_beta_r(acescg_wavelengths_m.y, air_ior, molecular_density),
		calc_beta_r(acescg_wavelengths_m.z, air_ior, molecular_density)
	};
	
	// Calculate Mie scattering coefficients
	atmosphere.mie_scattering_coefficients = {2.0e-6, 2.0e-6, 2.0e-6};
}

} // namespace ecs
