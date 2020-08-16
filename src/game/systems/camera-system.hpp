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
#include "utility/fundamental-types.hpp"

class camera;
class orbit_cam;

class camera_system:
	public entity_system,
	public event_handler<mouse_moved_event>
{
public:
	camera_system(entt::registry& registry);
	virtual void update(double t, double dt);

	void set_orbit_cam(orbit_cam* orbit_cam);
	void set_viewport(const float4& viewport);

private:
	virtual void handle_event(const mouse_moved_event& event);

	orbit_cam* orbit_cam;
	float4 viewport;
	float2 mouse_position;
};

#endif // ANTKEEPER_CAMERA_SYSTEM_HPP

