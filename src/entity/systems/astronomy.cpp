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
#include "entity/components/diffuse-reflector.hpp"
#include "geom/intersection.hpp"
#include "geom/cartesian.hpp"
#include "color/color.hpp"
#include "physics/orbit/orbit.hpp"
#include "physics/time/ut1.hpp"
#include "physics/light/photometry.hpp"
#include "physics/light/luminosity.hpp"
#include "physics/light/refraction.hpp"
#include "physics/gas/atmosphere.hpp"
#include "geom/cartesian.hpp"
#include "astro/apparent-size.hpp"
#include "geom/solid-angle.hpp"
#include "math/polynomial.hpp"
#include <iostream>

namespace entity {
namespace system {

template <class T>
math::vector3<T> transmittance(T depth_r, T depth_m, T depth_o, const math::vector3<T>& beta_r, const math::vector3<T>& beta_m, const math::vector3<T>& beta_o)
{
	math::vector3<T> transmittance_r = beta_r * depth_r;
	math::vector3<T> transmittance_m = beta_m * (T(10)/T(9)) * depth_m;
	math::vector3<T> transmittance_o = beta_o * depth_o;
	
	math::vector3<T> t = transmittance_r + transmittance_m + transmittance_o;
	t.x = std::exp(-t.x);
	t.y = std::exp(-t.y);
	t.z = std::exp(-t.z);
	
	return t;
}

astronomy::astronomy(entity::registry& registry):
	updatable(registry),
	time(0.0),
	time_scale(1.0),
	reference_entity(entt::null),
	observer_location{0, 0, 0},
	sun_light(nullptr),
	sky_light(nullptr),
	moon_light(nullptr),
	camera(nullptr),
	sky_pass(nullptr),
	exposure_offset(0.0),
	starlight_illuminance(0.0)
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

astronomy::~astronomy()
{
	registry.on_construct<entity::component::celestial_body>().disconnect<&astronomy::on_celestial_body_construct>(this);
	registry.on_replace<entity::component::celestial_body>().disconnect<&astronomy::on_celestial_body_replace>(this);
}

void astronomy::update(double t, double dt)
{
	double total_illuminance = 0.0;
	double sky_light_illuminance = 0.0;
	
	// Add scaled timestep to current time
	set_time(time + dt * time_scale);
	
	// Abort if no reference body
	if (reference_entity == entt::null)
		return;

	// Abort if either reference body or orbit have not been set
	if (!registry.has<entity::component::orbit>(reference_entity) || !registry.has<entity::component::celestial_body>(reference_entity))
		return;
	const entity::component::orbit& reference_orbit = registry.get<entity::component::orbit>(reference_entity);
	const entity::component::celestial_body& reference_body = registry.get<entity::component::celestial_body>(reference_entity);
	
	math::transformation::se3<double> icrf_to_bci{{-reference_orbit.position}, math::quaternion<double>::identity};
	
	const double days_from_epoch = time;
	const double centuries_from_epoch = days_from_epoch / 36525.0;
	
	// Evaluate reference body orientation polynomials
	const double reference_body_pole_ra = math::polynomial::horner(reference_body.pole_ra.begin(), reference_body.pole_ra.end(), centuries_from_epoch);
	const double reference_body_pole_dec = math::polynomial::horner(reference_body.pole_dec.begin(), reference_body.pole_dec.end(), centuries_from_epoch);
	const double reference_body_prime_meridian = math::polynomial::horner(reference_body.prime_meridian.begin(), reference_body.prime_meridian.end(), days_from_epoch);
	
	// Construct transformation from the ICRF frame to the reference body BCBF frame	
	icrf_to_bcbf = physics::orbit::frame::bci::to_bcbf
	(
		reference_body_pole_ra,
		reference_body_pole_dec,
		reference_body_prime_meridian
	);
	icrf_to_bcbf.t = icrf_to_bcbf.r * -reference_orbit.position;
	
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
		const double3 r_eus = icrf_to_eus * orbit.position;
		
		// Evaluate body orientation polynomials
		const double body_pole_ra = math::polynomial::horner(body.pole_ra.begin(), body.pole_ra.end(), centuries_from_epoch);
		const double body_pole_dec = math::polynomial::horner(body.pole_dec.begin(), body.pole_dec.end(), centuries_from_epoch);
		const double body_prime_meridian = math::polynomial::horner(body.prime_meridian.begin(), body.prime_meridian.end(), days_from_epoch);
		
		// Determine body orientation in the ICRF frame
		math::quaternion<double> rotation_icrf = physics::orbit::frame::bcbf::to_bci
		(
			body_pole_ra,
			body_pole_dec,
			body_prime_meridian
		).r;
		
		// Transform body orientation from the ICRF frame to the EUS frame.
		math::quaternion<double> rotation_eus = math::normalize(icrf_to_eus.r * rotation_icrf);
		
		// Update local transform
		if (orbit.parent != entt::null)
		{
			transform.local.translation = math::normalize(math::type_cast<float>(r_eus));
			transform.local.rotation = math::type_cast<float>(rotation_eus);
			transform.local.scale = {1.0f, 1.0f, 1.0f};
		}
		
		/*
		if (orbit.parent != entt::null)
		{
			// RA-DEC
			const double3 r_bci = icrf_to_bci * orbit.position;
			double3 r_bci_spherical = physics::orbit::frame::bci::spherical(r_bci);
			if (r_bci_spherical.z < 0.0)
				r_bci_spherical.z += math::two_pi<double>;
			const double dec = math::degrees(r_bci_spherical.y);
			const double ra = math::degrees(r_bci_spherical.z);
			
			// AZ-EL
			const double3 r_enu = icrf_to_enu * orbit.position;
			double3 r_enu_spherical = physics::orbit::frame::enu::spherical(r_enu);
			if (r_enu_spherical.z < 0.0)
				r_enu_spherical.z += math::two_pi<double>;
			const double el = math::degrees(r_enu_spherical.y);
			const double az = math::degrees(r_enu_spherical.z);
			
			std::cout << "t: " << this->time << "; ra: " << ra << "; dec: " << dec << std::endl;
			std::cout << "t: " << this->time << "; az: " << az << "; el: " << el << std::endl;
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
		double3 blackbody_position_eus = icrf_to_eus * orbit.position;
		double3 blackbody_position_enu = icrf_to_enu * orbit.position;
		double3 blackbody_forward_eus = math::normalize(-blackbody_position_eus);
		double3 blackbody_up_eus = icrf_to_eus.r * blackbody_up_icrf;
		
		// Calculate distance from observer to blackbody
		double blackbody_distance = math::length(blackbody_position_eus) - body.radius;
		
		// Calculate blackbody solid angle
		const double blackbody_angular_radius = astro::angular_radius(body.radius, blackbody_distance);
		const double blackbody_solid_angle = geom::solid_angle::cone(blackbody_angular_radius);
		
		// Calculate blackbody illuminance
		const double3 blackbody_illuminance = blackbody.luminance * blackbody_solid_angle;
		
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
				
				double optical_depth_r = physics::gas::atmosphere::optical_depth_exp(sample_start, sample_end, reference_body.radius, reference_atmosphere.rayleigh_scale_height, 16);
				double optical_depth_m = physics::gas::atmosphere::optical_depth_exp(sample_start, sample_end, reference_body.radius, reference_atmosphere.mie_scale_height, 16);
				double optical_depth_o = physics::gas::atmosphere::optical_depth_tri(sample_start, sample_end, reference_body.radius, reference_atmosphere.ozone_lower_limit, reference_atmosphere.ozone_upper_limit, reference_atmosphere.ozone_mode, 16);
				
				atmospheric_transmittance = transmittance(optical_depth_r, optical_depth_m, optical_depth_o, reference_atmosphere.rayleigh_scattering, reference_atmosphere.mie_scattering, reference_atmosphere.ozone_absorption);
			}
			
			// Add airglow to sky light illuminance
			sky_light_illuminance += reference_atmosphere.airglow;
		}
		
		// Blackbody illuminance transmitted through the atmosphere
		const double3 transmitted_blackbody_illuminance = blackbody_illuminance * atmospheric_transmittance;
		
		// Add atmosphere-transmitted blackbody illuminance to total illuminance
		total_illuminance += (transmitted_blackbody_illuminance.x + transmitted_blackbody_illuminance.y + transmitted_blackbody_illuminance.z) / 3.0;
		
		// Update sun light
		if (sun_light != nullptr)
		{
			sun_light->set_translation(math::normalize(math::type_cast<float>(blackbody_position_eus)));
			sun_light->set_rotation
			(
				math::look_rotation
				(
					math::type_cast<float>(blackbody_forward_eus),
					math::type_cast<float>(blackbody_up_eus)
				)
			);
			
			sun_light->set_color(math::type_cast<float>(transmitted_blackbody_illuminance));	
		}
		
		// Update sky light
		if (sky_light != nullptr)
		{
			// Calculate sky illuminance
			double3 blackbody_position_enu_spherical = physics::orbit::frame::enu::spherical(blackbody_position_enu);
			const double sky_illuminance = 25000.0 * std::max<double>(0.0, std::sin(blackbody_position_enu_spherical.y));
			
			// Add sky illuminance to sky light illuminance
			sky_light_illuminance += sky_illuminance;
			
			// Add starlight illuminance to sky light illuminance
			sky_light_illuminance += starlight_illuminance;
			
			// Add sky light illuminance to total illuminance
			total_illuminance += sky_light_illuminance;
			
			//std::cout << "sky light illum: " << sky_light_illuminance << std::endl;
			
			// Update sky light
			sky_light->set_color(float3{1.0f, 1.0f, 1.0f} * static_cast<float>(sky_light_illuminance));
		}
		
		// Upload blackbody params to sky pass
		if (this->sky_pass)
		{
			this->sky_pass->set_sun_position(math::type_cast<float>(blackbody_position_eus));
			this->sky_pass->set_sun_luminance(math::type_cast<float>(blackbody.luminance));
			this->sky_pass->set_sun_illuminance(math::type_cast<float>(blackbody_illuminance));
			this->sky_pass->set_sun_angular_radius(static_cast<float>(blackbody_angular_radius));
		}
		
		const double3& blackbody_icrf_position = orbit.position;
		
		// Update diffuse reflectors
		this->registry.view<component::celestial_body, component::orbit, component::diffuse_reflector, component::transform>().each(
		[&](entity::id entity_id, const auto& reflector_body, const auto& reflector_orbit, const auto& reflector, const auto& transform)
		{
			// Calculate distance to blackbody and direction of incoming light
			double3 blackbody_light_direction_icrf = reflector_orbit.position - blackbody_icrf_position;
			double blackbody_distance = math::length(blackbody_light_direction_icrf);
			blackbody_light_direction_icrf = blackbody_light_direction_icrf / blackbody_distance;
			
			// Transform blackbody light direction from the ICRF frame to the EUS frame
			double3 blackbody_light_direction_eus = icrf_to_eus.r * blackbody_light_direction_icrf;
			
			// Calculate blackbody solid angle
			const double blackbody_angular_radius = astro::angular_radius(body.radius, blackbody_distance);
			const double blackbody_solid_angle = geom::solid_angle::cone(blackbody_angular_radius);
			
			// Calculate blackbody illuminance
			
			double3 view_direction_icrf = reflector_orbit.position - reference_orbit.position;
			const double reflector_distance = math::length(view_direction_icrf);
			view_direction_icrf = view_direction_icrf / reflector_distance;
			
			const double3 sunlight_illuminance = blackbody.luminance * blackbody_solid_angle;
			const double reflector_angular_radius = astro::angular_radius(reflector_body.radius, reflector_distance);
			const double reflector_solid_angle = geom::solid_angle::cone(reflector_angular_radius);
			const double reflector_phase_factor = dot(view_direction_icrf, blackbody_light_direction_icrf) * 0.5 + 0.5;
			
			const double3 planet_luminance = (sunlight_illuminance * reference_body.albedo) / math::pi<double>;
			const double planet_angular_radius = astro::angular_radius(reference_body.radius, reflector_distance);
			const double planet_solid_angle = geom::solid_angle::cone(planet_angular_radius);
			const double planet_phase_factor = math::dot(-view_direction_icrf, math::normalize(reference_orbit.position - blackbody_icrf_position)) * 0.5 + 0.5;
			const double3 planetlight_illuminance = planet_luminance * planet_solid_angle * planet_phase_factor;
			double3 planetlight_direction_eus = math::normalize(icrf_to_eus.r * view_direction_icrf);
			
			const double3 reflected_sunlight_luminance = (sunlight_illuminance * reflector.albedo) / math::pi<double>;
			const double3 reflected_sunlight_illuminance = reflected_sunlight_luminance * reflector_solid_angle * reflector_phase_factor;
			const double3 reflected_planetlight_luminance = (planetlight_illuminance * reflector.albedo) / math::pi<double>;
			const double3 reflected_planetlight_illuminance = reflected_planetlight_luminance * reflector_solid_angle;
			
			/*
			std::cout << "reflected sunlight illuminance: " << reflected_sunlight_illuminance << std::endl;
			std::cout << "planetlight illuminance: " << planetlight_illuminance << std::endl;
			std::cout << "planet luminance: " << planet_luminance << std::endl;
			std::cout << "reflected planetlight luminance: " << reflected_planetlight_luminance << std::endl;
			std::cout << "reflected planetlight illuminance: " << reflected_planetlight_illuminance << std::endl;
			std::cout << "reflector phase: " << reflector_phase_factor << std::endl;
			std::cout << "planet phase: " << planet_phase_factor << std::endl;
			*/
			
			
			if (this->sky_pass)
			{
				this->sky_pass->set_moon_position(transform.local.translation);
				this->sky_pass->set_moon_rotation(transform.local.rotation);
				this->sky_pass->set_moon_angular_radius(static_cast<float>(reflector_angular_radius));
				this->sky_pass->set_moon_sunlight_direction(math::type_cast<float>(blackbody_light_direction_eus));
				this->sky_pass->set_moon_sunlight_illuminance(math::type_cast<float>(sunlight_illuminance));
				this->sky_pass->set_moon_planetlight_direction(math::type_cast<float>(planetlight_direction_eus));
				this->sky_pass->set_moon_planetlight_illuminance(math::type_cast<float>(planetlight_illuminance));
			}
			
			if (this->moon_light)
			{
				float3 reflector_up_eus = math::type_cast<float>(icrf_to_eus.r * double3{0, 0, 1});
				
				double3 reflected_illuminance = reflected_sunlight_illuminance + reflected_planetlight_illuminance;
				//reflected_illuminance *= std::max<double>(0.0, std::sin(transform.local.translation.y));
				
				total_illuminance += (reflected_illuminance.x + reflected_illuminance.y + reflected_illuminance.z) / 3.0;
				
				this->moon_light->set_color(math::type_cast<float>(reflected_illuminance));
				this->moon_light->set_rotation
				(
					math::look_rotation
					(
						math::normalize(-transform.local.translation),
						reflector_up_eus
					)
				);
			}
			
			/*
			std::cout << "moon: sun solid angle: " << blackbody_solid_angle << std::endl;
			std::cout << "moon: sun illuminance: " << blackbody_illuminance << std::endl;
			std::cout << "moon: moon luminance: " << reflector_luminance << std::endl;
			std::cout << "sun brightness: " << sun_brightness << std::endl;
			std::cout << "vega brightness: " << vega_brightness << std::endl;
			std::cout << "earth: moon distance: " << reflector_distance << std::endl;
			std::cout << "earth: moon solid angle: " << reflector_solid_angle << std::endl;
			std::cout << "earth: moon phase: " << reflector_phase << std::endl;
			std::cout << "earth: moon phase angle: " << math::degrees(reflector_phase_angle) << std::endl;
			std::cout << "earth: moon illum %: " << reflector_illumination_factor * 100.0 << std::endl;
			std::cout << "earth: moon illuminance: " << reflector_illuminance << std::endl;
			std::cout << "earth: moon phase-modulated illuminance: " << reflector_illuminance * reflector_illumination_factor << std::endl;
			*/
		});
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
			
