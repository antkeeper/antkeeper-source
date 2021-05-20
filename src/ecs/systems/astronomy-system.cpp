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

#include "ecs/systems/astronomy-system.hpp"
#include "astro/apparent-size.hpp"
#include "ecs/components/orbit-component.hpp"
#include "ecs/components/blackbody-component.hpp"
#include "ecs/components/atmosphere-component.hpp"
#include "ecs/components/transform-component.hpp"
#include "color/color.hpp"
#include "physics/orbit/orbit.hpp"
#include "physics/time/ut1.hpp"
#include "geom/cartesian.hpp"
#include <iostream>

namespace ecs {

astronomy_system::astronomy_system(ecs::registry& registry):
	entity_system(registry),
	universal_time(0.0),
	time_scale(1.0),
	reference_body(entt::null),
	reference_body_axial_tilt(0.0),
	reference_body_axial_rotation(0.0),
	sun_light(nullptr),
	sky_pass(nullptr)
{}

void astronomy_system::update(double t, double dt)
{
	// Add scaled timestep to current time
	set_universal_time(universal_time + dt * time_scale);
	
	// Abort if reference body has not been set
	if (reference_body == entt::null)
		return;
	
	// Abort if reference body has no orbit component
	if (!registry.has<ecs::orbit_component>(reference_body))
		return;
	
	// Update axial rotation of reference body
	reference_body_axial_rotation = physics::time::ut1::era(universal_time);
	
	// Get orbit component of reference body
	const auto& reference_orbit = registry.get<ecs::orbit_component>(reference_body);
	
	/// Construct reference frame which transforms coordinates from inertial space to reference body BCBF space
	inertial_to_bcbf = physics::orbit::inertial::to_bcbf
	(
		reference_orbit.state.r,
		reference_orbit.elements.i,
		reference_body_axial_tilt,
		reference_body_axial_rotation
	);
	
	/// Construct reference frame which transforms coordinates from inertial space to reference body topocentric space
	inertial_to_topocentric = inertial_to_bcbf * bcbf_to_topocentric;
	
	// Set the transform component translations of orbiting bodies to their topocentric positions
	registry.view<orbit_component, transform_component>().each(
	[&](ecs::entity entity, auto& orbit, auto& transform)
	{
		// Transform Cartesian position vector (r) from inertial space to topocentric space
		const math::vector3<double> r_topocentric = inertial_to_topocentric * orbit.state.r;
		
		// Update local transform
		transform.local.translation = math::type_cast<float>(r_topocentric);
	});
	
	// Get atmosphere component of reference body, if any
	if (registry.has<ecs::atmosphere_component>(reference_body))
	{
		const ecs::atmosphere_component& atmosphere = registry.get<ecs::atmosphere_component>(reference_body);
	}
	
	if (sun_light != nullptr)
	{
		const math::vector3<double> sun_position_inertial = {0, 0, 0};
		const math::vector3<double> sun_forward_inertial = math::normalize(reference_orbit.state.r - sun_position_inertial);
		const math::vector3<double> sun_up_inertial = {0, 0, 1};
		
		// Transform sun position, forward, and up vectors into topocentric space
		const math::vector3<double> sun_position_topocentric = inertial_to_topocentric * sun_position_inertial;
		const math::vector3<double> sun_forward_topocentric = inertial_to_topocentric.rotation * sun_forward_inertial;
		const math::vector3<double> sun_up_topocentric = inertial_to_topocentric.rotation * sun_up_inertial;
		
		// Update sun light transform
		sun_light->set_translation(math::type_cast<float>(sun_position_topocentric));
		sun_light->set_rotation
		(
			math::look_rotation
			(
				math::type_cast<float>(sun_forward_topocentric),
				math::type_cast<float>(sun_up_topocentric)
			)
		);
		
		// Convert sun topocentric Cartesian coordinates to spherical coordinates
		math::vector3<double> sun_az_el = geom::cartesian::to_spherical(ezs_to_sez * sun_position_topocentric);
		sun_az_el.z = math::pi<double> - sun_az_el.z;
		
		//std::cout << "el: " << math::degrees(sun_az_el.y) << "; az: " << math::degrees(sun_az_el.z) << std::endl;
		
		// Calculate sun color
		float cct = 3000.0f + std::sin(sun_az_el.y) * 5000.0f;
		float3 color_xyz = color::cct::to_xyz(cct);
		float3 color_acescg = color::xyz::to_acescg(color_xyz);
		sun_light->set_color(color_acescg);

		// Calculate sun intensity (in lux)
		const float illuminance_zenith = 108000.0f;
		float illuminance = std::max(0.0, std::sin(sun_az_el.y) * illuminance_zenith);
		sun_light->set_intensity(illuminance);
	}
	
	if (sky_pass != nullptr)
	{
		sky_pass->set_topocentric_frame
		(
			physics::frame<float>
			{
				math::type_cast<float>(inertial_to_topocentric.translation),
				math::type_cast<float>(inertial_to_topocentric.rotation)
			}
		);
		
		sky_pass->set_sun_object(sun_light);
	}
}

void astronomy_system::set_universal_time(double time)
{
	universal_time = time;
}

void astronomy_system::set_time_scale(double scale)
{
	time_scale = scale;
}

void astronomy_system::set_reference_body(ecs::entity entity)
{
	reference_body = entity;
}

void astronomy_system::set_reference_body_axial_tilt(double angle)
{
	reference_body_axial_tilt = angle;
}

void astronomy_system::set_observer_location(const double3& location)
{
	observer_location = location;
	
	// Construct reference frame which transforms coordinates from SEZ to EZS
	sez_to_ezs = physics::frame<double>
	{
		{0, 0, 0},
		math::normalize
		(
			math::quaternion<double>::rotate_x(-math::half_pi<double>) *
				math::quaternion<double>::rotate_z(-math::half_pi<double>)
		)
	};
	
	// Construct reference frame which transforms coordinates from EZS to SEZ
	ezs_to_sez = sez_to_ezs.inverse();
	
	// Construct reference frame which transforms coordinates from BCBF space to topocentric space
	bcbf_to_topocentric = physics::orbit::bcbf::to_topocentric
	(
		observer_location[0], // Radial distance
		observer_location[1], // Latitude
		observer_location[2]  // Longitude
	) * sez_to_ezs;
}

void astronomy_system::set_sun_light(scene::directional_light* light)
{
	sun_light = light;
}

void astronomy_system::set_sky_pass(::sky_pass* pass)
{
	this->sky_pass = pass;
}

} // namespace ecs
