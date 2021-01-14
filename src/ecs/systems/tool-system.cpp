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

#include "tool-system.hpp"
#include "ecs/components/collision-component.hpp"
#include "ecs/components/tool-component.hpp"
#include "ecs/components/transform-component.hpp"
#include "event/event-dispatcher.hpp"
#include "game/events/tool-events.hpp"
#include "animation/orbit-cam.hpp"
#include "animation/ease.hpp"
#include "geometry/mesh.hpp"
#include "geometry/intersection.hpp"
#include "math/math.hpp"
#include "ecs/commands.hpp"

namespace ecs {

tool_system::tool_system(ecs::registry& registry, ::event_dispatcher* event_dispatcher):
	entity_system(registry),
	event_dispatcher(event_dispatcher),
	camera(nullptr),
	orbit_cam(orbit_cam),
	viewport{0, 0, 0, 0},
	mouse_position{0, 0},
	pick_enabled(true),
	was_pick_enabled(pick_enabled),
	active_tool(entt::null)
{
	hand_angle_spring.z = 1.0f;
	hand_angle_spring.w = hz_to_rads(8.0f);
	hand_angle_spring.x1 = math::pi<float>;
	hand_angle_spring.x0 = hand_angle_spring.x1;
	hand_angle_spring.v = 0.0f;
	
	pick_spring.z = 1.0f;
	pick_spring.w = hz_to_rads(30.0f);
	pick_spring.x1 = {0.0f, 0.0f, 0.0f};
	pick_spring.x0 = pick_spring.x1;
	pick_spring.v = {0.0f, 0.0f, 0.0f};
	
	// Create descend animation
	animation_channel<float>* channel = descend_animation.add_channel(0);
	descend_animation.set_interpolator(ease<float, double>::out_cubic);
	descend_animation.set_frame_callback
	(
		[this](int channel, const float& t)
		{
			this->active_tool_distance = t;
		}
	);
	
	// Create descend animation
	channel = ascend_animation.add_channel(0);
	ascend_animation.set_interpolator(ease<float, double>::out_cubic);
	ascend_animation.set_frame_callback
	(
		[this](int channel, const float& t)
		{
			this->active_tool_distance = t;
		}
	);
	
	active_tool = entt::null;
	active_tool_distance = 0.0f;
	warp = true;
	tool_active = false;
	
	event_dispatcher->subscribe<mouse_moved_event>(this);
	event_dispatcher->subscribe<window_resized_event>(this);
}

tool_system::~tool_system()
{
	event_dispatcher->unsubscribe<mouse_moved_event>(this);
	event_dispatcher->unsubscribe<window_resized_event>(this);
}

void tool_system::update(double t, double dt)
{
	if (active_tool == entt::null)
		return;
	
	// Advance animations
	ascend_animation.advance(dt);
	descend_animation.advance(dt);
	
	if (!camera)
		return;
	
	float3 pick_near = camera->unproject({mouse_position[0], viewport[3] - mouse_position[1], 0.0f}, viewport);
	float3 pick_far = camera->unproject({mouse_position[0], viewport[3] - mouse_position[1], 1.0f}, viewport);
	float3 pick_origin = pick_near;
	float3 pick_direction = math::normalize(pick_far - pick_near);
	ray<float> picking_ray = {pick_near, pick_direction};

	float a = std::numeric_limits<float>::infinity();
	bool intersection = false;
	float3 pick;

	// Cast ray from cursor to collision components to find closest intersection
	registry.view<transform_component, collision_component>().each(
		[&](ecs::entity entity, auto& transform, auto& collision)
		{
			math::transform<float> inverse_transform = math::inverse(transform.local);
			float3 origin = inverse_transform * pick_origin;
			float3 direction = math::normalize(math::conjugate(transform.local.rotation) * pick_direction);
			ray<float> transformed_ray = {origin, direction};

			// Broad phase AABB test
			auto aabb_result = ray_aabb_intersection(transformed_ray, collision.bounds);
			if (!std::get<0>(aabb_result))
			{
				return;
			}

			// Narrow phase mesh test
			auto mesh_result = collision.mesh_accelerator.query_nearest(transformed_ray);
			if (mesh_result)
			{
				intersection = true;
				if (mesh_result->t < a)
				{
					a = mesh_result->t;
					pick = picking_ray.extrapolate(a);
					pick_spring.x1 = pick;
				}
			}
		});
	
	const float3& camera_position = camera->get_translation();
	float3 pick_planar_position = float3{pick.x, 0, pick.z};
	float3 camera_planar_position = float3{camera_position.x, 0, camera_position.z};

	float pick_angle = 0.0f;
	float3 pick_planar_direction = math::normalize(pick_planar_position - camera_planar_position);
	float3 camera_planar_focal_point = float3{orbit_cam->get_focal_point().x, 0, orbit_cam->get_focal_point().z};
	float3 camera_planar_direction = math::normalize(camera_planar_focal_point - camera_planar_position);
	if (std::fabs(math::length_squared(camera_planar_direction - pick_planar_direction) > 0.0001f))
	{
		pick_angle = std::acos(math::dot(camera_planar_direction, pick_planar_direction));
		if (math::dot(math::cross(camera_planar_direction, pick_planar_direction), float3{0, 1, 0}) < 0.0f)
			pick_angle = -pick_angle;
	}
	
	// Determine target hand angle
	hand_angle_spring.x1 = -std::min<float>(0.5f, std::max<float>(-0.5f, ((mouse_position[0] / viewport[2]) - 0.5f) * 1.0f)) * (math::pi<float>);
	
	// Solve springs
	solve_numeric_spring<float, float>(hand_angle_spring, dt);
	solve_numeric_spring<float3, float>(pick_spring, dt);
	
	// Don't use spring for picking
	pick_spring.x0 = pick_spring.x1;

	// Move active tools to intersection location
	registry.view<tool_component, transform_component>().each(
		[&](ecs::entity entity, auto& tool, auto& transform)
		{
			/*
			if (registry.has<model_component>(entity))
			{
				
			}*/
			
			if (!tool.active)
				return;
			
			active_tool = entity;
			
			float tool_distance = active_tool_distance;//(tool_active) ? tool.active_distance : tool.idle_distance;

			
			// Interpolate between left and right hand
			math::quaternion<float> hand_rotation = math::angle_axis(orbit_cam->get_azimuth() + hand_angle_spring.x0, float3{0, 1, 0});
			
			math::quaternion<float> tilt_rotation = math::angle_axis(orbit_cam->get_elevation(), float3{-1.0f, 0.0f, 0.0f});
						
			if (tool.heliotropic)
			{
				math::quaternion<float> solar_rotation = math::rotation(float3{0, -1, 0}, sun_direction);
				transform.local.translation = pick_spring.x0 + solar_rotation * float3{0, tool_distance, 0};
				transform.local.rotation = solar_rotation * hand_rotation;
			}
			else
			{
				math::quaternion<float> rotation = hand_rotation * tilt_rotation;
				transform.local.translation = pick_spring.x0 + rotation * float3{0, tool_distance, 0};
				transform.local.rotation = rotation;
			}
			
			if (warp)
			{
				transform.warp = true;
				command::assign_render_layers(registry, active_tool, 1);
				warp = false;
			}
			
			// Update tool's cursor position
			tool.cursor = pick_spring.x0;

			//math::quaternion<float> rotation = math::angle_axis(orbit_cam->get_azimuth() + pick_angle, float3{0, 1, 0});
			//transform.transform.rotation = rotation;
		});
	
	was_pick_enabled = pick_enabled;
}

void tool_system::set_camera(const scene::camera* camera)
{
	this->camera = camera;
}

void tool_system::set_orbit_cam(const ::orbit_cam* orbit_cam)
{
	this->orbit_cam = orbit_cam;
}

void tool_system::set_viewport(const float4& viewport)
{
	this->viewport = viewport;
	mouse_position.x = viewport[2] * 0.5f;
	mouse_position.y = viewport[3] * 0.5f;
}

void tool_system::set_pick(bool enabled)
{
	pick_enabled = enabled;
}

void tool_system::set_sun_direction(const float3& direction)
{
	sun_direction = direction;
}

void tool_system::set_active_tool(ecs::entity entity)
{
	if (active_tool == entity)
		return;
	
	const float descent_time = 0.1f;
	const float ascent_time = 0.1f;
	
	if (active_tool != entt::null)
	{
		auto& tool = registry.get<tool_component>(active_tool);
		tool.active = false;
		command::assign_render_layers(registry, active_tool, 0);
	}
	
	active_tool = entity;
	
	if (active_tool != entt::null)
	{
		auto& tool = registry.get<tool_component>(active_tool);
		tool.active = true;
		
		active_tool_distance = tool.idle_distance;
		
		command::warp_to(registry, active_tool, pick_spring.x0 + float3{0.0f, tool.idle_distance, 0.0f});
		
		// Adjust descend and ascend animations
		animation_channel<float>* channel = descend_animation.get_channel(0);
		channel->remove_keyframes();
		channel->insert_keyframe({0.0, tool.idle_distance});
		channel->insert_keyframe({descent_time, tool.active_distance});
		
		channel = ascend_animation.get_channel(0);
		channel->remove_keyframes();
		channel->insert_keyframe({0.0, tool.active_distance});
		channel->insert_keyframe({ascent_time, tool.idle_distance});
	}
	
	warp = true;
}

void tool_system::set_tool_active(bool active)
{
	tool_active = active;
	
	if (active)
	{
		descend_animation.rewind();
		descend_animation.play();
		
		// Queue tool pressed event
		tool_pressed_event event;
		event.entity = active_tool;
		event.position = pick_spring.x0;
		event_dispatcher->queue(event);
	}
	else
	{
		ascend_animation.rewind();
		ascend_animation.play();
		
			// Queue tool pressed event
		tool_released_event event;
		event.entity = active_tool;
		event.position = pick_spring.x0;
		event_dispatcher->queue(event);
	}
}

void tool_system::handle_event(const mouse_moved_event& event)
{
	if (pick_enabled && was_pick_enabled)
	{
		mouse_position[0] = event.x;
		mouse_position[1] = event.y;
	}
}

void tool_system::handle_event(const window_resized_event& event)
{
	set_viewport({0.0f, 0.0f, static_cast<float>(event.w), static_cast<float>(event.h)});
}

} // namespace ecs
