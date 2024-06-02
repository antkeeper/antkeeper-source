// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/astronomy-system.hpp"
#include <engine/astro/apparent-size.hpp>
#include "game/components/blackbody-component.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/diffuse-reflector-component.hpp"
#include <engine/geom/intersection.hpp>
#include <engine/geom/primitives/sphere.hpp>
#include <engine/color/color.hpp>
#include <engine/physics/orbit/orbit.hpp>
#include <engine/physics/time/ut1.hpp>
#include <engine/physics/time/jd.hpp>
#include <engine/physics/light/photometry.hpp>
#include <engine/physics/light/luminosity.hpp>
#include <engine/physics/light/refraction.hpp>
#include <engine/physics/gas/atmosphere.hpp>
#include <engine/astro/apparent-size.hpp>
#include <engine/geom/solid-angle.hpp>
#include <engine/math/transform.hpp>
#include <engine/math/polynomial.hpp>
#include <engine/debug/log.hpp>

astronomy_system::astronomy_system(entity::registry& registry):
	updatable_system(registry)
{
	// Construct ENU to EUS transformation
	m_enu_to_eus = math::se3<double>
	{
		{0, 0, 0},
		math::dquat::rotate_x(-math::half_pi<double>)
	};
	
	m_registry.on_construct<::observer_component>().connect<&astronomy_system::on_observer_modified>(this);
	m_registry.on_update<::observer_component>().connect<&astronomy_system::on_observer_modified>(this);
	m_registry.on_destroy<::observer_component>().connect<&astronomy_system::on_observer_destroyed>(this);
	m_registry.on_construct<::celestial_body_component>().connect<&astronomy_system::on_celestial_body_modified>(this);
	m_registry.on_update<::celestial_body_component>().connect<&astronomy_system::on_celestial_body_modified>(this);
	m_registry.on_destroy<::celestial_body_component>().connect<&astronomy_system::on_celestial_body_destroyed>(this);
	m_registry.on_construct<::orbit_component>().connect<&astronomy_system::on_orbit_modified>(this);
	m_registry.on_update<::orbit_component>().connect<&astronomy_system::on_orbit_modified>(this);
	m_registry.on_destroy<::orbit_component>().connect<&astronomy_system::on_orbit_destroyed>(this);
	m_registry.on_construct<::atmosphere_component>().connect<&astronomy_system::on_atmosphere_modified>(this);
	m_registry.on_update<::atmosphere_component>().connect<&astronomy_system::on_atmosphere_modified>(this);
	m_registry.on_destroy<::atmosphere_component>().connect<&astronomy_system::on_atmosphere_destroyed>(this);
}

astronomy_system::~astronomy_system()
{
	m_registry.on_construct<::observer_component>().disconnect<&astronomy_system::on_observer_modified>(this);
	m_registry.on_update<::observer_component>().disconnect<&astronomy_system::on_observer_modified>(this);
	m_registry.on_destroy<::observer_component>().disconnect<&astronomy_system::on_observer_destroyed>(this);
	m_registry.on_construct<::celestial_body_component>().disconnect<&astronomy_system::on_celestial_body_modified>(this);
	m_registry.on_update<::celestial_body_component>().disconnect<&astronomy_system::on_celestial_body_modified>(this);
	m_registry.on_destroy<::celestial_body_component>().disconnect<&astronomy_system::on_celestial_body_destroyed>(this);
	m_registry.on_construct<::orbit_component>().disconnect<&astronomy_system::on_orbit_modified>(this);
	m_registry.on_update<::orbit_component>().disconnect<&astronomy_system::on_orbit_modified>(this);
	m_registry.on_destroy<::orbit_component>().disconnect<&astronomy_system::on_orbit_destroyed>(this);
	m_registry.on_construct<::atmosphere_component>().disconnect<&astronomy_system::on_atmosphere_modified>(this);
	m_registry.on_update<::atmosphere_component>().disconnect<&astronomy_system::on_atmosphere_modified>(this);
	m_registry.on_destroy<::atmosphere_component>().disconnect<&astronomy_system::on_atmosphere_destroyed>(this);
}

