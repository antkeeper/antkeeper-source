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

class sky_pass;

namespace ecs {

/**
 * Calculates apparent properties of celestial bodies relative to an observer (magnitude, angular radius, horizontal coordinates) and modifies their model component and/or light component to render them accordingly.
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
	 * Sets the location of the observer.
	 *
	 * @param location Spherical coordinates of the observer, in the ISO order of radial distance, polar angle (radians), and azimuthal angle (radians).
	 */
	void set_observer_location(const double3& location);
	
	/**
	 * Sets the obliquity of the ecliptic, a.k.a. axial tilt.
	 *
	 * @param angle Angle between the planet's rotational axis and its orbital axis, in radians.
	 */
	void set_obliquity(double angle);
	
	/**
	 * Sets the rotational speed of the observer's planet.
	 *
	 * @param speed Rotational speed, in radians per day.
	 */
	void set_axial_rotation_speed(double speed);
	
	/**
	 * Sets the axial rotation of the observer's planet when the universal time is `0.0`.
	 *
	 * @param angle Axial rotation angle, in radians.
	 */
	void set_axial_rotation_at_epoch(double angle);
	
	void set_sky_pass(sky_pass* pass);
	
	void set_sun_light(scene::directional_light* light);
	
private:
	/// Updates the axial rotation angle
	void update_axial_rotation();
	
	/// Updates the local sidereal time (LST) and dependent variables
	void update_sidereal_time();
	
	/// Updates the ecliptic-to-horizontal transformation matrix
	void update_ecliptic_to_horizontal();
	
	double universal_time;
	double days_per_timestep;
	double lst;
	double axial_rotation_at_epoch;
	double axial_rotation_speed;
	double axial_rotation;
	double3 observer_location;
	double obliquity;
	double3x3 ecliptic_to_horizontal;
	sky_pass* sky_pass;
	scene::directional_light* sun_light;
};

} // namespace ecs

#endif // ANTKEEPER_ECS_ASTRONOMY_SYSTEM_HPP
