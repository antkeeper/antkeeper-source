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

#ifndef ANTKEEPER_CAMERA_SYSTEM_HPP
#define ANTKEEPER_CAMERA_SYSTEM_HPP

#include "entity-system.hpp"
#include "event/event-handler.hpp"
#include "event/input-events.hpp"
#include "event/window-events.hpp"
#include "utility/fundamental-types.hpp"
#include "math/quaternion-type.hpp"
#include "math/transform-type.hpp"
#include "animation/orbit-cam.hpp"
#include "scene/camera.hpp"

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
	
	void pan(float angle);
	void tilt(float angle);
	void zoom(float factor);
	
	void set_camera(scene::camera* camera);
	void set_viewport(const float4& viewport);
	
	const orbit_cam* get_orbit_cam() const;
	orbit_cam* get_orbit_cam();

private:
	virtual void handle_event(const mouse_moved_event& event);
	virtual void handle_event(const window_resized_event& event);

	scene::camera* camera;
	float4 viewport;
	float2 mouse_position;
	
	orbit_cam orbit_cam;
};

inline const orbit_cam* camera_system::get_orbit_cam() const
{
	return &orbit_cam;
}

inline orbit_cam* camera_system::get_orbit_cam()
{
	return &orbit_cam;
}

#endif // ANTKEEPER_CAMERA_SYSTEM_HPP

