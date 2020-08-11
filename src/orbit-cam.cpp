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

#include "orbit-cam.hpp"
#include "scene/camera.hpp"
#include "math/math.hpp"
#include <algorithm>
#include <cmath>

orbit_cam::orbit_cam():
	elevation_rotation(math::identity_quaternion<float>),
	azimuth_rotation(math::identity_quaternion<float>),
	target_elevation_rotation(math::identity_quaternion<float>),
	target_azimuth_rotation(math::identity_quaternion<float>),
	target_rotation(math::identity_quaternion<float>)
{}

orbit_cam::~orbit_cam()
{}

void orbit_cam::update(float dt)
{
	float interpolation_factor = 1.0f;

	// Calculate rotation and target rotation quaternions
	//rotation = azimuth_rotation * elevation_rotation;
	target_rotation = math::normalize(target_azimuth_rotation * target_elevation_rotation);
	
	// Calculate target translation
	target_translation = target_focal_point + target_rotation * float3{0.0f, 0.0f, target_focal_distance};
	
	// Interpolate rotation
	//rotation = glm::mix(rotation, target_rotation, interpolation_factor);
	
	// Interpolate angles
	set_elevation(math::lerp(elevation, target_elevation, interpolation_factor));
	set_azimuth(math::lerp(azimuth, target_azimuth, interpolation_factor));
	
	// Calculate rotation
	set_rotation(math::normalize(azimuth_rotation * elevation_rotation));
	
	// Interpolate focal point and focal distance
	focal_point = math::lerp(focal_point, target_focal_point, interpolation_factor);
	focal_distance = math::lerp(focal_distance, target_focal_distance, interpolation_factor);
	
	// Caluclate translation
	set_translation(focal_point + get_rotation() * float3{0.0f, 0.0f, focal_distance});
	/*
	// Recalculate azimuth
	azimuth_rotation = rotation;
	azimuth_rotation.x = 0.0f;
	azimuth_rotation.z = 0.0f;
	azimuth_rotation = glm::normalize(azimuth_rotation);
	azimuth = 2.0f * std::acos(azimuth_rotation.w);
	
	// Recalculate elevation
	elevation_rotation = rotation;
	elevation_rotation.y = 0.0f;
	elevation_rotation.z = 0.0f;
	elevation_rotation = glm::normalize(elevation_rotation);
	elevation = 2.0f * std::acos(elevation_rotation.w);
	*/
	
	// Update camera
	if (get_camera() != nullptr)
	{
		transform_type transform = math::identity_transform<float>;
		transform.translation = get_translation();
		transform.rotation = get_rotation();
		get_camera()->set_transform(transform);
		//get_camera()->look_at(get_translation(), get_translation() + get_forward(), get_up());
	}
}

void orbit_cam::move(const float2& direction)
{
	target_focal_point += azimuth_rotation * float3{direction[0], 0.0f, direction[1]};
}

void orbit_cam::rotate(float angle)
{
	set_target_azimuth(target_azimuth + angle);
}

void orbit_cam::tilt(float angle)
{
	set_target_elevation(target_elevation + angle);
}

void orbit_cam::zoom(float distance)
{
	set_target_focal_distance(target_focal_distance - distance);
}

void orbit_cam::set_focal_point(const float3& point)
{
	focal_point = point;
}

void orbit_cam::set_focal_distance(float distance)
{
	focal_distance = distance;
}

void orbit_cam::set_elevation(float angle)
{
	elevation = angle;
	elevation_rotation = math::angle_axis(elevation, float3{-1.0f, 0.0f, 0.0f});
}

void orbit_cam::set_azimuth(float angle)
{
	azimuth = angle;
	azimuth_rotation = math::angle_axis(azimuth, float3{0.0f, 1.0f, 0.0f});
}

void orbit_cam::set_target_focal_point(const float3& point)
{
	target_focal_point = point;
}

void orbit_cam::set_target_focal_distance(float distance)
{
	target_focal_distance = distance;
}

void orbit_cam::set_target_elevation(float angle)
{
	target_elevation = angle;
	target_elevation_rotation = math::angle_axis(target_elevation, float3{-1.0f, 0.0f, 0.0f});
}

void orbit_cam::set_target_azimuth(float angle)
{
	target_azimuth = angle;
	target_azimuth_rotation = math::angle_axis(target_azimuth, float3{0.0f, 1.0f, 0.0f});
}