void astronomy_system::update([[maybe_unused]] float t, float dt)
{
	// Add scaled timestep to current time
	set_time(m_time_days + dt * m_time_scale);
	
	// Abort if no valid observer entity or reference body entity
	if (m_observer_eid == entt::null || m_reference_body_eid == entt::null)
		return;
	
	// Get pointer to observer component
	const auto observer = m_registry.try_get<observer_component>(m_observer_eid);
	
	// Abort if no observer component
	if (!observer)
		return;
	
	// Get pointers to reference body components
	const auto
	[
		reference_body,
		reference_orbit,
		reference_atmosphere
	] = m_registry.try_get<celestial_body_component, orbit_component, atmosphere_component>(m_reference_body_eid);
	
	// Abort if no reference body or reference orbit
	if (!reference_body || !reference_orbit)
		return;
	
	// if (m_sky_pass)
	// {
		// m_sky_pass->set_ground_albedo(math::fvec3{1.0f, 1.0f, 1.0f} * static_cast<float>(reference_body->albedo));
	// }
	
	// Update ICRF to EUS transformation
	update_icrf_to_eus(*reference_body, *reference_orbit);
	
	// Set the transform component translations of orbiting bodies to their topocentric positions
	m_registry.view<celestial_body_component, orbit_component, transform_component>().each
	(
		[&](entity::id entity_id, const auto& body, const auto& orbit, auto& transform)
		{
			// Skip reference body entity
			if (entity_id == m_reference_body_eid || orbit.parent == entt::null)
				return;
			
			// Transform orbital Cartesian position (r) from the ICRF frame to the EUS frame
			const math::dvec3 r_eus = m_icrf_to_eus * orbit.position;
			
			// Evaluate body orientation polynomials
			const double body_pole_ra = math::horner(body.pole_ra.begin(), body.pole_ra.end(), m_time_centuries);
			const double body_pole_dec = math::horner(body.pole_dec.begin(), body.pole_dec.end(), m_time_centuries);
			const double body_prime_meridian = math::horner(body.prime_meridian.begin(), body.prime_meridian.end(), m_time_days);
			
			// Determine body orientation in the ICRF frame
			math::dquat rotation_icrf = physics::orbit::frame::bcbf::to_bci
			(
				body_pole_ra,
				body_pole_dec,
				body_prime_meridian
			).r;
			
			// Transform body orientation from the ICRF frame to the EUS frame.
			math::dquat rotation_eus = math::normalize(m_icrf_to_eus.r * rotation_icrf);
			
			// Update local transform
			transform.local.translation = math::normalize(math::fvec3(r_eus));
			transform.local.rotation = math::fquat(rotation_eus);
			transform.local.scale = {1.0f, 1.0f, 1.0f};
		}
	);
	
	// Update blackbody lighting
	m_registry.view<celestial_body_component, orbit_component, blackbody_component>().each(
	[&]([[maybe_unused]] entity::id entity_id, const auto& blackbody_body, const auto& blackbody_orbit, const auto& blackbody)
	{
		// Transform blackbody position from ICRF frame to EUS frame
		const math::dvec3 blackbody_position_eus = m_icrf_to_eus * blackbody_orbit.position;
		
		// Measure distance and direction, in EUS frame, from observer to blackbody
		const double observer_blackbody_distance = math::length(blackbody_position_eus);
		const math::dvec3 observer_blackbody_direction_eus = blackbody_position_eus / observer_blackbody_distance;
		
		// Measure blackbody solid angle as seen by observer
		const double observer_blackbody_angular_radius = astro::angular_radius(blackbody_body.radius, observer_blackbody_distance);
		const double observer_blackbody_solid_angle = geom::solid_angle::cone(observer_blackbody_angular_radius);
		
		// Calculate illuminance from blackbody reaching observer
		const math::dvec3 observer_blackbody_illuminance = blackbody.color * blackbody.luminance * observer_blackbody_solid_angle;
		
		// Calculate illuminance from blackbody reaching observer after atmospheric extinction
		math::dvec3 observer_blackbody_transmitted_illuminance = observer_blackbody_illuminance;
		if (reference_atmosphere)
		{
			// Construct ray at observer pointing towards the blackbody
			const geom::ray<double, 3> ray = {{0, 0, 0}, observer_blackbody_direction_eus};
			
			// Integrate atmospheric spectral transmittance factor between observer and blackbody
			math::dvec3 transmittance = integrate_transmittance(*observer, *reference_body, *reference_atmosphere, ray);

			// Attenuate illuminance from blackbody reaching observer by spectral transmittance factor
			observer_blackbody_transmitted_illuminance *= transmittance;
		}
		
		// Update sun light
		if (m_sun_light != nullptr)
		{
			const math::dvec3 blackbody_up_eus = m_icrf_to_eus.r * math::dvec3{0, 0, 1};
			m_sun_light->set_rotation
			(
				math::look_rotation
				(
					math::fvec3(-observer_blackbody_direction_eus),
					math::fvec3(blackbody_up_eus)
				)
			);
			
			m_sun_light->set_illuminance(static_cast<float>(math::max_element(observer_blackbody_transmitted_illuminance)));
			
			const auto max_component = math::max_element(observer_blackbody_transmitted_illuminance);
			if (max_component > 0.0)
			{
				m_sun_light->set_color(math::fvec3(observer_blackbody_transmitted_illuminance / max_component));
			}
			else
			{
				m_sun_light->set_color({});
			}
		}
		
		// Upload blackbody params to sky pass
		if (m_sky_pass)
		{
			m_sky_pass->set_sun_position(math::fvec3(blackbody_position_eus));
			m_sky_pass->set_sun_luminance(math::fvec3(blackbody.color * blackbody.luminance));
			m_sky_pass->set_sun_illuminance(math::fvec3(observer_blackbody_illuminance), math::fvec3(observer_blackbody_transmitted_illuminance));
			m_sky_pass->set_sun_angular_radius(static_cast<float>(observer_blackbody_angular_radius));
		}
		
		// Update diffuse reflectors
		m_registry.view<celestial_body_component, orbit_component, diffuse_reflector_component, transform_component>().each(
		[&]([[maybe_unused]] entity::id entity_id, const auto& reflector_body, const auto& reflector_orbit, const auto& reflector, const auto& transform)
		{
			// Transform reflector position from ICRF frame to EUS frame
			const math::dvec3 reflector_position_eus = m_icrf_to_eus * reflector_orbit.position;
			
			// Measure distance and direction, in EUS frame, from observer to reflector
			const double observer_reflector_distance = math::length(reflector_position_eus);
			const math::dvec3 observer_reflector_direction_eus = reflector_position_eus / observer_reflector_distance;
			
			// Measure distance and direction, in EUS frame, from reflector to blackbody
			math::dvec3 reflector_blackbody_direction_eus = blackbody_position_eus - reflector_position_eus;
			const double reflector_blackbody_distance = math::length(reflector_blackbody_direction_eus);
			reflector_blackbody_direction_eus /= reflector_blackbody_distance;
			
			// Measure blackbody solid angle as seen by reflector
			const double reflector_blackbody_angular_radius = astro::angular_radius(blackbody_body.radius, reflector_blackbody_distance);
			const double reflector_blackbody_solid_angle = geom::solid_angle::cone(reflector_blackbody_angular_radius);
			
			// Calculate blackbody illuminance reaching reflector
			const math::dvec3 reflector_blackbody_illuminance = blackbody.color * blackbody.luminance * reflector_blackbody_solid_angle;
			
			// Measure reflector solid angle as seen by observer
			const double observer_reflector_angular_radius = astro::angular_radius(reflector_body.radius, observer_reflector_distance);
			const double observer_reflector_solid_angle = geom::solid_angle::cone(observer_reflector_angular_radius);
			
			// Determine phase factor of reflector as seen by observer
			const double observer_reflector_phase_factor = math::dot(observer_reflector_direction_eus, -reflector_blackbody_direction_eus) * 0.5 + 0.5;
			
			// Measure observer reference body solid angle as seen by reflector
			const double reflector_observer_angular_radius = astro::angular_radius(reference_body->radius, observer_reflector_distance);
			const double reflector_observer_solid_angle = geom::solid_angle::cone(reflector_observer_angular_radius);
			
			// Determine phase factor of observer reference body as by reflector
			const double reflector_observer_phase_factor = math::dot(-observer_reflector_direction_eus, -observer_blackbody_direction_eus) * 0.5 + 0.5;
			
			// Calculate spectral transmittance between observer and reflector factor due to atmospheric extinction
			math::dvec3 observer_reflector_transmittance = {1, 1, 1};
			if (reference_atmosphere)
			{
				// const geom::ray<double, 3> ray = {{0, 0, 0}, observer_reflector_direction_eus};
				// observer_reflector_transmittance = integrate_transmittance(*observer, *reference_body, *reference_atmosphere, ray);
			}
			
			// Measure luminance of observer reference body as seen by reflector
			const math::dvec3 reflector_observer_luminance = observer_blackbody_illuminance * reference_body->albedo * observer_reflector_transmittance * reflector_observer_phase_factor * math::inv_pi<double>;
			
			// Measure illuminance from observer reference body reaching reflector
			const math::dvec3 reflector_observer_illuminance = reflector_observer_luminance * reflector_observer_solid_angle;
			
			// Measure luminance of reflector as seen by observer
			const math::dvec3 observer_reflector_luminance = (reflector_blackbody_illuminance * observer_reflector_phase_factor + reflector_observer_illuminance) * reflector.albedo * observer_reflector_transmittance * math::inv_pi<double>;
			
			// Measure illuminance from reflector reaching observer
			const math::dvec3 observer_reflector_illuminance = observer_reflector_luminance * observer_reflector_solid_angle;
			
			if (m_sky_pass)
			{
				m_sky_pass->set_moon_position(transform.local.translation);
				m_sky_pass->set_moon_rotation(transform.local.rotation);
				m_sky_pass->set_moon_angular_radius(static_cast<float>(observer_reflector_angular_radius));
				m_sky_pass->set_moon_sunlight_direction(math::fvec3(-reflector_blackbody_direction_eus));
				m_sky_pass->set_moon_sunlight_illuminance(math::fvec3(reflector_blackbody_illuminance * observer_reflector_transmittance));
				m_sky_pass->set_moon_planetlight_direction(math::fvec3(observer_reflector_direction_eus));
				m_sky_pass->set_moon_planetlight_illuminance(math::fvec3(reflector_observer_illuminance * observer_reflector_transmittance));
				m_sky_pass->set_moon_illuminance(math::fvec3(observer_reflector_illuminance / observer_reflector_transmittance), math::fvec3(observer_reflector_illuminance));
			}
			
			if (m_moon_light)
			{
				const math::fvec3 reflector_up_eus = math::fvec3(m_icrf_to_eus.r * math::dvec3{0, 0, 1});
				
				m_moon_light->set_illuminance(static_cast<float>(math::max_element(observer_reflector_illuminance)));
				
				const auto max_component = math::max_element(observer_reflector_illuminance);
				if (max_component > 0.0)
				{
					m_moon_light->set_color(math::fvec3(observer_reflector_illuminance / max_component));
				}
				else
				{
					m_moon_light->set_color({});
				}
				
				m_moon_light->set_rotation
				(
					math::look_rotation
					(
						math::fvec3(-observer_reflector_direction_eus),
						reflector_up_eus
					)
				);
			}
		});
	});
}

