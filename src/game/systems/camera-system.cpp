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

#include "camera-system.hpp"
#include "game/components/camera-subject-component.hpp"
#include "game/components/transform-component.hpp"
#include "animation/spring.hpp"
#include "scene/camera.hpp"
#include "math/math.hpp"
#include <cmath>
#include <iostream>

using namespace ecs;

camera_system::camera_system(entt::registry& registry):
	entity_system(registry),
	camera(nullptr),
	viewport{0, 0, 0, 0},
	mouse_position{0, 0}
{
	// Init azimuth spring constraint
	azimuth_spring.v = 0.0f;
	azimuth_spring.z = 1.0f;
	azimuth_spring.w = 2.0f * math::two_pi<float>;
	
	// Init elevation spring constraint
	elevation_spring.v = 0.0f;
	elevation_spring.z = 1.0f;
	elevation_spring.w = 2.0f * math::two_pi<float>;
	
	// Init focal distance spring constraint
	focal_distance_spring.v = 0.0f;
	focal_distance_spring.z = 1.0f;
	focal_distance_spring.w = 5.0f * math::two_pi<float>;
	
	// Init fov spring constraint
	fov_spring.v = 0.0f;
	fov_spring.z = 1.0f;
	fov_spring.w = 5.0f * math::two_pi<float>;
}

void camera_system::update(double t, double dt)
{
	if (!camera)
		return;
	
	// Determine target focal point
	int subject_count = 0;
	float3 target_focal_point = {0, 0, 0};
	registry.view<camera_subject_component, transform_component>().each(
		[&](auto entity, auto& subject, auto& transform)
		{
			target_focal_point += transform.transform.translation;
			++subject_count;
		});
	if (subject_count > 1)
		target_focal_point /= static_cast<float>(subject_count);
	
	// Get source transform
	transform_type source_transform = camera->get_transform();
	
	// Solve azimuth spring
	float2 xz_direction = math::normalize(math::swizzle<0, 2>(target_focal_point) - math::swizzle<0, 2>(source_transform.translation));	
	azimuth_spring.x0 = math::wrap_radians(std::atan2(-xz_direction.y, xz_direction.x) - math::half_pi<float>);
	azimuth_spring.x1 = azimuth_spring.x0 + math::wrap_radians(azimuth_spring.x1 - azimuth_spring.x0);
	solve_spring_constraint<float, float>(azimuth_spring, dt);
	
	// Sovle elevation spring
	elevation_spring.x0 = elevation;
	elevation_spring.x1 = elevation_spring.x0 + math::wrap_radians(elevation_spring.x1 - elevation_spring.x0);
	solve_spring_constraint<float, float>(elevation_spring, dt);
	
	// Solve focal distance spring
	focal_distance_spring.x0 = math::length(source_transform.translation - target_focal_point);
	solve_spring_constraint<float, float>(focal_distance_spring, dt);
	
	// Solve FOV spring
	fov_spring.x0 = camera->get_fov();
	solve_spring_constraint<float, float>(fov_spring, dt);
	
	// Determine camera rotation
	quaternion_type smooth_azimuth_rotation = math::angle_axis(azimuth_spring.x0, float3{0.0f, 1.0f, 0.0f});
	quaternion_type smooth_elevation_rotation = math::angle_axis(elevation_spring.x0, float3{-1.0f, 0.0f, 0.0f});
	quaternion_type smooth_rotation = math::normalize(smooth_azimuth_rotation * smooth_elevation_rotation);
	
	// Determine camera view point
	float3 smooth_view_point = target_focal_point + smooth_rotation * float3{0.0f, 0.0f, focal_distance_spring.x0};

	// Update camera transform
	transform_type smooth_transform;
	smooth_transform.translation = smooth_view_point;
	smooth_transform.rotation = smooth_rotation;
	smooth_transform.scale = source_transform.scale;
	camera->set_transform(smooth_transform);
	
	// Determine aspect ratio
	float aspect_ratio = viewport[2] / viewport[3];
	
	// Determine clipping planes
	float clip_near = math::log_lerp<float>(near_clip_far, near_clip_near, zoom_factor);
	float clip_far = math::log_lerp<float>(far_clip_far, far_clip_near, zoom_factor);
	
	// Update camera projection
	camera->set_perspective(fov_spring.x0, aspect_ratio, clip_near, clip_far);
}

void camera_system::rotate(float angle)
{
	set_azimuth(azimuth + angle);
}

void camera_system::tilt(float angle)
{
	set_elevation(elevation + angle);
}

void camera_system::zoom(float factor)
{
	set_zoom(zoom_factor + factor);
}

void camera_system::set_camera(::camera* camera)
{
	this->camera = camera;
}

void camera_system::set_viewport(const float4& viewport)
{
	this->viewport = viewport;
}

void camera_system::set_azimuth(float angle)
{
	azimuth = math::wrap_radians(angle);
	azimuth_rotation = math::angle_axis(azimuth, float3{0.0f, 1.0f, 0.0f});
	azimuth_spring.x1 = azimuth;
}

void camera_system::set_elevation(float angle)
{
	elevation = math::wrap_radians(angle);
	elevation_rotation = math::angle_axis(elevation, float3{-1.0f, 0.0f, 0.0f});
	elevation_spring.x1 = elevation;
}

void camera_system::set_zoom(float factor)
{
	this->zoom_factor = std::max<float>(0.0f, std::min<float>(1.0f, factor));
	update_focal_distance();
	update_fov();
}

void camera_system::set_focal_distance(float distance_near, float distance_far)
{
	focal_distance_near = distance_near;
	focal_distance_far = distance_far;
	update_focal_distance();
}

void camera_system::set_fov(float angle_near, float angle_far)
{
	fov_near = angle_near;
	fov_far = angle_far;
	update_fov();
}

void camera_system::set_clip_near(float distance_near, float distance_far)
{
	near_clip_near = distance_near;
	near_clip_far = distance_far;
}

void camera_system::set_clip_far(float distance_near, float distance_far)
{
	far_clip_near = distance_near;
	far_clip_far = distance_far;
}

void camera_system::handle_event(const mouse_moved_event& event)
{
	mouse_position[0] = event.x;
	mouse_position[1] = event.y;
}

void camera_system::handle_event(const window_resized_event& event)
{
	set_viewport({0.0f, 0.0f, static_cast<float>(event.w), static_cast<float>(event.h)});
}

void camera_system::update_focal_distance()
{
	focal_distance = math::log_lerp<float>(focal_distance_far, focal_distance_near, zoom_factor);
	focal_distance_spring.x1 = focal_distance;
}

void camera_system::update_fov()
{
	fov = math::log_lerp<float>(fov_far, fov_near, zoom_factor);
	fov_spring.x1 = fov;
}
