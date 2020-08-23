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

#ifndef ANTKEEPER_TOOL_SYSTEM_HPP
#define ANTKEEPER_TOOL_SYSTEM_HPP

#include "entity-system.hpp"
#include "event/event-handler.hpp"
#include "event/input-events.hpp"
#include "event/window-events.hpp"
#include "utility/fundamental-types.hpp"
#include "animation/spring.hpp"

class camera;
class orbit_cam;

class tool_system:
	public entity_system,
	public event_handler<mouse_moved_event>,
	public event_handler<window_resized_event>
{
public:
	tool_system(entt::registry& registry);
	virtual void update(double t, double dt);

	void set_camera(const camera* camera);
	void set_orbit_cam(const orbit_cam* camera);
	void set_viewport(const float4& viewport);
	void set_pick(bool enabled);
	void set_sun_direction(const float3& direction);

private:
	virtual void handle_event(const mouse_moved_event& event);
	virtual void handle_event(const window_resized_event& event);

	const camera* camera;
	const orbit_cam* orbit_cam;
	float4 viewport;
	float2 mouse_position;
	bool was_pick_enabled;
	bool pick_enabled;
	float3 sun_direction;
	
	numeric_spring<float, float> hand_angle_spring;
	numeric_spring<float3, float> pick_spring;
};

#endif // ANTKEEPER_TOOL_SYSTEM_HPP
