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

#include "camera-system.hpp"
#include "game/components/camera-follow-component.hpp"
#include "game/components/transform-component.hpp"
#include "scene/camera.hpp"
#include "math/math.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

using namespace ecs;

camera_system::camera_system(entt::registry& registry):
	entity_system(registry),
	camera(nullptr),
	viewport{0, 0, 0, 0},
	mouse_position{0, 0}
{
	orbit_cam.set_elevation_limits({math::radians(5.0f), math::radians(89.0f)});
	//orbit_cam.set_elevation_limits({math::radians(-89.0f), math::radians(89.0f)});
	orbit_cam.set_focal_distance_limits({2.0f, 200.0f});
	orbit_cam.set_fov_limits({math::radians(80.0f), math::radians(35.0f)});
	orbit_cam.set_clip_near_limits({0.1f, 5.0f});
	orbit_cam.set_clip_far_limits({100.0f, 5000.0f});
	
	orbit_cam.set_target_focal_point({0.0f, 0.0f, 0.0f});
	orbit_cam.set_target_azimuth(0.0f);
	orbit_cam.set_target_elevation(math::radians(45.0f));
	orbit_cam.set_target_zoom(0.0f);
	
	orbit_cam.set_focal_point_oscillation(hz_to_rads(8.0f));
	orbit_cam.set_azimuth_oscillation(hz_to_rads(2.0f));
	orbit_cam.set_elevation_oscillation(hz_to_rads(2.0f));
	orbit_cam.set_zoom_oscillation(hz_to_rads(5.0f));
	
	orbit_cam.reset_springs();
}

void camera_system::update(double t, double dt)
{
	if (!camera)
		return;
	
	// Determine target focal point
	int subject_count = 0;
	float3 target_focal_point = {0, 0, 0};
	registry.view<camera_follow_component, transform_component>().each(
		[&](auto entity, auto& follow, auto& transform)
		{
			target_focal_point += transform.local.translation;
			++subject_count;
		});
	if (subject_count > 1)
		target_focal_point /= static_cast<float>(subject_count);
	
	// Focus at ant's head height off the ground.
	target_focal_point.y += 0.2f;
	
	// Check for collision with environment
	//...
	
	orbit_cam.set_target_focal_point(target_focal_point);
	orbit_cam.update(static_cast<float>(dt));
}

void camera_system::pan(float angle)
{
	orbit_cam.pan(angle);
}

void camera_system::tilt(float angle)
{
	orbit_cam.tilt(angle);
}

void camera_system::zoom(float factor)
{
	orbit_cam.zoom(factor);
}

void camera_system::set_camera(::camera* camera)
{
	this->camera = camera;
	if (camera)
	{
		orbit_cam.attach(camera);
	}
	else
	{
		orbit_cam.detach();
	}
}

void camera_system::set_viewport(const float4& viewport)
{
	this->viewport = viewport;
	orbit_cam.set_aspect_ratio(viewport[2] / viewport[3]);
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
