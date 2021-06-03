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
#include "physics/light/refraction.hpp"
#include "physics/atmosphere.hpp"
#include "math/quadrature.hpp"
#include "geom/cartesian.hpp"
#include <iostream>

namespace ecs {

template <class T>
math::vector3<T> transmittance(T depth_r, T depth_m, T depth_o, const math::vector3<T>& beta_r, const math::vector3<T>& beta_m)
{
	math::vector3<T> transmittance_r = beta_r * depth_r;
	math::vector3<T> transmittance_m = beta_m * 1.1 * depth_m;
	math::vector3<T> transmittance_o = {0, 0, 0};
	
	math::vector3<T> t = transmittance_r + transmittance_m + transmittance_o;
	t.x = std::exp(-t.x);
	t.y = std::exp(-t.y);
	t.z = std::exp(-t.z);
	
	return t;
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
	// RGB wavelengths determined by matching wavelengths to XYZ, transforming XYZ to ACEScg, then selecting the max wavelengths for R, G, and B.
	rgb_wavelengths_nm = {602.224, 541.069, 448.143};
	rgb_wavelengths_m = rgb_wavelengths_nm * 1e-9;
	
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
	
	const double earth_radius = 6.3781e6;
	
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
		double blackbody_distance = math::length(blackbody_position_topocentric);
		
		// Calculate blackbody distance attenuation
		double distance_attenuation = 1.0 / (blackbody_distance * blackbody_distance);
		
		// Init atmospheric transmittance
		double3 atmospheric_transmittance = {1.0, 1.0, 1.0};
		
		// Get atmosphere component of reference body (if any)
		if (this->registry.has<ecs::atmosphere_component>(reference_body))
		{
			const ecs::atmosphere_component& atmosphere = this->registry.get<ecs::atmosphere_component>(reference_body);
			
			// Altitude of observer in meters	
			geom::ray<double> sample_ray;
			sample_ray.origin = {0, observer_location[0], 0};
			sample_ray.direction = math::normalize(blackbody_position_topocentric);
			
			geom::sphere<double> exosphere;
			exosphere.center = {0, 0, 0};
			exosphere.radius = earth_radius + atmosphere.exosphere_altitude;
			
			auto intersection_result = geom::ray_sphere_intersection(sample_ray, exosphere);
			
			if (std::get<0>(intersection_result))
			{
				double3 sample_start = sample_ray.origin;
				double3 sample_end = sample_ray.extrapolate(std::get<2>(intersection_result));
				
				double optical_depth_r = physics::atmosphere::optical_depth(sample_start, sample_end, earth_radius, atmosphere.rayleigh_scale_height, 32);
				double optical_depth_k = physics::atmosphere::optical_depth(sample_start, sample_end, earth_radius, atmosphere.mie_scale_height, 32);
				double optical_depth_o = 0.0;
				
				atmospheric_transmittance = transmittance(optical_depth_r, optical_depth_k, optical_depth_o, atmosphere.rayleigh_scattering, atmosphere.mie_scattering);
			}
		}
		
		if (sun_light != nullptr)
		{
			// Update blackbody light transform
			sun_light->set_translation(math::normalize(math::type_cast<float>(blackbody_position_topocentric)));
			sun_light->set_rotation
			(
				math::look_rotation
				(
					math::type_cast<float>(blackbody_forward_topocentric),
					math::type_cast<float>(blackbody_up_topocentric)
				)
			);
			
			// Update blackbody light color and intensity
			sun_light->set_color(math::type_cast<float>(blackbody.luminous_intensity * atmospheric_transmittance));	
			sun_light->set_intensity(static_cast<float>(distance_attenuation));
			
			// Upload blackbody params to sky pass
			if (this->sky_pass)
			{
				this->sky_pass->set_sun_position(math::type_cast<float>(blackbody_position_topocentric));
				this->sky_pass->set_sun_color(math::type_cast<float>(blackbody.luminous_intensity * distance_attenuation));
				
				double blackbody_angular_radius = std::asin((blackbody.radius * 2.0) / (blackbody_distance * 2.0));
				this->sky_pass->set_sun_angular_radius(static_cast<float>(blackbody_angular_radius));
			}
		}
	});
	
	// Update sky pass topocentric frame
	if (sky_pass != nullptr)
	{
		// Upload topocentric frame to sky pass
		sky_pass->set_topocentric_frame
		(
			physics::frame<float>
			{
				math::type_cast<float>(inertial_to_topocentric.translation),
				math::type_cast<float>(inertial_to_topocentric.rotation)
			}
		);
		
		// Upload observer altitude to sky pass
		float observer_altitude = observer_location[0] - earth_radius;
		sky_pass->set_observer_altitude(observer_altitude);
		
		// Upload atmosphere params to sky pass
		if (this->registry.has<ecs::atmosphere_component>(reference_body))
		{
			const ecs::atmosphere_component& atmosphere = this->registry.get<ecs::atmosphere_component>(reference_body);
			
			sky_pass->set_scale_heights(atmosphere.rayleigh_scale_height, atmosphere.mie_scale_height);
			sky_pass->set_scattering_coefficients(math::type_cast<float>(atmosphere.rayleigh_scattering), math::type_cast<float>(atmosphere.mie_scattering));
			sky_pass->set_mie_anisotropy(atmosphere.mie_anisotropy);
			sky_pass->set_atmosphere_radii(earth_radius, earth_radius + atmosphere.exosphere_altitude);
		}
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
	// Calculate the surface area of a spherical blackbody
	const double surface_area = 4.0 * math::pi<double> * blackbody.radius * blackbody.radius;
	
	// Construct a lambda function which calculates the blackbody's RGB luminous intensity of a given wavelength
	auto rgb_luminous_intensity = [blackbody, surface_area](double wavelength_nm) -> double3
	{
		// Convert wavelength from nanometers to meters
		const double wavelength_m = wavelength_nm * 1e-9;
		
		// Calculate the spectral intensity of the wavelength
		const double spectral_intensity = physics::light::blackbody::spectral_intensity<double>(blackbody.temperature, surface_area, wavelength_m);
		
		// Calculate the ACEScg color of the wavelength using CIE color matching functions
		double3 spectral_color = color::xyz::to_acescg(color::xyz::match(wavelength_nm));
		
		// Scale the spectral color by spectral intensity
		return spectral_color * spectral_intensity * 1e-9 * physics::light::max_luminous_efficacy<double>;
	};
	
	// Construct a range of sample wavelengths in the visible spectrum
	std::vector<double> samples(780 - 280);
	std::iota(samples.begin(), samples.end(), 280);
	
	// Integrate the blackbody RGB luminous intensity over wavelengths in the visible spectrum
	blackbody.luminous_intensity = math::quadrature::simpson(rgb_luminous_intensity, samples.begin(), samples.end());
}