void astronomy_system::set_time(double t)
{
	m_time_days = t;
	m_time_centuries = m_time_days * physics::time::jd::centuries_per_day<double>;
}

void astronomy_system::set_time_scale(double scale)
{
	m_time_scale = scale;
}

void astronomy_system::set_observer(entity::id eid)
{
	if (m_observer_eid != eid)
	{
		m_observer_eid = eid;
		
		if (m_observer_eid != entt::null)
			observer_modified();
		else
			m_reference_body_eid = entt::null;
	}
}

void astronomy_system::set_transmittance_samples(std::size_t samples)
{
	m_transmittance_samples = samples;
}

void astronomy_system::set_sun_light(scene::directional_light* light)
{
	m_sun_light = light;
}

void astronomy_system::set_moon_light(scene::directional_light* light)
{
	m_moon_light = light;
}

void astronomy_system::set_starlight_illuminance(const math::dvec3& illuminance)
{
	m_starlight_illuminance = illuminance;
}

void astronomy_system::set_sky_pass(::render::sky_pass* pass)
{
	m_sky_pass = pass;
	
	if (m_sky_pass)
	{
		if (m_observer_eid != entt::null)
		{
			// Get pointer to observer
			const auto observer = m_registry.try_get<observer_component>(m_reference_body_eid);
			
			m_sky_pass->set_observer_elevation(static_cast<float>(observer->elevation));
		}
		
		if (m_reference_body_eid != entt::null)
		{
			// Get pointer to reference celestial body
			const auto reference_body = m_registry.try_get<celestial_body_component>(m_reference_body_eid);
			
			if (reference_body)
			{
				m_sky_pass->set_planet_radius(static_cast<float>(reference_body->radius));
				// m_sky_pass->set_ground_albedo(math::fvec3{1.0f, 1.0f, 1.0f} * static_cast<float>(reference_body->albedo));
			}
			else
			{
				m_sky_pass->set_planet_radius(0.0f);
				// m_sky_pass->set_ground_albedo({0.0f, 0.0f, 0.0f});
			}
		}
	}
}