			sky_pass->set_particle_distributions
			(
				static_cast<float>(reference_atmosphere.rayleigh_scale_height),
				static_cast<float>(reference_atmosphere.mie_scale_height),
				static_cast<float>(reference_atmosphere.ozone_lower_limit),
				static_cast<float>(reference_atmosphere.ozone_upper_limit),
				static_cast<float>(reference_atmosphere.ozone_mode)
			);
			sky_pass->set_scattering_coefficients(math::type_cast<float>(reference_atmosphere.rayleigh_scattering), math::type_cast<float>(reference_atmosphere.mie_scattering));
			sky_pass->set_mie_anisotropy(reference_atmosphere.mie_anisotropy);
			sky_pass->set_absorption_coefficients(math::type_cast<float>(reference_atmosphere.ozone_absorption));
			sky_pass->set_atmosphere_radii(reference_body.radius, reference_body.radius + reference_atmosphere.exosphere_altitude);
		}
	}
	
	// Auto-exposure
	if (camera)
	{
		const double calibration = 250.0;
		const double ev100 = std::log2((total_illuminance * 100.0) / calibration);
		//std::cout << "LUX: " << total_illuminance << std::endl;
		//std::cout << "EV100: " << ev100 << std::endl;
		//camera->set_exposure(exposure_offset);
	}
}

void astronomy::set_time(double time)
{
	this->time = time;
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

void astronomy::set_moon_light(scene::directional_light* light)
{
	moon_light = light;
}

void astronomy::set_camera(scene::camera* camera)
{
	this->camera = camera;
}

void astronomy::set_exposure_offset(float offset)
{
	exposure_offset = offset;
}

void astronomy::set_starlight_illuminance(double illuminance)
{
	starlight_illuminance = illuminance;
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
