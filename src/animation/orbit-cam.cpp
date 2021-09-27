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

#include "animation/orbit-cam.hpp"
#include "scene/camera.hpp"
#include "math/math.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>

orbit_cam::orbit_cam():
	azimuth_limits({-std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()}),
	elevation_limits({-std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()}),
	focal_distance_limits({-std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()}),
	fov_limits({-std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()}),
	clip_near_limits({-std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()}),
	clip_far_limits({-std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()})
{
	// Make all springs critically-damped
	focal_point_spring.z = 1.0f;
	azimuth_spring.z = 1.0f;
	elevation_spring.z = 1.0f;
	zoom_spring.z = 1.0f;
	
	// Init spring oscillation frequencies to 1 rad/s
	focal_point_spring.w = math::two_pi<float>;
	azimuth_spring.w = math::two_pi<float>;
	elevation_spring.w = math::two_pi<float>;
	zoom_spring.w = math::two_pi<float>;
	
	// Zero spring values and velocities
	focal_point_spring.x1 = {0.0f, 0.0f, 0.0f};
	azimuth_spring.x1 = 0.0f;
	elevation_spring.x1 = 0.0f;
	zoom_spring.x1 = 0.0f;
	reset_springs();
}

orbit_cam::~orbit_cam()
{}

void orbit_cam::update(float dt)
{
	if (!get_camera())
	{
		return;
	}
	
	// Solve springs
	solve_numeric_spring<float3, float>(focal_point_spring, dt);
	solve_numeric_spring<float, float>(azimuth_spring, dt);
	solve_numeric_spring<float, float>(elevation_spring, dt);
	solve_numeric_spring<float, float>(zoom_spring, dt);
	
	// Calculate zoom-dependent variables
	float focal_distance = math::log_lerp<float>(focal_distance_limits[1], focal_distance_limits[0], zoom_spring.x0);
	float fov = math::log_lerp<float>(fov_limits[1], fov_limits[0], zoom_spring.x0);
	float clip_near = math::log_lerp<float>(clip_near_limits[1], clip_near_limits[0], zoom_spring.x0);
	float clip_far = math::log_lerp<float>(clip_far_limits[1], clip_far_limits[0], zoom_spring.x0);
	
	// Calculate camera transform
	transform_type transform = math::identity_transform<float>;
	
	// Determine rotation
	azimuth_rotation = math::angle_axis(azimuth_spring.x0, float3{0.0f, 1.0f, 0.0f});
	elevation_rotation = math::angle_axis(elevation_spring.x0, float3{-1.0f, 0.0f, 0.0f});
	transform.rotation = math::normalize(azimuth_rotation * elevation_rotation);
	
	// Determine translation
	transform.translation = focal_point_spring.x0 + transform.rotation * float3{0.0f, 0.0f, focal_distance};
	
	// Update camera transform
	update_transform(transform);
	
	// Update camera projection
	update_projection(fov, aspect_ratio, clip_near, clip_far);
}

void orbit_cam::move(const float3& translation)
{
	set_target_focal_point(focal_point_spring.x1 + translation);
}

void orbit_cam::pan(float angle)
{
	set_target_azimuth(azimuth_spring.x1 + angle);
}

void orbit_cam::tilt(float angle)
{
	set_target_elevation(elevation_spring.x1 + angle);
}

void orbit_cam::zoom(float factor)
{
	set_target_zoom(zoom_spring.x1 + factor);
}

void orbit_cam::reset_springs()
{
	// Reset values
	focal_point_spring.x0 = focal_point_spring.x1;
	azimuth_spring.x0 = azimuth_spring.x1;
	elevation_spring.x0 = elevation_spring.x1;
	zoom_spring.x0 = zoom_spring.x1;
	
	// Reset velocities
	focal_point_spring.v = {0.0f, 0.0f, 0.0f};
	azimuth_spring.v = 0.0f;
	elevation_spring.v = 0.0f;
	zoom_spring.v = 0.0f;
}

void orbit_cam::set_aspect_ratio(float ratio)
{
	aspect_ratio = ratio;
}

void orbit_cam::set_focal_point(const float3& point)
{
	focal_point_spring.x0 = point;
}

void orbit_cam::set_azimuth(float angle)
{
	azimuth_spring.x0 = std::max<float>(azimuth_limits[0], std::min<float>(azimuth_limits[1], angle));
}

void orbit_cam::set_elevation(float angle)
{
	elevation_spring.x0 = std::max<float>(elevation_limits[0], std::min<float>(elevation_limits[1], angle));
}

void orbit_cam::set_zoom(float factor)
{
	zoom_spring.x0 = std::max<float>(0.0f, std::min<float>(1.0f, factor));
}

void orbit_cam::set_target_focal_point(const float3& point)
{
	focal_point_spring.x1 = point;
}

void orbit_cam::set_target_azimuth(float angle)
{
	azimuth_spring.x1 = std::max<float>(azimuth_limits[0], std::min<float>(azimuth_limits[1], angle));
}

void orbit_cam::set_target_elevation(float angle)
{
	elevation_spring.x1 = std::max<float>(elevation_limits[0], std::min<float>(elevation_limits[1], angle));
}

void orbit_cam::set_target_zoom(float factor)
{
	zoom_spring.x1 = std::max<float>(0.0f, std::min<float>(1.0f, factor));
}

void orbit_cam::set_azimuth_limits(const std::array<float, 2>& limits)
{
	azimuth_limits = limits;
}

void orbit_cam::set_elevation_limits(const std::array<float, 2>& limits)
{
	elevation_limits = limits;
}

void orbit_cam::set_focal_distance_limits(const std::array<float, 2>& limits)
{
	focal_distance_limits = limits;
}

void orbit_cam::set_fov_limits(const std::array<float, 2>& limits)
{
	fov_limits = limits;
}

void orbit_cam::set_clip_near_limits(const std::array<float, 2>& limits)
{
	clip_near_limits = limits;
}

void orbit_cam::set_clip_far_limits(const std::array<float, 2>& limits)
{
	clip_far_limits = limits;
}

void orbit_cam::set_focal_point_oscillation(float frequency)
{
	focal_point_spring.w = frequency;
}

void orbit_cam::set_azimuth_oscillation(float frequency)
{
	azimuth_spring.w = frequency;
}

void orbit_cam::set_elevation_oscillation(float frequency)
{
	elevation_spring.w = frequency;
}

void orbit_cam::set_zoom_oscillation(float frequency)
{
	zoom_spring.w = frequency;
}