void astronomy_system::on_observer_modified([[maybe_unused]] entity::registry& registry, entity::id entity_id)
{
	if (entity_id == m_observer_eid)
		observer_modified();
}

void astronomy_system::on_observer_destroyed([[maybe_unused]] entity::registry& registry, entity::id entity_id)
{
	if (entity_id == m_observer_eid)
		observer_modified();
}

void astronomy_system::on_celestial_body_modified([[maybe_unused]] entity::registry& registry, entity::id entity_id)
{
	if (entity_id == m_reference_body_eid)
		reference_body_modified();
}

void astronomy_system::on_celestial_body_destroyed([[maybe_unused]] entity::registry& registry, entity::id entity_id)
{
	if (entity_id == m_reference_body_eid)
		reference_body_modified();
}

void astronomy_system::on_orbit_modified([[maybe_unused]] entity::registry& registry, entity::id entity_id)
{
	if (entity_id == m_reference_body_eid)
		reference_orbit_modified();
}

void astronomy_system::on_orbit_destroyed([[maybe_unused]] entity::registry& registry, entity::id entity_id)
{
	if (entity_id == m_reference_body_eid)
		reference_orbit_modified();
}

void astronomy_system::on_atmosphere_modified([[maybe_unused]] entity::registry& registry, entity::id entity_id)
{
	if (entity_id == m_reference_body_eid)
		reference_atmosphere_modified();
}

