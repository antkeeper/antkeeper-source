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
#include "scene/camera.hpp"
#include "utility/fundamental-types.hpp"
#include "math/se3.hpp"
#include "render/passes/sky-pass.hpp"
#include "entity/components/atmosphere.hpp"
#include "entity/components/celestial-body.hpp"
#include "entity/components/orbit.hpp"

namespace entity {
namespace system {

/**
 * Calculates apparent properties of celestial bodies relative to an observer.
 */
class astronomy:
	public updatable
{
public:
	astronomy(entity::registry& registry);
	
	/**
	 * Scales then adds the timestep `dt` to the current time, then recalculates the positions of celestial bodies.
	 *
	 * @param t Time, in seconds.
	 * @param dt Delta time, in seconds.
	 */
	virtual void update(double t, double dt);
	
	/**
	 * Sets the current time.
	 *
	 * @param time Time, in days.
	 */
	void set_time(double time);
	
	/**
	 * Sets the factor by which the timestep `dt` will be scaled before being added to the current time.
	 *
	 * @param scale Factor by which to scale the timestep.
	 */
	void set_time_scale(double scale);
	
	/**
	 * Sets the reference body entity, from which observations are taking place.
	 *
	 * @param entity Entity of the reference body.
	 */
	void set_reference_body(entity::id entity_id);
	
	/**
	 * Sets the location of the observer using spherical coordinates in BCBF space.
	 *
	 * @param location Spherical coordinates of the observer, in reference body BCBF space, in the ISO order of altitude (meters), latitude (radians), and longitude (radians).
	 */
	void set_observer_location(const double3& location);
	
	void set_sun_light(scene::directional_light* light);
	void set_sky_light(scene::ambient_light* light);
	void set_moon_light(scene::directional_light* light);
	void set_camera(scene::camera* camera);
	void set_exposure_offset(float offset);
	
	inline float get_exposure_offset() const { return exposure_offset; };
	
	void set_sky_pass(::render::sky_pass* pass);
	
private:
	void on_celestial_body_construct(entity::registry& registry, entity::id entity_id, entity::component::celestial_body& celestial_body);
	void on_celestial_body_replace(entity::registry& registry, entity::id entity_id, entity::component::celestial_body& celestial_body);
	
	void update_bcbf_to_enu();

	double time;
	double time_scale;
	
	entity::id reference_entity;
	
	double3 observer_location;
	
	math::transformation::se3<double> icrf_to_bcbf;
	math::transformation::se3<double> bcbf_to_enu;
	math::transformation::se3<double> icrf_to_enu;
	math::transformation::se3<double> enu_to_eus;
	math::transformation::se3<double> icrf_to_eus;
	
	scene::directional_light* sun_light;
	scene::ambient_light* sky_light;
	scene::directional_light* moon_light;
	scene::camera* camera;
	::render::sky_pass* sky_pass;
	float exposure_offset;
};

} // namespace system
} // namespace entity

#endif // ANTKEEPER_ENTITY_SYSTEM_ASTRONOMY_HPP
