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

#include "game/system/astronomy.hpp"
#include "astro/apparent-size.hpp"
#include "game/component/blackbody.hpp"
#include "game/component/transform.hpp"
#include "game/component/diffuse-reflector.hpp"
#include "geom/intersection.hpp"
#include "geom/cartesian.hpp"
#include "color/color.hpp"
#include "physics/orbit/orbit.hpp"
#include "physics/time/ut1.hpp"
#include "physics/time/jd.hpp"
#include "physics/light/photometry.hpp"
#include "physics/light/luminosity.hpp"
#include "physics/light/refraction.hpp"
#include "physics/gas/atmosphere.hpp"
#include "geom/cartesian.hpp"
#include "astro/apparent-size.hpp"
#include "geom/solid-angle.hpp"
#include "math/polynomial.hpp"
#include <iostream>

namespace game {
namespace system {

astronomy::astronomy(entity::registry& registry):
	updatable(registry),
	time_days(0.0),
	time_centuries(0.0),
	time_scale(1.0),
	observer_eid(entt::null),
	reference_body_eid(entt::null),
	transmittance_samples(0),
	sun_light(nullptr),
	sky_light(nullptr),
	moon_light(nullptr),
	bounce_light(nullptr),
	bounce_albedo{0, 0, 0},
	sky_pass(nullptr),
	starlight_illuminance{0, 0, 0}
{
	// Construct ENU to EUS transformation
	enu_to_eus = math::transformation::se3<double>
	{
		{0, 0, 0},
		math::quaternion<double>::rotate_x(-math::half_pi<double>)
	};
	
	registry.on_construct<game::component::observer>().connect<&astronomy::on_observer_modified>(this);
	registry.on_update<game::component::observer>().connect<&astronomy::on_observer_modified>(this);
	registry.on_destroy<game::component::observer>().connect<&astronomy::on_observer_destroyed>(this);
	registry.on_construct<game::component::celestial_body>().connect<&astronomy::on_celestial_body_modified>(this);
	registry.on_update<game::component::celestial_body>().connect<&astronomy::on_celestial_body_modified>(this);
	registry.on_destroy<game::component::celestial_body>().connect<&astronomy::on_celestial_body_destroyed>(this);
	registry.on_construct<game::component::orbit>().connect<&astronomy::on_orbit_modified>(this);
	registry.on_update<game::component::orbit>().connect<&astronomy::on_orbit_modified>(this);
	registry.on_destroy<game::component::orbit>().connect<&astronomy::on_orbit_destroyed>(this);
	registry.on_construct<game::component::atmosphere>().connect<&astronomy::on_atmosphere_modified>(this);
	registry.on_update<game::component::atmosphere>().connect<&astronomy::on_atmosphere_modified>(this);
	registry.on_destroy<game::component::atmosphere>().connect<&astronomy::on_atmosphere_destroyed>(this);
}

astronomy::~astronomy()
{
	registry.on_construct<game::component::observer>().disconnect<&astronomy::on_observer_modified>(this);
	registry.on_update<game::component::observer>().disconnect<&astronomy::on_observer_modified>(this);
	registry.on_destroy<game::component::observer>().disconnect<&astronomy::on_observer_destroyed>(this);
	registry.on_construct<game::component::celestial_body>().disconnect<&astronomy::on_celestial_body_modified>(this);
	registry.on_update<game::component::celestial_body>().disconnect<&astronomy::on_celestial_body_modified>(this);
	registry.on_destroy<game::component::celestial_body>().disconnect<&astronomy::on_celestial_body_destroyed>(this);
	registry.on_construct<game::component::orbit>().disconnect<&astronomy::on_orbit_modified>(this);
	registry.on_update<game::component::orbit>().disconnect<&astronomy::on_orbit_modified>(this);
	registry.on_destroy<game::component::orbit>().disconnect<&astronomy::on_orbit_destroyed>(this);
	registry.on_construct<game::component::atmosphere>().disconnect<&astronomy::on_atmosphere_modified>(this);
	registry.on_update<game::component::atmosphere>().disconnect<&astronomy::on_atmosphere_modified>(this);
	registry.on_destroy<game::component::atmosphere>().disconnect<&astronomy::on_atmosphere_destroyed>(this);
}

void astronomy::update(double t, double dt)
{
	double3 sky_light_illuminance = {0.0, 0.0, 0.0};
	
	// Add scaled timestep to current time
	set_time(time_days + dt * time_scale);
	
	// Abort if no valid observer entity or reference body entity
	if (observer_eid == entt::null || reference_body_eid == entt::null)
		return;
	
	// Get pointer to observer component
	const auto observer = registry.try_get<component::observer>(observer_eid);
	
	// Abort if no observer component
	if (!observer)
		return;
	
	// Get pointers to reference body components
	const auto
	[
		reference_body,
		reference_orbit,
		reference_atmosphere
	] = registry.try_get<component::celestial_body, component::orbit, component::atmosphere>(reference_body_eid);
	
	// Abort if no reference body or reference orbit
	if (!reference_body || !reference_orbit)
		return;
	
	// Update ICRF to EUS transformation
	update_icrf_to_eus(*reference_body, *reference_orbit);
	
	// Set the transform component translations of orbiting bodies to their topocentric positions
	registry.view<component::celestial_body, component::orbit, component::transform>().each(
	[&](entity::id entity_id, const auto& body, const auto& orbit, auto& transform)
	{
		// Skip reference body entity
		if (entity_id == reference_body_eid)
			return;
		
		// Transform orbital Cartesian position (r) from the ICRF frame to the EUS frame
		const double3 r_eus = icrf_to_eus * orbit.position;
		
		// Evaluate body orientation polynomials
		const double body_pole_ra = math::polynomial::horner(body.pole_ra.begin(), body.pole_ra.end(), time_centuries);
		const double body_pole_dec = math::polynomial::horner(body.pole_dec.begin(), body.pole_dec.end(), time_centuries);
		const double body_prime_meridian = math::polynomial::horner(body.prime_meridian.begin(), body.prime_meridian.end(), time_days);
		
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
			transform.local.translation = math::normalize(float3(r_eus));
			transform.local.rotation = math::type_cast<float>(rotation_eus);
			transform.local.scale = {1.0f, 1.0f, 1.0f};
		}
	});
	