void astronomy_system::on_atmosphere_construct(ecs::registry& registry, ecs::entity entity, ecs::atmosphere_component& atmosphere)
{
	on_atmosphere_replace(registry, entity, atmosphere);
}

void astronomy_system::on_atmosphere_replace(ecs::registry& registry, ecs::entity entity, ecs::atmosphere_component& atmosphere)
{
	// Calculate polarization factors
	const double rayleigh_polarization = physics::atmosphere::polarization(atmosphere.index_of_refraction, atmosphere.rayleigh_density);
	const double mie_polarization = physics::atmosphere::polarization(atmosphere.index_of_refraction, atmosphere.mie_density);
	
	// Calculate Rayleigh scattering coefficients
	atmosphere.rayleigh_scattering =
	{
		physics::atmosphere::scattering_rayleigh(rgb_wavelengths_m.x, atmosphere.rayleigh_density, rayleigh_polarization),
		physics::atmosphere::scattering_rayleigh(rgb_wavelengths_m.y, atmosphere.rayleigh_density, rayleigh_polarization),
		physics::atmosphere::scattering_rayleigh(rgb_wavelengths_m.z, atmosphere.rayleigh_density, rayleigh_polarization)
	};
	
	// Calculate Mie scattering coefficients
	const double mie_scattering = physics::atmosphere::scattering_mie(atmosphere.mie_density, mie_polarization);
	atmosphere.mie_scattering = 
	{
		mie_scattering,
		mie_scattering,
		mie_scattering
	};
}

} // namespace ecs