void astronomy_system::on_atmosphere_destroyed([[maybe_unused]] entity::registry& registry, entity::id entity_id)
{
	if (entity_id == m_reference_body_eid)
		reference_atmosphere_modified();
}

void astronomy_system::observer_modified()
{
	// Get pointer to observer component
	const auto observer = m_registry.try_get<observer_component>(m_observer_eid);
	
	if (observer)
	{
		if (m_reference_body_eid != observer->reference_body_eid)
		{
			// Reference body changed
			m_reference_body_eid = observer->reference_body_eid;
			reference_body_modified();
			reference_orbit_modified();
			reference_atmosphere_modified();
		}
		
		if (m_reference_body_eid != entt::null)
		{
			// Get pointer to reference celestial body
			const auto reference_body = m_registry.try_get<celestial_body_component>(m_reference_body_eid);
			
			// Update BCBF to EUS transformation
			if (reference_body)
				update_bcbf_to_eus(*observer, *reference_body);
		}
	
		// Upload observer elevation to sky pass
		if (m_sky_pass)
			m_sky_pass->set_observer_elevation(static_cast<float>(observer->elevation));
	}
}

void astronomy_system::reference_body_modified()
{
	// Get pointer to reference celestial body
	const auto reference_body = m_registry.try_get<celestial_body_component>(m_reference_body_eid);
	
	if (reference_body)
	{
		// Get pointer to observer
		const auto observer = m_registry.try_get<observer_component>(m_observer_eid);
		
		// Update BCBF to EUS transformation
		if (observer)
			update_bcbf_to_eus(*observer, *reference_body);
	}
	
	// Update reference celestial body-related sky pass parameters
	if (m_sky_pass)
	{
		if (reference_body)
			m_sky_pass->set_planet_radius(static_cast<float>(reference_body->radius));
		else
			m_sky_pass->set_planet_radius(0.0f);
	}
}

void astronomy_system::reference_orbit_modified()
{
	
}

void astronomy_system::reference_atmosphere_modified()
{

}

void astronomy_system::update_bcbf_to_eus(const ::observer_component& observer, const ::celestial_body_component& body)
{
	// Construct BCBF to EUS transformation
	m_bcbf_to_eus = physics::orbit::frame::bcbf::to_enu
	(
		body.radius + observer.elevation,
		observer.latitude,
		observer.longitude
	) * m_enu_to_eus;
}

