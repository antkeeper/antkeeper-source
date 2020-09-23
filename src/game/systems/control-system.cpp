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

#include "control-system.hpp"
#include "input/control.hpp"
#include "scene/camera.hpp"
#include "geometry/intersection.hpp"
#include "animation/ease.hpp"
#include "nest.hpp"
#include "math/math.hpp"
#include "game/entity-commands.hpp"
#include "game/systems/camera-system.hpp"
#include "animation/orbit-cam.hpp"

control_system::control_system(entt::registry& registry):
	entity_system(registry),
	timestep(0.0f),
	zoom(0.0f),
	tool(nullptr),
	flashlight_eid(entt::null),
	underworld_camera(nullptr)
{
	control_set.add_control(&move_forward_control);
	control_set.add_control(&move_back_control);
	control_set.add_control(&move_right_control);
	control_set.add_control(&move_left_control);
	control_set.add_control(&rotate_ccw_control);
	control_set.add_control(&rotate_cw_control);
	control_set.add_control(&tilt_up_control);
	control_set.add_control(&tilt_down_control);
	control_set.add_control(&zoom_in_control);
	control_set.add_control(&zoom_out_control);
	control_set.add_control(&adjust_camera_control);
	control_set.add_control(&ascend_control);
	control_set.add_control(&descend_control);
	control_set.add_control(&toggle_view_control);
	control_set.add_control(&tool_menu_control);
	control_set.add_control(&equip_lens_control);
	control_set.add_control(&equip_brush_control);
	control_set.add_control(&equip_forceps_control);
	control_set.add_control(&equip_marker_control);
	control_set.add_control(&equip_container_control);
	control_set.add_control(&equip_twig_control);
	control_set.add_control(&next_marker_control);
	control_set.add_control(&previous_marker_control);
	control_set.add_control(&use_tool_control);
	control_set.add_control(&fast_forward_control);
	control_set.add_control(&rewind_control);
	
	// Set deadzone at 15% for all controls
	const std::list<control*>* controls = control_set.get_controls();
	for (control* control: *controls)
	{
		control->set_deadzone(0.15f);
	}

	zoom_speed = 5.0f; //1
	min_elevation = math::radians(-85.0f);
	max_elevation = math::radians(85.0f);
	near_focal_distance = 2.0f;
	far_focal_distance = 200.0f;
	near_movement_speed = 10.0f;
	far_movement_speed = 80.0f;
	near_fov = math::radians(80.0f);
	far_fov = math::radians(35.0f);
	near_clip_near = 0.1f;
	far_clip_near = 5.0f;
	near_clip_far = 100.0f;
	far_clip_far = 2000.0f;

	nest = nullptr;
	
	mouse_angle = 0.0f;
	old_mouse_angle = mouse_angle;
	flashlight_turns = 0.0f;
	flashlight_turns_i = 0;
	flashlight_turns_f = 0.0f;
}

