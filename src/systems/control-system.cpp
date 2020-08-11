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
#include "orbit-cam.hpp"
#include "scene/camera.hpp"
#include "geometry/intersection.hpp"
#include "animation/ease.hpp"
#include "nest.hpp"
#include "math/math.hpp"

control_system::control_system():
	timestep(0.0f),
	zoom(0.0f),
	tool(nullptr),
	flashlight(nullptr),
	flashlight_light_cone(nullptr),
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
	
	// Set deadzone at 15% for all controls
	const std::list<control*>* controls = control_set.get_controls();
	for (control* control: *controls)
	{
		control->set_deadzone(0.15f);
	}

	zoom_speed = 4.0f; //1
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
	orbit_cam = nullptr;
	
	mouse_angle = 0.0f;
	old_mouse_angle = mouse_angle;
	flashlight_turns = 0.0f;
	flashlight_turns_i = 0;
	flashlight_turns_f = 0.0f;
}

void control_system::update(float dt)
{
	this->timestep = dt;

	if (adjust_camera_control.is_active() && !adjust_camera_control.was_active())
	{
	}

	// Determine zoom
	if (zoom_in_control.is_active())
		zoom += zoom_speed * dt * zoom_in_control.get_current_value();
	if (zoom_out_control.is_active())
		zoom -= zoom_speed * dt * zoom_out_control.get_current_value();
	zoom = std::max<float>(0.0f, std::min<float>(1.0f, zoom));

	float focal_distance = math::log_lerp<float>(near_focal_distance, far_focal_distance, 1.0f - zoom);

	float fov = math::log_lerp<float>(near_fov, far_fov, 1.0f - zoom);

	//float elevation_factor = (orbit_cam->get_target_elevation() - min_elevation) / max_elevation;
	//fov = math::log_lerp<float>(near_fov, far_fov, elevation_factor);
	float clip_near = math::log_lerp<float>(near_clip_near, far_clip_near, 1.0f - zoom);
	float clip_far = math::log_lerp<float>(near_clip_far, far_clip_far, 1.0f - zoom);
	float movement_speed = math::log_lerp<float>(near_movement_speed * dt, far_movement_speed * dt, 1.0f - zoom);

	orbit_cam->set_target_focal_distance(focal_distance);
	orbit_cam->get_camera()->set_perspective(fov, orbit_cam->get_camera()->get_aspect_ratio(), clip_near, clip_far);

	const float rotation_speed = 2.0f * dt;
	float rotation = 0.0f;
	if (rotate_ccw_control.is_active())
		rotation += rotate_ccw_control.get_current_value() * rotation_speed;
	if (rotate_cw_control.is_active())
		rotation -= rotate_cw_control.get_current_value() * rotation_speed;
	if (rotation)
	{
		orbit_cam->rotate(rotation);
	}

	const float tilt_speed = 2.0f * dt;
	float tilt = 0.0f;
	if (tilt_up_control.is_active())
		tilt -= tilt_up_control.get_current_value() * tilt_speed;
	if (tilt_down_control.is_active())
		tilt += tilt_down_control.get_current_value() * tilt_speed;
	if (tilt)
	{
		orbit_cam->tilt(tilt);
		float elevation = orbit_cam->get_target_elevation();
		elevation = std::min<float>(max_elevation, std::max<float>(min_elevation, elevation));
		orbit_cam->set_target_elevation(elevation);
	}

	float2 movement{0.0f, 0.0f};
	if (move_right_control.is_active())
		movement[0] += move_right_control.get_current_value();
	if (move_left_control.is_active())
		movement[0] -= move_left_control.get_current_value();
	if (move_forward_control.is_active())
		movement[1] -= move_forward_control.get_current_value();
	if (move_back_control.is_active())
		movement[1] += move_back_control.get_current_value();
	
	const float deadzone = 0.01f;
	float magnitude_squared = math::length_squared(movement);

	if (magnitude_squared > deadzone)
	{
		if (magnitude_squared > 1.0f)
		{
			movement = math::normalize(movement);
		}

		orbit_cam->move(movement * movement_speed);
	}

	float ascention = 0.0f;
	if (ascend_control.is_active())
		ascention += ascend_control.get_current_value();
	if (descend_control.is_active())
		ascention -= descend_control.get_current_value();
	
	if (ascention)
	{
		float old_depth = -orbit_cam->get_target_focal_point()[1];
		
		orbit_cam->set_target_focal_point(orbit_cam->get_target_focal_point() + float3{0.0f, ascention * movement_speed, 0.0f});

		if (nest)
		{
			float3 focal_point = orbit_cam->get_target_focal_point();
			float depth = -focal_point[1];
			
			float delta_shaft_angle = nest->get_shaft_angle(*nest->get_central_shaft(), depth) - nest->get_shaft_angle(*nest->get_central_shaft(), old_depth);
			
			//orbit_cam->set_target_azimuth(orbit_cam->get_target_azimuth() - delta_shaft_angle);
			orbit_cam->set_target_focal_point(nest->get_shaft_position(*nest->get_central_shaft(), depth));
		}
	}

	orbit_cam->update(dt);

	camera* camera = orbit_cam->get_camera();

	float3 pick_near = camera->unproject({mouse_position[0], viewport[3] - mouse_position[1], 0.0f}, viewport);
	float3 pick_far = camera->unproject({mouse_position[0], viewport[3] - mouse_position[1], 1.0f}, viewport);
	float3 pick_direction = math::normalize(pick_far - pick_near);
	ray<float> picking_ray = {pick_near, pick_direction};
	plane<float> ground_plane = {float3{0, 1, 0}, 0.0f};

	auto intersection_result = ray_plane_intersection(picking_ray, ground_plane);
	if (std::get<0>(intersection_result))
	{
		float3 pick = picking_ray.extrapolate(std::get<1>(intersection_result));
		if (tool)
			tool->set_translation(pick);
	}

	if (toggle_view_control.is_active() && !toggle_view_control.was_active())
	{
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
		
		if (flashlight && nest)
		{
			math::transform<float> flashlight_transform = math::identity_transform<float>;
			
			float flashlight_depth = nest->get_shaft_depth(*nest->get_central_shaft(), flashlight_turns);
			
			flashlight_transform.translation = {0.0f, -flashlight_depth, 0.0f};
			flashlight_transform.rotation = math::angle_axis(-flashlight_turns * math::two_pi<float> + math::half_pi<float>, {0, 1, 0});
			
			flashlight->set_transform(flashlight_transform);
			flashlight_light_cone->set_transform(flashlight_transform);
			
			if (underworld_camera)
			{
				underworld_camera->look_at({0, -flashlight_depth + 50.0f, 0}, {0, -flashlight_depth, 0}, {0, 0, -1});
			}
		}
	}
	

}

void control_system::set_orbit_cam(::orbit_cam* orbit_cam)
{
	this->orbit_cam = orbit_cam;
}

void control_system::set_nest(::nest* nest)
{
	this->nest = nest;
}

void control_system::set_tool(model_instance* tool)
{
	this->tool = tool;
}

void control_system::set_flashlight(model_instance* flashlight, model_instance* light_cone)
{
	this->flashlight = flashlight;
	this->flashlight_light_cone = light_cone;
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

		float rotation = math::radians(22.5f) * rotation_factor * timestep;
		float elevation = orbit_cam->get_target_elevation() + elevation_factor * 0.25f * timestep;
		elevation = std::min<float>(max_elevation, std::max<float>(min_elevation, elevation));

		orbit_cam->rotate(rotation);
		orbit_cam->set_target_elevation(elevation);
	}
	else if (!adjust_camera_control.was_active())
	{
		mouse_position[0] = event.x;
		mouse_position[1] = event.y;
	}
}

