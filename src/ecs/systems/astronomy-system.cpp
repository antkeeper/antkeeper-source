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
#include "ecs/components/blackbody-component.hpp"
#include "ecs/components/transform-component.hpp"
#include "geom/intersection.hpp"
#include "color/color.hpp"
#include "physics/orbit/orbit.hpp"
#include "physics/time/ut1.hpp"
#include "physics/light/photometry.hpp"
#include "physics/light/luminosity.hpp"
#include "physics/light/refraction.hpp"
#include "physics/atmosphere.hpp"
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
	reference_entity(entt::null),
	reference_orbit(nullptr),
	reference_body(nullptr),
	reference_atmosphere(nullptr),
	sun_light(nullptr),
	sky_pass(nullptr)
{}

void astronomy_system::update(double t, double dt)
{
	// Add scaled timestep to current time
	set_universal_time(universal_time + dt * time_scale);
	
	// Abort if either reference body or orbit have not been set
	if (!reference_orbit || !reference_body)
		return;
	
	// Determine axial rotation at current time
	const double reference_axial_rotation = reference_body->axial_rotation + reference_body->angular_frequency * universal_time;
	
	// Construct reference frame which transforms coordinates from inertial space to reference body BCBF space
	inertial_to_bcbf = physics::orbit::inertial::to_bcbf
	(
		reference_orbit->state.r,
		reference_orbit->elements.i,
		reference_body->axial_tilt,
		reference_axial_rotation
	);
	
	// Construct reference frame which transforms coordinates from inertial space to reference body topocentric space
	inertial_to_topocentric = inertial_to_bcbf * bcbf_to_topocentric;
	
	// Set the transform component translations of orbiting bodies to their topocentric positions
	registry.view<celestial_body_component, orbit_component, transform_component>().each(
	[&](ecs::entity entity, const auto& celestial_body, const auto& orbit, auto& transform)
	{
		// Transform Cartesian position vector (r) from inertial space to topocentric space
		const math::vector3<double> r_topocentric = inertial_to_topocentric * orbit.state.r;
		
		// Update local transform
		transform.local.translation = math::type_cast<float>(r_topocentric);
	});
	
	// Update blackbody lighting
	registry.view<celestial_body_component, orbit_component, blackbody_component>().each(
	[&](ecs::entity entity, const auto& celestial_body, const auto& orbit, const auto& blackbody)
	{
		// Calculate blackbody inertial basis
		double3 blackbody_forward_inertial = math::normalize(reference_orbit->state.r - orbit.state.r);
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
		if (reference_atmosphere)
		{
			// Altitude of observer in meters	
			geom::ray<double> sample_ray;
			sample_ray.origin = {0, observer_location[0], 0};
			sample_ray.direction = math::normalize(blackbody_position_topocentric);
			
			geom::sphere<double> exosphere;
			exosphere.center = {0, 0, 0};
			exosphere.radius = reference_body->radius + reference_atmosphere->exosphere_altitude;
			
			auto intersection_result = geom::ray_sphere_intersection(sample_ray, exosphere);
			
			if (std::get<0>(intersection_result))
			{
				double3 sample_start = sample_ray.origin;
				double3 sample_end = sample_ray.extrapolate(std::get<2>(intersection_result));
				
				double optical_depth_r = physics::atmosphere::optical_depth(sample_start, sample_end, reference_body->radius, reference_atmosphere->rayleigh_scale_height, 32);
				double optical_depth_k = physics::atmosphere::optical_depth(sample_start, sample_end, reference_body->radius, reference_atmosphere->mie_scale_height, 32);
				double optical_depth_o = 0.0;
				
				atmospheric_transmittance = transmittance(optical_depth_r, optical_depth_k, optical_depth_o, reference_atmosphere->rayleigh_scattering, reference_atmosphere->mie_scattering);
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
				
				double blackbody_angular_radius = std::asin((celestial_body.radius * 2.0) / (blackbody_distance * 2.0));
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
		float observer_altitude = observer_location[0] - reference_body->radius;
		sky_pass->set_observer_altitude(observer_altitude);
		
		// Upload atmosphere params to sky pass
		if (reference_atmosphere)
		{
			sky_pass->set_scale_heights(reference_atmosphere->rayleigh_scale_height, reference_atmosphere->mie_scale_height);
			sky_pass->set_scattering_coefficients(math::type_cast<float>(reference_atmosphere->rayleigh_scattering), math::type_cast<float>(reference_atmosphere->mie_scattering));
			sky_pass->set_mie_anisotropy(reference_atmosphere->mie_anisotropy);
			sky_pass->set_atmosphere_radii(reference_body->radius, reference_body->radius + reference_atmosphere->exosphere_altitude);
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
	reference_entity = entity;
	reference_orbit = nullptr;
	reference_body = nullptr;
	reference_atmosphere = nullptr;
	
	if (reference_entity != entt::null)
	{
		if (registry.has<ecs::orbit_component>(reference_entity))
			reference_orbit = &registry.get<ecs::orbit_component>(reference_entity);
		
		if (registry.has<ecs::celestial_body_component>(reference_entity))
			reference_body = &registry.get<ecs::celestial_body_component>(reference_entity);
		
		if (registry.has<ecs::atmosphere_component>(reference_entity))
			reference_atmosphere = &registry.get<ecs::atmosphere_component>(reference_entity);
	}
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

} // namespace ecs