	constexpr double3 bounce_normal = {0, 1, 0};
	double3 bounce_illuminance = {0, 0, 0};
	
	// Update blackbody lighting
	registry.view<component::celestial_body, component::orbit, component::blackbody>().each(
	[&](entity::id entity_id, const auto& blackbody_body, const auto& blackbody_orbit, const auto& blackbody)
	{
		// Transform blackbody position from ICRF frame to EUS frame
		const double3 blackbody_position_eus = icrf_to_eus * blackbody_orbit.position;
		
		// Measure distance and direction, in EUS frame, from observer to blackbody
		const double observer_blackbody_distance = math::length(blackbody_position_eus);
		const double3 observer_blackbody_direction_eus = blackbody_position_eus / observer_blackbody_distance;
		
		// Measure blackbody solid angle as seen by observer
		const double observer_blackbody_angular_radius = astro::angular_radius(blackbody_body.radius, observer_blackbody_distance);
		const double observer_blackbody_solid_angle = geom::solid_angle::cone(observer_blackbody_angular_radius);
		
		// Calculate illuminance from blackbody reaching observer
		const double3 observer_blackbody_illuminance = blackbody.luminance * observer_blackbody_solid_angle;
		
		// Calculate illuminance from blackbody reaching observer after atmospheric extinction
		double3 observer_blackbody_transmitted_illuminance = observer_blackbody_illuminance;
		if (reference_atmosphere)
		{
			// Construct ray at observer pointing towards the blackbody
			const geom::ray<double> ray = {{0, 0, 0}, observer_blackbody_direction_eus};
			
			// Integrate atmospheric spectral transmittance factor between observer and blackbody
			const double3 transmittance = integrate_transmittance(*observer, *reference_body, *reference_atmosphere, ray);
			
			// Attenuate illuminance from blackbody reaching observer by spectral transmittance factor
			observer_blackbody_transmitted_illuminance *= transmittance;
		}
		
		// Update sun light
		if (sun_light != nullptr)
		{
			const double3 blackbody_up_eus = icrf_to_eus.r * double3{0, 0, 1};
			sun_light->set_rotation
			(
				math::look_rotation
				(
					float3(-observer_blackbody_direction_eus),
					float3(blackbody_up_eus)
				)
			);
			
			sun_light->set_color(float3(observer_blackbody_transmitted_illuminance));
			
			// Bounce sun light
			bounce_illuminance += std::max(0.0, math::dot(bounce_normal, -observer_blackbody_direction_eus)) * observer_blackbody_transmitted_illuminance * bounce_albedo;
		}
		
		// Update sky light
		if (sky_light != nullptr)
		{
			// Calculate sky illuminance
			double3 blackbody_position_enu_spherical = physics::orbit::frame::enu::spherical(enu_to_eus.inverse() * blackbody_position_eus);
			const double sky_illuminance = 25000.0 * std::max<double>(0.0, std::sin(blackbody_position_enu_spherical.y()));
			
			// Add sky illuminance to sky light illuminance
			sky_light_illuminance += {sky_illuminance, sky_illuminance, sky_illuminance};
			
			// Add starlight illuminance to sky light illuminance
			sky_light_illuminance += starlight_illuminance;
			
			// Update sky light
			sky_light->set_color(float3(sky_light_illuminance));
			
			// Bounce sky light
			bounce_illuminance += sky_light_illuminance * bounce_albedo;
		}
		
		// Upload blackbody params to sky pass
		if (this->sky_pass)
		{
			this->sky_pass->set_sun_position(float3(blackbody_position_eus));
			this->sky_pass->set_sun_luminance(float3(blackbody.luminance));
			this->sky_pass->set_sun_illuminance(float3(observer_blackbody_illuminance), float3(observer_blackbody_transmitted_illuminance));
			this->sky_pass->set_sun_angular_radius(static_cast<float>(observer_blackbody_angular_radius));
		}
		
		// Update diffuse reflectors
		this->registry.view<component::celestial_body, component::orbit, component::diffuse_reflector, component::transform>().each(
		[&](entity::id entity_id, const auto& reflector_body, const auto& reflector_orbit, const auto& reflector, const auto& transform)
		{
			// Transform reflector position from ICRF frame to EUS frame
			const double3 reflector_position_eus = icrf_to_eus * reflector_orbit.position;
			
			// Measure distance and direction, in EUS frame, from observer to reflector
			const double observer_reflector_distance = math::length(reflector_position_eus);
			const double3 observer_reflector_direction_eus = reflector_position_eus / observer_reflector_distance;
			
			// Measure distance and direction, in EUS frame, from reflector to blackbody
			double3 reflector_blackbody_direction_eus = blackbody_position_eus - reflector_position_eus;
			const double reflector_blackbody_distance = math::length(reflector_blackbody_direction_eus);
			reflector_blackbody_direction_eus /= reflector_blackbody_distance;
			
			// Measure blackbody solid angle as seen by reflector
			const double reflector_blackbody_angular_radius = astro::angular_radius(blackbody_body.radius, reflector_blackbody_distance);
			const double reflector_blackbody_solid_angle = geom::solid_angle::cone(reflector_blackbody_angular_radius);
			
			// Calculate blackbody illuminance reaching reflector
			const double3 reflector_blackbody_illuminance = blackbody.luminance * reflector_blackbody_solid_angle;
			
			// Measure reflector solid angle as seen by observer
			const double observer_reflector_angular_radius = astro::angular_radius(reflector_body.radius, observer_reflector_distance);
			const double observer_reflector_solid_angle = geom::solid_angle::cone(observer_reflector_angular_radius);
			
			// Determine phase factor of reflector as seen by observer
			const double observer_reflector_phase_factor = dot(observer_reflector_direction_eus, -reflector_blackbody_direction_eus) * 0.5 + 0.5;
			
			// Measure observer reference body solid angle as seen by reflector
			const double reflector_observer_angular_radius = astro::angular_radius(reference_body->radius, observer_reflector_distance);
			const double reflector_observer_solid_angle = geom::solid_angle::cone(reflector_observer_angular_radius);
			
			// Determine phase factor of observer reference body as by reflector
			const double reflector_observer_phase_factor = dot(-observer_reflector_direction_eus, -observer_blackbody_direction_eus) * 0.5 + 0.5;
			
			// Calculate spectral transmittance between observer and reflector factor due to atmospheric extinction
			double3 observer_reflector_transmittance = {1, 1, 1};
			if (reference_atmosphere)
			{
				const geom::ray<double> ray = {{0, 0, 0}, observer_reflector_direction_eus};
				observer_reflector_transmittance = integrate_transmittance(*observer, *reference_body, *reference_atmosphere, ray);
			}
			
			// Measure luminance of observer reference body as seen by reflector
			const double3 reflector_observer_luminance = observer_blackbody_illuminance * reference_body->albedo * observer_reflector_transmittance * reflector_observer_phase_factor * math::inverse_pi<double>;
			
			// Measure illuminance from observer reference body reaching reflector
			const double3 reflector_observer_illuminance = reflector_observer_luminance * reflector_observer_solid_angle;
			
			// Measure luminance of reflector as seen by observer
			const double3 observer_reflector_luminance = (reflector_blackbody_illuminance * observer_reflector_phase_factor + reflector_observer_illuminance) * reflector.albedo * observer_reflector_transmittance * math::inverse_pi<double>;
			
			// Measure illuminance from reflector reaching observer
			const double3 observer_reflector_illuminance = observer_reflector_luminance * observer_reflector_solid_angle;
			
			if (this->sky_pass)
			{
				this->sky_pass->set_moon_position(transform.local.translation);
				this->sky_pass->set_moon_rotation(transform.local.rotation);
				this->sky_pass->set_moon_angular_radius(static_cast<float>(observer_reflector_angular_radius));
				this->sky_pass->set_moon_sunlight_direction(float3(-reflector_blackbody_direction_eus));
				this->sky_pass->set_moon_sunlight_illuminance(float3(reflector_blackbody_illuminance * observer_reflector_transmittance));
				this->sky_pass->set_moon_planetlight_direction(float3(observer_reflector_direction_eus));
				this->sky_pass->set_moon_planetlight_illuminance(float3(reflector_observer_illuminance * observer_reflector_transmittance));
				this->sky_pass->set_moon_illuminance(float3(observer_reflector_illuminance / observer_reflector_transmittance), float3(observer_reflector_illuminance));
			}
			
			if (this->moon_light)
			{
				const float3 reflector_up_eus = float3(icrf_to_eus.r * double3{0, 0, 1});
				
				this->moon_light->set_color(float3(observer_reflector_illuminance));
				this->moon_light->set_rotation
				(
					math::look_rotation
					(
						float3(-observer_reflector_direction_eus),
						reflector_up_eus
					)
				);
				
				// Bounce moon light
				bounce_illuminance += std::max(0.0, math::dot(bounce_normal, -observer_reflector_direction_eus)) * observer_reflector_illuminance * bounce_albedo;
			}
		});
	});
	
