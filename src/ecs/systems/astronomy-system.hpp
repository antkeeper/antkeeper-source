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

#ifndef ANTKEEPER_ECS_ASTRONOMY_SYSTEM_HPP
#define ANTKEEPER_ECS_ASTRONOMY_SYSTEM_HPP

#include "entity-system.hpp"
#include "ecs/entity.hpp"
#include "scene/directional-light.hpp"
#include "utility/fundamental-types.hpp"
#include "physics/frame.hpp"
#include "renderer/passes/sky-pass.hpp"
#include "ecs/components/blackbody-component.hpp"
#include "ecs/components/atmosphere-component.hpp"

namespace ecs {

/**
 * Calculates apparent properties of celestial bodies relative to an observer.
 */
class astronomy_system:
	public entity_system
{
public:
	astronomy_system(ecs::registry& registry);
	
	/**
	 * Scales then adds the timestep `dt` to the current time, then recalculates the positions of celestial bodies.
	 *
	 * @param t Time, in seconds.
	 * @param dt Delta time, in seconds.
	 */
	virtual void update(double t, double dt);
	
	/**
	 * Sets the current universal time.
	 *
	 * @param time Universal time, in days.
	 */
	void set_universal_time(double time);
	
	/**
	 * Sets the factor by which the timestep `dt` will be scaled before being added to the current universal time.
	 *
	 * @param scale Factor by which to scale the timestep.
	 */
	void set_time_scale(double scale);
	
	/**
	 * Sets the reference body, from which observations are taking place.
	 *
	 * @param entity Entity of the reference body.
	 */
	void set_reference_body(ecs::entity entity);
	
	/**
	 * Sets the axial tilt of the reference body.
	 *
	 * @param angle Angle between the reference body's rotational axis and its orbital axis, in radians.
	 */
	void set_reference_body_axial_tilt(double angle);
	
	/**
	 * Sets the location of the observer using spherical coordinates in BCBF space.
	 *
	 * @param location Spherical coordinates of the observer, in reference body BCBF space, in the ISO order of radial distance, polar angle (radians), and azimuthal angle (radians).
	 */
	void set_observer_location(const double3& location);
	
	void set_sun_light(scene::directional_light* light);
	
	void set_sky_pass(sky_pass* pass);
	
private:
	void on_blackbody_construct(ecs::registry& registry, ecs::entity entity, ecs::blackbody_component& blackbody);
	void on_blackbody_replace(ecs::registry& registry, ecs::entity entity, ecs::blackbody_component& blackbody);
	
	void on_atmosphere_construct(ecs::registry& registry, ecs::entity entity, ecs::atmosphere_component& atmosphere);
	void on_atmosphere_replace(ecs::registry& registry, ecs::entity entity, ecs::atmosphere_component& atmosphere);

	
	double universal_time;
	double time_scale;
	ecs::entity reference_body;
	double reference_body_axial_tilt;
	double reference_body_axial_rotation;
	double3 observer_location;
	scene::directional_light* sun_light;
	sky_pass* sky_pass;
	
	physics::frame<double> inertial_to_bcbf;
	physics::frame<double> bcbf_to_topocentric;
	physics::frame<double> inertial_to_topocentric;
	physics::frame<double> sez_to_ezs;
	physics::frame<double> ezs_to_sez;
};

} // namespace ecs

#endif // ANTKEEPER_ECS_ASTRONOMY_SYSTEM_HPP
