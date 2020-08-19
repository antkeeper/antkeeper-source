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
{}

void camera_system::update(double t, double dt)
{
	if (!camera)
		return;
	
	// Determine focal point
	int subject_count = 0;
	float3 focal_point = {0, 0, 0};
	registry.view<camera_subject_component, transform_component>().each(
		[&](auto entity, auto& subject, auto& transform)
		{
			focal_point += transform.transform.translation;
			++subject_count;
		});
	if (subject_count > 1)
		focal_point /= static_cast<float>(subject_count);
	
	// Determine focal distance
	float focal_distance = math::log_lerp<float>(focal_distance_far, focal_distance_near, zoom_factor);
	
	// Determine view point
	quaternion_type rotation = math::normalize(azimuth_rotation * elevation_rotation);
	float3 view_point = focal_point + rotation * float3{0.0f, 0.0f, focal_distance};
	
	// Update camera transform
	transform_type source_transform = camera->get_transform();
	transform_type target_transform = math::identity_transform<float>;
	target_transform.translation = view_point;
	target_transform.rotation = rotation;
	
	
	float2 xz_direction = math::normalize(math::swizzle<0, 2>(focal_point) - math::swizzle<0, 2>(source_transform.translation));
	float source_azimuth = math::wrap_radians(std::atan2(-xz_direction.y, xz_direction.x) - math::half_pi<float>);
	float source_elevation = elevation;
	
	float smooth_factor = 0.1f;
	float smooth_azimuth = math::lerp_angle(source_azimuth, azimuth, smooth_factor);
	float smooth_elevation = math::lerp_angle(source_elevation, elevation, smooth_factor);
	
	
	smooth_azimuth = source_azimuth;
	float shortest_angle = math::wrap_radians(azimuth - source_azimuth);
	static float velocity = 0.0f;
	spring<float, float>(smooth_azimuth, velocity, smooth_azimuth + shortest_angle, 1.0f, 2.0f * math::two_pi<float>, dt);
	
	quaternion_type smooth_azimuth_rotation = math::angle_axis(smooth_azimuth, float3{0.0f, 1.0f, 0.0f});
	quaternion_type smooth_elevation_rotation = math::angle_axis(smooth_elevation, float3{-1.0f, 0.0f, 0.0f});
	quaternion_type smooth_rotation = math::normalize(smooth_azimuth_rotation * smooth_elevation_rotation);
	
	float3 smooth_view_point = focal_point + smooth_rotation * float3{0.0f, 0.0f, focal_distance};

	transform_type smooth_transform;
	smooth_transform.translation = smooth_view_point;
	//smooth_transform.translation = math::lerp(source_transform.translation, target_transform.translation, smooth_factor);
	//smooth_transform.rotation = math::slerp(source_transform.rotation, target_transform.rotation, smooth_factor);
	smooth_transform.rotation = smooth_rotation;
	smooth_transform.scale = math::lerp(source_transform.scale, target_transform.scale, smooth_factor);
	camera->set_transform(smooth_transform);

	
	// Determine FOV
	float fov = math::log_lerp<float>(fov_far, fov_near, zoom_factor);
	
	// Determine aspect ratio
	float aspect_ratio = viewport[2] / viewport[3];
	
	// Determine clipping planes
	float clip_near = math::log_lerp<float>(near_clip_far, near_clip_near, zoom_factor);
	float clip_far = math::log_lerp<float>(far_clip_far, far_clip_near, zoom_factor);
	
	// Update camera projection
	camera->set_perspective(fov, aspect_ratio, clip_near, clip_far);
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
	set_zoom(std::max<float>(0.0f, std::min<float>(1.0f, zoom_factor + factor)));
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
}

void camera_system::set_elevation(float angle)
{
	elevation = math::wrap_radians(angle);
	elevation_rotation = math::angle_axis(elevation, float3{-1.0f, 0.0f, 0.0f});
}

void camera_system::set_zoom(float factor)
{
	this->zoom_factor = factor;
}

void camera_system::set_focal_distance(float distance_near, float distance_far)
{
	focal_distance_near = distance_near;
	focal_distance_far = distance_far;
}

void camera_system::set_fov(float angle_near, float angle_far)
{
	fov_near = angle_near;
	fov_far = angle_far;
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