	if (bounce_light)
	{
		bounce_light->set_color(float3(bounce_illuminance));
	}
}

void astronomy::set_time(double t)
{
	time_days = t;
	time_centuries = time_days * physics::time::jd::centuries_per_day<double>;
}

void astronomy::set_time_scale(double scale)
{
	time_scale = scale;
}

void astronomy::set_observer(entity::id eid)
{
	if (observer_eid != eid)
	{
		observer_eid = eid;
		
		if (observer_eid != entt::null)
			observer_modified();
		else
			reference_body_eid = entt::null;
	}
}

void astronomy::set_transmittance_samples(std::size_t samples)
{
	transmittance_samples = samples;
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

void astronomy::set_bounce_light(scene::directional_light* light)
{
	bounce_light = light;
}

void astronomy::set_bounce_albedo(const double3& albedo)
{
	bounce_albedo = albedo;
}

void astronomy::set_starlight_illuminance(const double3& illuminance)
{
	starlight_illuminance = illuminance;
}

void astronomy::set_sky_pass(::render::sky_pass* pass)
{
	this->sky_pass = pass;
	
	if (sky_pass)
	{
		if (observer_eid != entt::null)
		{
			// Get pointer to observer
			const auto observer = registry.try_get<component::observer>(reference_body_eid);
			
			sky_pass->set_observer_elevation(static_cast<float>(observer->elevation));
		}
		
		if (reference_body_eid != entt::null)
		{
			// Get pointer to reference celestial body
			const auto reference_body = registry.try_get<component::celestial_body>(reference_body_eid);
			
			if (reference_body)
				sky_pass->set_planet_radius(static_cast<float>(reference_body->radius));
			else
				sky_pass->set_planet_radius(0.0f);
		}
	}
}

void astronomy::on_observer_modified(entity::registry& registry, entity::id entity_id)
{
	if (entity_id == observer_eid)
		observer_modified();
}

void astronomy::on_observer_destroyed(entity::registry& registry, entity::id entity_id)
{
	if (entity_id == observer_eid)
		observer_modified();
}

void astronomy::on_celestial_body_modified(entity::registry& registry, entity::id entity_id)
{
	if (entity_id == reference_body_eid)
		reference_body_modified();
}

void astronomy::on_celestial_body_destroyed(entity::registry& registry, entity::id entity_id)
{
	if (entity_id == reference_body_eid)
		reference_body_modified();
}

void astronomy::on_orbit_modified(entity::registry& registry, entity::id entity_id)
{
	if (entity_id == reference_body_eid)
		reference_orbit_modified();
}

void astronomy::on_orbit_destroyed(entity::registry& registry, entity::id entity_id)
{
	if (entity_id == reference_body_eid)
		reference_orbit_modified();
}

void astronomy::on_atmosphere_modified(entity::registry& registry, entity::id entity_id)
{
	if (entity_id == reference_body_eid)
		reference_atmosphere_modified();
}

void astronomy::on_atmosphere_destroyed(entity::registry& registry, entity::id entity_id)
{
	if (entity_id == reference_body_eid)
		reference_atmosphere_modified();
}

void astronomy::observer_modified()
{
	// Get pointer to observer component
	const auto observer = registry.try_get<component::observer>(observer_eid);
	
	if (observer)
	{
		if (reference_body_eid != observer->reference_body_eid)
		{
			// Reference body changed
			reference_body_eid = observer->reference_body_eid;
			reference_body_modified();
			reference_orbit_modified();
			reference_atmosphere_modified();
		}
		
		if (reference_body_eid != entt::null)
		{
			// Get pointer to reference celestial body
			const auto reference_body = registry.try_get<component::celestial_body>(reference_body_eid);
			
			// Update BCBF to EUS transformation
			if (reference_body)
				update_bcbf_to_eus(*observer, *reference_body);
		}
	
		// Upload observer elevation to sky pass
		if (sky_pass)
			sky_pass->set_observer_elevation(static_cast<float>(observer->elevation));
	}
}

void astronomy::reference_body_modified()
{
	// Get pointer to reference celestial body
	const auto reference_body = registry.try_get<component::celestial_body>(reference_body_eid);
	
	if (reference_body)
	{
		// Get pointer to observer
		const auto observer = registry.try_get<component::observer>(observer_eid);
		
		// Update BCBF to EUS transformation
		if (observer)
			update_bcbf_to_eus(*observer, *reference_body);
	}
	
	// Update reference celestial body-related sky pass parameters
	if (sky_pass)
	{
		if (reference_body)
			sky_pass->set_planet_radius(static_cast<float>(reference_body->radius));
		else
			sky_pass->set_planet_radius(0.0f);
	}
}

void astronomy::reference_orbit_modified()
{
	
}

void astronomy::reference_atmosphere_modified()
{

}

void astronomy::update_bcbf_to_eus(const game::component::observer& observer, const game::component::celestial_body& body)
{
	// Construct BCBF to EUS transformation
	bcbf_to_eus = physics::orbit::frame::bcbf::to_enu
	(
		body.radius + observer.elevation,
		observer.latitude,
		observer.longitude
	) * enu_to_eus;
}

void astronomy::update_icrf_to_eus(const game::component::celestial_body& body, const game::component::orbit& orbit)
{
	// Evaluate reference body orientation polynomials
	const double body_pole_ra = math::polynomial::horner(body.pole_ra.begin(), body.pole_ra.end(), time_centuries);
	const double body_pole_dec = math::polynomial::horner(body.pole_dec.begin(), body.pole_dec.end(), time_centuries);
	const double body_prime_meridian = math::polynomial::horner(body.prime_meridian.begin(), body.prime_meridian.end(), time_days);
	
	// Construct ICRF frame to BCBF transformation
	math::transformation::se3<double> icrf_to_bcbf = physics::orbit::frame::bci::to_bcbf
	(
		body_pole_ra,
		body_pole_dec,
		body_prime_meridian
	);
	icrf_to_bcbf.t = icrf_to_bcbf.r * -orbit.position;
	
	/// Construct ICRF to EUS transformation
	icrf_to_eus = icrf_to_bcbf * bcbf_to_eus;
	
	// Pass ICRF to EUS transformation to sky pass
	if (sky_pass)
	{
		// Upload topocentric frame to sky pass
		sky_pass->set_icrf_to_eus
		(
			math::transformation::se3<float>
			{
				float3(icrf_to_eus.t),
				math::type_cast<float>(icrf_to_eus.r)
			}
		);
	}
}

double3 astronomy::integrate_transmittance(const game::component::observer& observer, const game::component::celestial_body& body, const game::component::atmosphere& atmosphere, geom::ray<double> ray) const
{
	double3 transmittance = {1, 1, 1};
	
	// Make ray height relative to center of reference body
	ray.origin.y() += body.radius + observer.elevation;
	
	// Construct sphere representing upper limit of the atmosphere
	geom::sphere<double> atmosphere_sphere;
	atmosphere_sphere.center = {0, 0, 0};
	atmosphere_sphere.radius = body.radius + atmosphere.upper_limit;
	
	// Check for intersection between the ray and atmosphere
	auto intersection = geom::ray_sphere_intersection(ray, atmosphere_sphere);
	if (std::get<0>(intersection))
	{
		// Get point of intersection
		const double3 intersection_point = ray.extrapolate(std::get<2>(intersection));
		
		// Integrate optical of Rayleigh, Mie, and ozone particles
		const double optical_depth_r = physics::gas::atmosphere::optical_depth_exp(ray.origin, intersection_point, body.radius, atmosphere.rayleigh_scale_height, transmittance_samples);
		const double optical_depth_m = physics::gas::atmosphere::optical_depth_exp(ray.origin, intersection_point, body.radius, atmosphere.mie_scale_height, transmittance_samples);
		const double optical_depth_o = physics::gas::atmosphere::optical_depth_tri(ray.origin, intersection_point, body.radius, atmosphere.ozone_lower_limit, atmosphere.ozone_upper_limit, atmosphere.ozone_mode, transmittance_samples);
		
		// Calculate transmittance factor due to scattering and absorption
		const double3 extinction_r = atmosphere.rayleigh_scattering * optical_depth_r;
		const double extinction_m = atmosphere.mie_extinction * optical_depth_m;
		const double3 extinction_o = atmosphere.ozone_absorption * optical_depth_o;
		transmittance = extinction_r + double3{extinction_m, extinction_m, extinction_m} + extinction_o;
		transmittance.x() = std::exp(-transmittance.x());
		transmittance.y() = std::exp(-transmittance.y());
		transmittance.z() = std::exp(-transmittance.z());
	}
	
	return transmittance;
}

} // namespace system
} // namespace game
