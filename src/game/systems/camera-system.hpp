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

#ifndef ANTKEEPER_CAMERA_SYSTEM_HPP
#define ANTKEEPER_CAMERA_SYSTEM_HPP

#include "entity-system.hpp"
#include "event/event-handler.hpp"
#include "event/input-events.hpp"
#include "event/window-events.hpp"
#include "utility/fundamental-types.hpp"
#include "math/quaternion-type.hpp"
#include "math/transform-type.hpp"

class camera;
class orbit_cam;

class camera_system:
	public entity_system,
	public event_handler<mouse_moved_event>,
	public event_handler<window_resized_event>
{
public:
	typedef math::quaternion<float> quaternion_type;
	typedef math::transform<float> transform_type;
	
	camera_system(entt::registry& registry);
	virtual void update(double t, double dt);
	
	void rotate(float angle);
	void tilt(float angle);
	void zoom(float factor);
	
	void set_camera(::camera* camera);
	void set_viewport(const float4& viewport);
	void set_azimuth(float angle);
	void set_elevation(float angle);
	void set_zoom(float factor);
	void set_focal_distance(float distance_near, float distance_far);
	void set_fov(float angle_near, float angle_far);
	void set_clip_near(float distance_near, float distance_far);
	void set_clip_far(float distance_near, float distance_far);

private:
	virtual void handle_event(const mouse_moved_event& event);
	virtual void handle_event(const window_resized_event& event);

	camera* camera;
	float4 viewport;
	float2 mouse_position;
	
	float azimuth;
	float elevation;
	float zoom_factor;
	float focal_distance_near;
	float focal_distance_far;
	float fov_near;
	float fov_far;
	float near_clip_near;
	float near_clip_far;
	float far_clip_near;
	float far_clip_far;
	
	quaternion_type elevation_rotation;
	quaternion_type azimuth_rotation;
};

#endif // ANTKEEPER_CAMERA_SYSTEM_HPP

