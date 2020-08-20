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

#ifndef ANTKEEPER_CONTROL_SYSTEM_HPP
#define ANTKEEPER_CONTROL_SYSTEM_HPP

#include "game/systems/entity-system.hpp"
#include "event/event-handler.hpp"
#include "event/input-events.hpp"
#include "event/window-events.hpp"
#include "input/control.hpp"
#include "input/control-set.hpp"
#include "scene/model-instance.hpp"
#include "utility/fundamental-types.hpp"

class nest;
class camera;
class camera_system;

class control_system:
	public entity_system,
	public event_handler<mouse_moved_event>,
	public event_handler<window_resized_event>
{
public:
	control_system(entt::registry& registry);
	
	virtual void update(double t, double dt);
	
	void set_invert_mouse_x(bool invert);
	void set_invert_mouse_y(bool invert);

	void set_camera_system(camera_system* camera_system);
	void set_nest(::nest* nest);
	void set_tool(model_instance* tool);
	void set_flashlight(entt::entity eid);
	void set_camera_subject(entt::entity eid);
	
	void set_viewport(const float4& viewport);
	void set_underworld_camera(::camera* camera);

	control_set* get_control_set();
	control* get_move_forward_control();
	control* get_move_back_control();
	control* get_move_left_control();
	control* get_move_right_control();
	control* get_rotate_ccw_control();
	control* get_rotate_cw_control();
	control* get_tilt_up_control();
	control* get_tilt_down_control();
	control* get_zoom_in_control();
	control* get_zoom_out_control();
	control* get_adjust_camera_control();
	control* get_ascend_control();
	control* get_descend_control();
	control* get_toggle_view_control();
	control* get_tool_menu_control();

private:
	virtual void handle_event(const mouse_moved_event& event);
	virtual void handle_event(const window_resized_event& event);

	control_set control_set;
	control move_forward_control;
	control move_back_control;
	control move_left_control;
	control move_right_control;
	control rotate_ccw_control;
	control rotate_cw_control;
	control tilt_up_control;
	control tilt_down_control;
	control zoom_in_control;
	control zoom_out_control;
	control adjust_camera_control;
	control ascend_control;
	control descend_control;
	control toggle_view_control;
	control tool_menu_control;

	float zoom_speed;
	float min_elevation;
	float max_elevation;
	float near_focal_distance;
	float far_focal_distance;
	float near_movement_speed;
	float far_movement_speed;
	float near_fov;
	float far_fov;
	float near_clip_near;
	float far_clip_near;
	float near_clip_far;
	float far_clip_far;
	
	float timestep;
	float zoom;
	camera_system* camera_system;
	::nest* nest;
	model_instance* tool;
	float2 mouse_position;
	float4 viewport;
	
	entt::entity flashlight_eid;
	entt::entity camera_subject_eid;
	camera* underworld_camera;
	
	float mouse_angle;
	float old_mouse_angle;
	float flashlight_turns;
	float flashlight_turns_i;
	float flashlight_turns_f;
};

inline control_set* control_system::get_control_set()
{
	return &control_set;
}

inline control* control_system::get_move_forward_control()
{
	return &move_forward_control;
}

inline control* control_system::get_move_back_control()
{
	return &move_back_control;
}

inline control* control_system::get_move_left_control()
{
	return &move_left_control;
}

inline control* control_system::get_move_right_control()
{
	return &move_right_control;
}

inline control* control_system::get_rotate_ccw_control()
{
	return &rotate_ccw_control;
}

inline control* control_system::get_rotate_cw_control()
{
	return &rotate_cw_control;
}

inline control* control_system::get_tilt_up_control()
{
	return &tilt_up_control;
}

inline control* control_system::get_tilt_down_control()
{
	return &tilt_down_control;
}

inline control* control_system::get_zoom_in_control()
{
	return &zoom_in_control;
}

inline control* control_system::get_zoom_out_control()
{
	return &zoom_out_control;
}

inline control* control_system::get_adjust_camera_control()
{
	return &adjust_camera_control;
}

inline control* control_system::get_ascend_control()
{
	return &ascend_control;
}

inline control* control_system::get_descend_control()
{
	return &descend_control;
}

inline control* control_system::get_toggle_view_control()
{
	return &toggle_view_control;
}

inline control* control_system::get_tool_menu_control()
{
	return &tool_menu_control;
}

#endif // ANTKEEPER_CONTROL_SYSTEM_HPP