void astronomy_system::update_icrf_to_eus(const ::celestial_body_component& body, const ::orbit_component& orbit)
{
	// Evaluate reference body orientation polynomials
	const double body_pole_ra = math::horner(body.pole_ra.begin(), body.pole_ra.end(), m_time_centuries);
	const double body_pole_dec = math::horner(body.pole_dec.begin(), body.pole_dec.end(), m_time_centuries);
	const double body_prime_meridian = math::horner(body.prime_meridian.begin(), body.prime_meridian.end(), m_time_days);
	
	// Construct ICRF frame to BCBF transformation
	math::se3<double> icrf_to_bcbf = physics::orbit::frame::bci::to_bcbf
	(
		body_pole_ra,
		body_pole_dec,
		body_prime_meridian
	);
	icrf_to_bcbf.t = icrf_to_bcbf.r * -orbit.position;
	
	/// Construct ICRF to EUS transformation
	m_icrf_to_eus = icrf_to_bcbf * m_bcbf_to_eus;
	
	// Pass ICRF to EUS transformation to sky pass
	if (m_sky_pass)
	{
		// Upload topocentric frame to sky pass
		m_sky_pass->set_icrf_to_eus
		(
			math::se3<float>
			{
				math::fvec3(m_icrf_to_eus.t),
				math::fquat(m_icrf_to_eus.r)
			}
		);
	}
}

math::dvec3 astronomy_system::integrate_transmittance(const ::observer_component& observer, const ::celestial_body_component& body, const ::atmosphere_component& atmosphere, geom::ray<double, 3> ray) const
{
	math::dvec3 transmittance = {1, 1, 1};
	
	// Make ray height relative to center of reference body
	ray.origin.y() += body.radius + observer.elevation;
	
	// Construct sphere representing upper limit of the atmosphere
	geom::sphere<double> atmosphere_sphere;
	atmosphere_sphere.center = {0, 0, 0};
	atmosphere_sphere.radius = body.radius + atmosphere.upper_limit;
	
	// Check for intersection between the ray and atmosphere
	auto intersection = geom::intersection(ray, atmosphere_sphere);
	if (intersection && std::get<1>(*intersection) > 0.0)
	{
		// Determine height at ray origin and cosine of the angle between the ray direction and local zenith direction at ray origin
		const double height = math::length(ray.origin);
		const double cos_view_zenith = math::dot(ray.direction, ray.origin) / height;
		
		// Precalculate terms re-used in sample height calculation
		const double sqr_height = height * height;
		const double height_cos_view_zenith = height * cos_view_zenith;
		const double two_height_cos_view_zenith = 2.0 * height_cos_view_zenith;
		
		// Get distance to upper limit of atmosphere
		const double sample_end_distance = std::get<1>(*intersection);
		
		// Integrate atmospheric particle densities
		math::dvec3 densities{};
		double previous_sample_distance = 0.0;
		for (std::size_t i = 0; i < m_transmittance_samples; ++i)
		{
			// Determine distance along sample ray to sample point and length of the sample
			const double sample_distance = (static_cast<double>(i) + 0.5) / static_cast<double>(m_transmittance_samples) * sample_end_distance;
			const double sample_length = sample_distance - previous_sample_distance;
			previous_sample_distance = sample_distance;
			
			// Calculate sample elevation
			const double sample_height = std::sqrt(sample_distance * sample_distance + sqr_height + two_height_cos_view_zenith * sample_distance);
			const double sample_elevation = sample_height - body.radius;
			
			// Weigh and sum atmospheric particle densities at sample elevation
			densities.x() += physics::gas::atmosphere::density::exponential(1.0, sample_elevation, atmosphere.rayleigh_scale_height) * sample_length;
			densities.y() += physics::gas::atmosphere::density::exponential(1.0, sample_elevation, atmosphere.mie_scale_height) * sample_length;
			densities.x() += physics::gas::atmosphere::density::triangular(1.0, sample_elevation, atmosphere.ozone_lower_limit, atmosphere.ozone_upper_limit, atmosphere.ozone_mode) * sample_length;
		}
		
		// Calculate extinction coefficients from integrated atmospheric particle densities
		const math::dvec3 extinction = densities.x() * atmosphere.rayleigh_scattering +
			densities.y() * atmosphere.mie_extinction +
			densities.z() * atmosphere.ozone_absorption;
		
		// Calculate transmittance factor from extinction coefficients
		transmittance = {std::exp(-extinction.x()), std::exp(-extinction.y()), std::exp(-extinction.z())};
	}
	
	// Scatter in BT.709, then convert to BT.2020
	return color::bt2020<double>.xyz_to_rgb(color::bt709<double>.rgb_to_xyz(transmittance));
}
