/*
 * Copyright (C) 2023  Christopher J. Howard
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

#ifndef ANTKEEPER_GAME_ASTRONOMY_SYSTEM_HPP
#define ANTKEEPER_GAME_ASTRONOMY_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include <engine/entity/id.hpp>
#include <engine/scene/directional-light.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/se3.hpp>
#include <engine/render/passes/sky-pass.hpp>
#include "game/components/observer-component.hpp"
#include "game/components/atmosphere-component.hpp"
#include "game/components/celestial-body-component.hpp"
#include "game/components/orbit-component.hpp"
#include <engine/geom/primitives/ray.hpp>


/**
 * Calculates apparent properties of celestial bodies as seen by an observer.
 */
class astronomy_system:
	public updatable_system
{
public:
	explicit astronomy_system(entity::registry& registry);
	~astronomy_system();
	
	/**
	 * Adds the timestep `dt`, scaled by set time scale, to the current time, then calculates apparent properties of celestial bodies as seen by an observer.
	 *
	 * @param t Time, in seconds.
	 * @param dt Delta time, in seconds.
	 */
	virtual void update(float t, float dt);
	
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
	void set_moon_light(scene::directional_light* light);
	void set_starlight_illuminance(const math::dvec3& illuminance);
	void set_sky_pass(::render::sky_pass* pass);
	
	[[nodiscard]] inline double get_time() const noexcept
	{
		return time_days;
	}
	
private:
	void on_observer_modified(entity::registry& registry, entity::id entity_id);
	void on_observer_destroyed(entity::registry& registry, entity::id entity_id);
	void on_celestial_body_modified(entity::registry& registry, entity::id entity_id);
	void on_celestial_body_destroyed(entity::registry& registry, entity::id entity_id);
	void on_orbit_modified(entity::registry& registry, entity::id entity_id);
	void on_orbit_destroyed(entity::registry& registry, entity::id entity_id);
	void on_atmosphere_modified(entity::registry& registry, entity::id entity_id);
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
	void update_bcbf_to_eus(const ::observer_component& observer, const ::celestial_body_component& body);
	
	/// Updates the ICRF to EUS transformation.
	void update_icrf_to_eus(const ::celestial_body_component& body, const ::orbit_component& orbit);
	
	/**
	 * Integrates a transmittance factor due to atmospheric extinction along a ray.
	 *
	 * @param ray Ray to cast, in the EUS frame.
	 * @param samples Number of samples to integrate.
	 *
	 * @return Spectral transmittance factor.
	 */
	math::dvec3 integrate_transmittance(const ::observer_component& observer, const ::celestial_body_component& body, const ::atmosphere_component& atmosphere, geom::ray<double, 3> ray) const;
	
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
	scene::directional_light* moon_light;
	::render::sky_pass* sky_pass;
	math::dvec3 starlight_illuminance;
};


#endif // ANTKEEPER_GAME_ASTRONOMY_SYSTEM_HPP
