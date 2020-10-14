/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "game/systems/solar-system.hpp"
#include "game/astronomy/celestial-coordinates.hpp"
#include "game/astronomy/celestial-mechanics.hpp"
#include "game/astronomy/celestial-time.hpp"
#include "game/components/orbit-component.hpp"
#include "game/components/transform-component.hpp"

using namespace ecs;

static constexpr double seconds_per_day = 24.0 * 60.0 * 60.0;

solar_system::solar_system(entt::registry& registry):
	entity_system(registry),
	julian_date(0.0),
	time_scale(1.0),
	latitude(0.0),
	longitude(0.0),
	altitude(0.0)
{}

void solar_system::update(double t, double dt)
{
	// Add scaled timestep to Julian date
	set_julian_date(julian_date + (dt * time_scale) / seconds_per_day);
	
	// Update horizontal (topocentric) positions of orbiting bodies
	registry.view<orbit_component, transform_component>().each(
	[&](auto entity, auto& orbit, auto& transform)
	{
		double a = orbit.a;
		double ec = orbit.ec;
		double w = orbit.w;
		double ma = orbit.ma;
		double i = orbit.i;
		double om = orbit.om;
		
		double3 ecliptic = ast::solve_kepler(a, ec, w, ma, i, om);
		double3 horizontal = ecliptic_to_horizontal * ecliptic;
		
		// Subtract Earth's radius (in AU), for positon of observer
		horizontal.z -= 4.25875e-5;
		
		// Transform into local right-handed coordinates
		double3 translation = ast::horizontal_to_right_handed * horizontal;
		double3x3 rotation = ast::horizontal_to_right_handed * ecliptic_to_horizontal;
		
		transform.local.translation = math::type_cast<float>(translation);
		transform.local.rotation = math::type_cast<float>(math::quaternion_cast(rotation));
	});
}

void solar_system::set_julian_date(double jd)
{
	julian_date = jd;
	
	// Recalculate obliquity of the ecliptic
	ecl = ast::approx_ecliptic_obliquity(julian_date);
	
	// Recalculate LMST
	lmst = ast::jd_to_lmst(julian_date, longitude);
	
	// Recalculate ecliptic to horizontal transformation matrix
	ecliptic_to_horizontal = ast::ecliptic_to_horizontal(ecl, latitude, lmst);
}

void solar_system::set_time_scale(double scale)
{
	time_scale = scale;
}

void solar_system::set_observer_location(double latitude, double longitude, double altitude)
{
	this->latitude = latitude;
	this->longitude = longitude;
	this->altitude = altitude;
	
	// Recalculate LMST
	lmst = ast::jd_to_lmst(julian_date, longitude);
	
	// Recalculate ecliptic to horizontal transformation matrix
	ecliptic_to_horizontal = ast::ecliptic_to_horizontal(ecl, latitude, lmst);
}