void control_system::update(double t, double dt)
{
	timestep = dt;
	
	// Zoom camera
	if (zoom_in_control.is_active())
		camera_system->zoom(zoom_speed * dt);
	if (zoom_out_control.is_active())
		camera_system->zoom(-zoom_speed * dt);
	
	// Rotate camera
	const float rotation_speed = math::radians(270.0f);
	if (rotate_ccw_control.is_active())
		camera_system->pan(rotation_speed * dt * std::min<float>(1.0f, rotate_ccw_control.get_current_value()));
	if (rotate_cw_control.is_active())
		camera_system->pan(-rotation_speed * dt * std::min<float>(1.0f, rotate_cw_control.get_current_value()));
	
	// Move camera
	float3 movement{0.0f, 0.0f, 0.0f};
	if (move_right_control.is_active())
		movement.x += move_right_control.get_current_value();
	if (move_left_control.is_active())
		movement.x -= move_left_control.get_current_value();
	if (move_forward_control.is_active())
		movement.z -= move_forward_control.get_current_value();
	if (move_back_control.is_active())
		movement.z += move_back_control.get_current_value();
	
	if (math::length_squared(movement) != 0.0f)
	{
		std::array<float, 2> speed_limits = {15.0f, 100.0f};
		
		float zoom = camera_system->get_orbit_cam()->get_zoom();
		float max_speed = math::log_lerp(speed_limits[1], speed_limits[0], zoom) * dt;
		
		float speed = std::min<float>(max_speed, math::length(movement * max_speed));
		movement = math::normalize(movement) * speed;
		
		const math::quaternion<float>& azimuth_rotation = camera_system->get_orbit_cam()->get_azimuth_rotation();
		movement = azimuth_rotation * movement;
		
		ec::translate(registry, camera_subject_eid, movement);
	}
	
	// Turn flashlight
	float2 viewport_center = {(viewport[0] + viewport[2]) * 0.5f, (viewport[1] + viewport[3]) * 0.5f};
	float2 mouse_direction = math::normalize(mouse_position - viewport_center);
	old_mouse_angle = mouse_angle;
	mouse_angle = std::atan2(-mouse_direction.y, mouse_direction.x);
	
	if (mouse_angle - old_mouse_angle != 0.0f)
	{
		if (mouse_angle - old_mouse_angle <= -math::pi<float>)
			flashlight_turns_i -= 1;
		else if (mouse_angle - old_mouse_angle >= math::pi<float>)
			flashlight_turns_i += 1;
		
		flashlight_turns_f = (mouse_angle) / math::two_pi<float>;
		flashlight_turns = flashlight_turns_i - flashlight_turns_f;
		
		if (flashlight_eid != entt::null && nest)
		{
			math::transform<float> flashlight_transform = math::identity_transform<float>;
			
			float flashlight_depth = nest->get_shaft_depth(*nest->get_central_shaft(), flashlight_turns);
			
			flashlight_transform.translation = {0.0f, -flashlight_depth, 0.0f};
			flashlight_transform.rotation = math::angle_axis(-flashlight_turns * math::two_pi<float> + math::half_pi<float>, {0, 1, 0});
			
			ec::set_transform(registry, flashlight_eid, flashlight_transform, false);
			
			if (underworld_camera)
			{
				underworld_camera->look_at({0, -flashlight_depth + 50.0f, 0}, {0, -flashlight_depth, 0}, {0, 0, -1});
			}
		}
	}
}

void control_system::set_camera_system(::camera_system* camera_system)
{
	this->camera_system = camera_system;
}

void control_system::set_nest(::nest* nest)
{
	this->nest = nest;
}

void control_system::set_tool(model_instance* tool)
{
	this->tool = tool;
}

void control_system::set_flashlight(entt::entity eid)
{
	flashlight_eid = eid;
}

void control_system::set_camera_subject(entt::entity eid)
{
	camera_subject_eid = eid;
}

void control_system::set_viewport(const float4& viewport)
{
	this->viewport = viewport;
}

void control_system::set_underworld_camera(::camera* camera)
{
	this->underworld_camera = camera;
}

void control_system::handle_event(const mouse_moved_event& event)
{
	if (adjust_camera_control.is_active())
	{
		bool invert_x = true;
		bool invert_y = false;

		float rotation_factor = event.dx;
		float elevation_factor = event.dy;
		
		if (invert_x)
		{
			rotation_factor *= -1.0f;
		}
		if (invert_y)
		{
			elevation_factor *= -1.0f;
		}

		float rotation = math::radians(15.0f) * rotation_factor * timestep;
		float tilt = math::radians(15.0f) * elevation_factor * timestep;
		
		camera_system->pan(rotation);
		camera_system->tilt(tilt);
		
	}
	else if (!adjust_camera_control.was_active())
	{
		mouse_position[0] = event.x;
		mouse_position[1] = event.y;
	}
}

void control_system::handle_event(const window_resized_event& event)
{
	set_viewport({0.0f, 0.0f, static_cast<float>(event.w), static_cast<float>(event.h)});
}
