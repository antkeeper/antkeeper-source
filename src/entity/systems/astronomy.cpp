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

#include "entity/systems/astronomy.hpp"
#include "astro/apparent-size.hpp"
#include "entity/components/blackbody.hpp"
#include "entity/components/transform.hpp"
#include "geom/intersection.hpp"
#include "geom/cartesian.hpp"
#include "color/color.hpp"
#include "physics/orbit/orbit.hpp"
#include "physics/time/ut1.hpp"
#include "physics/light/photometry.hpp"
#include "physics/light/luminosity.hpp"
#include "physics/light/refraction.hpp"
#include "physics/atmosphere.hpp"
#include "geom/cartesian.hpp"
#include <iostream>

namespace entity {
namespace system {

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

astronomy::astronomy(entity::registry& registry):
	updatable(registry),
	universal_time(0.0),
	time_scale(1.0),
	reference_entity(entt::null),
	observer_location{0, 0, 0},
	sun_light(nullptr),
	sky_light(nullptr),
	sky_pass(nullptr)
{
	// Construct transformation which transforms coordinates from ENU to EUS
	enu_to_eus = math::transformation::se3<double>
	{
		{0, 0, 0},
		math::quaternion<double>::rotate_x(-math::half_pi<double>)
	};
	
	registry.on_construct<entity::component::celestial_body>().connect<&astronomy::on_celestial_body_construct>(this);
	registry.on_replace<entity::component::celestial_body>().connect<&astronomy::on_celestial_body_replace>(this);
}

void astronomy::update(double t, double dt)
{
	// Add scaled timestep to current time
	set_universal_time(universal_time + dt * time_scale);
	
	// Abort if no reference body
	if (reference_entity == entt::null)
		return;

	// Abort if either reference body or orbit have not been set
	if (!registry.has<entity::component::orbit>(reference_entity) || !registry.has<entity::component::celestial_body>(reference_entity))
		return;
	const entity::component::orbit& reference_orbit = registry.get<entity::component::orbit>(reference_entity);
	const entity::component::celestial_body& reference_body = registry.get<entity::component::celestial_body>(reference_entity);
	
	math::transformation::se3<double> icrf_to_bci{{-reference_orbit.icrf_position}, math::identity_quaternion<double>};
	
	// Construct transformation from the ICRF frame to the reference body BCBF frame	
	icrf_to_bcbf = physics::orbit::frame::bci::to_bcbf
	(
		reference_body.pole_ra,
		reference_body.pole_dec,
		reference_body.prime_meridian + (math::two_pi<double> / reference_body.rotation_period) * universal_time
	);
	icrf_to_bcbf.t = icrf_to_bcbf.r * -reference_orbit.icrf_position;
	
	icrf_to_enu = icrf_to_bcbf * bcbf_to_enu;
	icrf_to_eus = icrf_to_enu * enu_to_eus;
	
	// Set the transform component translations of orbiting bodies to their topocentric positions
	registry.view<component::celestial_body, component::orbit, component::transform>().each(
	[&](entity::id entity_id, const auto& body, const auto& orbit, auto& transform)
	{
		// Skip reference entity
		if (entity_id == this->reference_entity)
			return;
		
		// Transform orbital Cartesian position (r) from the ICRF frame to the EUS frame
		const double3 r_eus = icrf_to_eus * orbit.icrf_position;
		
		// Determine body orientation in the ICRF frame
		math::quaternion<double> rotation_icrf = physics::orbit::frame::bcbf::to_bci
		(
			body.pole_ra,
			body.pole_dec,
			body.prime_meridian + (math::two_pi<double> / body.rotation_period) * this->universal_time
		).r;
		
		// Transform body orientation from the ICRF frame to the EUS frame.
		math::quaternion<double> rotation_eus = math::normalize(icrf_to_eus.r * rotation_icrf);
		
		// Update local transform
		if (orbit.parent != entt::null)
		{
			transform.local.translation = math::normalize(math::type_cast<float>(r_eus)) * 1000.0f;
			transform.local.rotation = math::type_cast<float>(rotation_eus);
			transform.local.scale = {50.0f, 50.0f, 50.0f};
		}
		
		/*
		if (orbit.parent == entt::null)
		{
			// RA-DEC
			const double3 r_bci = icrf_to_bci * orbit.icrf_position;
			double3 r_bci_spherical = physics::orbit::frame::bci::spherical(r_bci);
			if (r_bci_spherical.z < 0.0)
				r_bci_spherical.z += math::two_pi<double>;
			const double dec = math::degrees(r_bci_spherical.y);
			const double ra = math::degrees(r_bci_spherical.z);
			
			// AZ-EL
			const double3 r_enu = icrf_to_enu * orbit.icrf_position;
			double3 r_enu_spherical = physics::orbit::frame::enu::spherical(r_enu);
			if (r_enu_spherical.z < 0.0)
				r_enu_spherical.z += math::two_pi<double>;
			const double el = math::degrees(r_enu_spherical.y);
			const double az = math::degrees(r_enu_spherical.z);
			
			std::cout << "t: " << this->universal_time << "; ra: " << ra << "; dec: " << dec << std::endl;
			std::cout << "t: " << this->universal_time << "; az: " << az << "; el: " << el << std::endl;
		}
		*/
	});
	
	// Update blackbody lighting
	registry.view<component::celestial_body, component::orbit, component::blackbody>().each(
	[&](entity::id entity_id, const auto& body, const auto& orbit, const auto& blackbody)
	{
		// Calculate blackbody inertial basis
		//double3 blackbody_forward_icrf = math::normalize(reference_orbit.icrf_position - orbit.icrf_position);
		double3 blackbody_up_icrf = {0, 0, 1};
		
		// Transform blackbody ICRF position and basis into EUS frame
		double3 blackbody_position_eus = icrf_to_eus * orbit.icrf_position;
		double3 blackbody_position_enu = icrf_to_enu * orbit.icrf_position;
		double3 blackbody_forward_eus = math::normalize(-blackbody_position_eus);
		double3 blackbody_up_eus = icrf_to_eus.r * blackbody_up_icrf;
		
		// Calculate distance from observer to blackbody
		double blackbody_distance = math::length(blackbody_position_eus) - body.radius;
		
		// Calculate blackbody distance attenuation
		double distance_attenuation = 1.0 / (blackbody_distance * blackbody_distance);
		
		// Init atmospheric transmittance
		double3 atmospheric_transmittance = {1.0, 1.0, 1.0};
		
		// Get atmosphere component of reference body (if any)
		if (this->registry.has<entity::component::atmosphere>(reference_entity))
		{
			const entity::component::atmosphere& reference_atmosphere = registry.get<entity::component::atmosphere>(reference_entity);

			// Altitude of observer in meters	
			geom::ray<double> sample_ray;
			sample_ray.origin = {0, reference_body.radius + observer_location[0], 0};
			sample_ray.direction = math::normalize(blackbody_position_eus);
			
			geom::sphere<double> exosphere;
			exosphere.center = {0, 0, 0};
			exosphere.radius = reference_body.radius + reference_atmosphere.exosphere_altitude;
			
			auto intersection_result = geom::ray_sphere_intersection(sample_ray, exosphere);
			
			if (std::get<0>(intersection_result))
			{
				double3 sample_start = sample_ray.origin;
				double3 sample_end = sample_ray.extrapolate(std::get<2>(intersection_result));
				
				double optical_depth_r = physics::atmosphere::optical_depth(sample_start, sample_end, reference_body.radius, reference_atmosphere.rayleigh_scale_height, 32);
				double optical_depth_k = physics::atmosphere::optical_depth(sample_start, sample_end, reference_body.radius, reference_atmosphere.mie_scale_height, 32);
				double optical_depth_o = 0.0;
				
				atmospheric_transmittance = transmittance(optical_depth_r, optical_depth_k, optical_depth_o, reference_atmosphere.rayleigh_scattering, reference_atmosphere.mie_scattering);
			}
		}
		
		if (sun_light != nullptr)
		{
			// Update blackbody light transform
			sun_light->set_translation(math::normalize(math::type_cast<float>(blackbody_position_eus)));
			sun_light->set_rotation
			(
				math::look_rotation
				(
					math::type_cast<float>(blackbody_forward_eus),
					math::type_cast<float>(blackbody_up_eus)
				)
			);
			
			// Sun illuminance at the outer atmosphere
			float3 sun_illuminance_outer = math::type_cast<float>(blackbody.luminous_intensity * distance_attenuation);
			
			// Sun illuminance at sea level
			float3 sun_illuminance_inner = math::type_cast<float>(blackbody.luminous_intensity * distance_attenuation * atmospheric_transmittance);
			
			// Update blackbody light color and intensity
			sun_light->set_color(sun_illuminance_inner);	
			sun_light->set_intensity(1.0f);
			
			// Upload blackbody params to sky pass
			if (this->sky_pass)
			{
				this->sky_pass->set_sun_position(math::type_cast<float>(blackbody_position_eus));
				this->sky_pass->set_sun_illuminance(sun_illuminance_outer, sun_illuminance_inner);
				
				double blackbody_angular_radius = std::asin((body.radius * 2.0) / (blackbody_distance * 2.0));
				this->sky_pass->set_sun_angular_radius(static_cast<float>(blackbody_angular_radius));
			}
		}
		
		if (sky_light != nullptr)
		{
			double3 blackbody_position_enu_spherical = physics::orbit::frame::enu::spherical(icrf_to_enu * orbit.icrf_position);
			
			double illuminance = 25000.0 * std::max<double>(0.0, std::sin(blackbody_position_enu_spherical.y));
			
			sky_light->set_color({1.0f, 1.0f, 1.0f});
			sky_light->set_intensity(static_cast<float>(illuminance));
		}
	});
	
	// Update sky pass topocentric frame
	if (sky_pass != nullptr)
	{
		// Upload topocentric frame to sky pass
		sky_pass->set_icrf_to_eus
		(
			math::transformation::se3<float>
			{
				math::type_cast<float>(icrf_to_eus.t),
				math::type_cast<float>(icrf_to_eus.r)
			}
		);
		
		// Upload observer altitude to sky pass
		sky_pass->set_observer_altitude(observer_location[0]);
		
		// Upload atmosphere params to sky pass
		if (registry.has<entity::component::atmosphere>(reference_entity))
		{
			const entity::component::atmosphere& reference_atmosphere = registry.get<entity::component::atmosphere>(reference_entity);
			
			sky_pass->set_scale_heights(reference_atmosphere.rayleigh_scale_height, reference_atmosphere.mie_scale_height);
			sky_pass->set_scattering_coefficients(math::type_cast<float>(reference_atmosphere.rayleigh_scattering), math::type_cast<float>(reference_atmosphere.mie_scattering));
			sky_pass->set_mie_anisotropy(reference_atmosphere.mie_anisotropy);
			sky_pass->set_atmosphere_radii(reference_body.radius, reference_body.radius + reference_atmosphere.exosphere_altitude);
		}
	}
}

void astronomy::set_universal_time(double time)
{
	universal_time = time;
}

void astronomy::set_time_scale(double scale)
{
	time_scale = scale;
}

void astronomy::set_reference_body(entity::id entity_id)
{
	reference_entity = entity_id;
	update_bcbf_to_enu();
}

void astronomy::set_observer_location(const double3& location)
{
	observer_location = location;
	update_bcbf_to_enu();
}

void astronomy::set_sun_light(scene::directional_light* light)
{
	sun_light = light;
}

void astronomy::set_sky_light(scene::ambient_light* light)
{
	sky_light = light;
}

void astronomy::set_sky_pass(::render::sky_pass* pass)
{
	this->sky_pass = pass;
}

void astronomy::on_celestial_body_construct(entity::registry& registry, entity::id entity_id, entity::component::celestial_body& celestial_body)
{
	if (entity_id == reference_entity)
		update_bcbf_to_enu();
}

void astronomy::on_celestial_body_replace(entity::registry& registry, entity::id entity_id, entity::component::celestial_body& celestial_body)
{
	if (entity_id == reference_entity)
		update_bcbf_to_enu();
}

void astronomy::update_bcbf_to_enu()
{
	double radial_distance = observer_location[0];
	
	if (reference_entity)
	{
		if (registry.has<entity::component::celestial_body>(reference_entity))
			radial_distance += registry.get<entity::component::celestial_body>(reference_entity).radius;
	}
	
	// Construct reference frame which transforms coordinates from a BCBF frame to a horizontal frame
	bcbf_to_enu = physics::orbit::frame::bcbf::to_enu
	(
		radial_distance,
		observer_location[1],
		observer_location[2]
	);
}

} // namespace system
} // namespace entity
