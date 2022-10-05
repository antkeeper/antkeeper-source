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

#ifndef ANTKEEPER_ENTITY_SYSTEM_ASTRONOMY_HPP
#define ANTKEEPER_ENTITY_SYSTEM_ASTRONOMY_HPP

#include "entity/systems/updatable.hpp"
#include "entity/id.hpp"
#include "scene/directional-light.hpp"
#include "scene/ambient-light.hpp"
#include "utility/fundamental-types.hpp"
#include "math/se3.hpp"
#include "render/passes/sky-pass.hpp"
#include "entity/components/observer.hpp"
#include "entity/components/atmosphere.hpp"
#include "entity/components/celestial-body.hpp"
#include "entity/components/orbit.hpp"
#include "geom/ray.hpp"

namespace entity {
namespace system {

/**
 * Calculates apparent properties of celestial bodies as seen by an observer.
 */
class astronomy:
	public updatable
{
public:
	astronomy(entity::registry& registry);
	~astronomy();
	
	/**
	 * Adds the timestep `dt`, scaled by set time scale, to the current time, then calculates apparent properties of celestial bodies as seen by an observer.
	 *
	 * @param t Time, in seconds.
	 * @param dt Delta time, in seconds.
	 */
	virtual void update(double t, double dt);
	
	/**
	 * Sets the current time.
	 *
	 * @param t Time since epoch, in days.
	 */
	void set_time(double t);
	
	/**
	 * Sets the factor by which the timestep `dt` will be scaled before being added to the current time.
	 *
	 * @param scale Factor by which to scale the timestep.
	 */
	void set_time_scale(double scale);
	
	/**
	 * Sets the observer entity.
	 *
	 * @param eid Entity ID of the observer.
	 */
	void set_observer(entity::id eid);
	
	/**
	 * Sets the number of samples to take when integrating atmospheric transmittance.
	 *
	 * @param samples Number of integration samples.
	 */
	void set_transmittance_samples(std::size_t samples);
	
	void set_sun_light(scene::directional_light* light);
	void set_sky_light(scene::ambient_light* light);
	void set_moon_light(scene::directional_light* light);
	void set_bounce_light(scene::directional_light* light);
	void set_bounce_albedo(const double3& albedo);
	void set_starlight_illuminance(const double3& illuminance);
	void set_sky_pass(::render::sky_pass* pass);
	
private:
	void on_observer_modified(entity::registry& registry, entity::id entity_id, entity::component::observer& component);
	void on_observer_destroyed(entity::registry& registry, entity::id entity_id);
	void on_celestial_body_modified(entity::registry& registry, entity::id entity_id, entity::component::celestial_body& component);
	void on_celestial_body_destroyed(entity::registry& registry, entity::id entity_id);
	void on_orbit_modified(entity::registry& registry, entity::id entity_id, entity::component::orbit& component);
	void on_orbit_destroyed(entity::registry& registry, entity::id entity_id);
	void on_atmosphere_modified(entity::registry& registry, entity::id entity_id, entity::component::atmosphere& component);
	void on_atmosphere_destroyed(entity::registry& registry, entity::id entity_id);
	
	/// Called each time the observer is modified.
	void observer_modified();
	
	/// Called each time the celestial body of the reference body is modified.
	void reference_body_modified();
	
	/// Called each time the orbit of the reference body is modified.
	void reference_orbit_modified();
	
	/// Called each time the atmosphere of the reference body is modified.
	void reference_atmosphere_modified();

	/// Updates the BCBF to EUS transformation.
	void update_bcbf_to_eus(const entity::component::observer& observer, const entity::component::celestial_body& body);
	
	/// Updates the ICRF to EUS transformation.
	void update_icrf_to_eus(const entity::component::celestial_body& body, const entity::component::orbit& orbit);
	
	/**
	 * Integrates a transmittance factor due to atmospheric extinction along a ray.
	 *
	 * @param ray Ray to cast, in the EUS frame.
	 * @param samples Number of samples to integrate.
	 *
	 * @return Spectral transmittance factor.
	 */
	double3 integrate_transmittance(const entity::component::observer& observer, const entity::component::celestial_body& body, const entity::component::atmosphere& atmosphere, geom::ray<double> ray) const;
	
	/// Time since epoch, in days.
	double time_days;
	
	/// Time since epoch, in centuries.
	double time_centuries;
	
	/// Time scale.
	double time_scale;
	
	/// Number of transmittance integration samples.
	std::size_t transmittance_samples;
	
	/// Entity ID of the observer.
	entity::id observer_eid;
	
	/// Entity ID of the reference body.
	entity::id reference_body_eid;
	
	/// ENU to EUS transformation.
	math::transformation::se3<double> enu_to_eus;
	
	/// BCBF to EUS transformation.
	math::transformation::se3<double> bcbf_to_eus;
	
	/// ICRF to EUS tranformation.
	math::transformation::se3<double> icrf_to_eus;
	
	scene::directional_light* sun_light;
	scene::ambient_light* sky_light;
	scene::directional_light* moon_light;
	scene::directional_light* bounce_light;
	double3 bounce_albedo;
	::render::sky_pass* sky_pass;
	double3 starlight_illuminance;
};

} // namespace system
} // namespace entity

#endif // ANTKEEPER_ENTITY_SYSTEM_ASTRONOMY_HPP
