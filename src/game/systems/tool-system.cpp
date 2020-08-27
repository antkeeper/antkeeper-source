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

#include "tool-system.hpp"
#include "game/components/collision-component.hpp"
#include "game/components/tool-component.hpp"
#include "game/components/transform-component.hpp"
#include "scene/camera.hpp"
#include "animation/orbit-cam.hpp"
#include "geometry/mesh.hpp"
#include "geometry/intersection.hpp"
#include "math/math.hpp"
#include "game/entity-commands.hpp"

using namespace ecs;

tool_system::tool_system(entt::registry& registry):
	entity_system(registry),
	camera(nullptr),
	orbit_cam(orbit_cam),
	viewport{0, 0, 0, 0},
	mouse_position{0, 0},
	pick_enabled(true),
	was_pick_enabled(pick_enabled)
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
	
	active_tool = entt::null;
}

void tool_system::update(double t, double dt)
{
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
		[&](auto entity, auto& transform, auto& collision)
		{
			math::transform<float> inverse_transform = math::inverse(transform.transform);
			float3 origin = inverse_transform * pick_origin;
			float3 direction = math::normalize(math::conjugate(transform.transform.rotation) * pick_direction);
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
	hand_angle_spring.x1 = math::pi<float> - std::min<float>(0.5f, std::max<float>(-0.5f, ((mouse_position[0] / viewport[2]) - 0.5f) * 1.0f)) * (math::pi<float> + math::half_pi<float>);
	
	// Solve springs
	solve_numeric_spring<float, float>(hand_angle_spring, dt);
	solve_numeric_spring<float3, float>(pick_spring, dt);

	// Move active tools to intersection location
	registry.view<tool_component, transform_component>().each(
		[&](auto entity, auto& tool, auto& transform)
		{
			/*
			if (registry.has<model_component>(entity))
			{
				
			}*/
			
			if (!tool.active)
				return;
			
			active_tool = entity;

			if (intersection)
			{
				transform.transform.translation = pick_spring.x0 + float3{0, tool.hover_distance, 0};
			}
			
			// Interpolate between left and right hand
			math::quaternion<float> hand_rotation = math::angle_axis(orbit_cam->get_azimuth() + hand_angle_spring.x0, float3{0, 1, 0});
			
			if (tool.heliotropic)
			{
				math::quaternion<float> solar_rotation = math::rotation(float3{0, -1, 0}, sun_direction);
				transform.transform.translation = pick_spring.x0 + solar_rotation * float3{0, tool.hover_distance, 0};
				
				transform.transform.rotation = solar_rotation * hand_rotation;
			}
			else
			{
				transform.transform.rotation = hand_rotation;
			}

			//math::quaternion<float> rotation = math::angle_axis(orbit_cam->get_azimuth() + pick_angle, float3{0, 1, 0});
			//transform.transform.rotation = rotation;
		});
	
	was_pick_enabled = pick_enabled;
}

void tool_system::set_camera(const ::camera* camera)
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
}

void tool_system::set_pick(bool enabled)
{
	pick_enabled = enabled;
}

void tool_system::set_sun_direction(const float3& direction)
{
	sun_direction = direction;
}

void tool_system::set_active_tool(entt::entity entity)
{
	if (active_tool != entt::null)
	{
		auto& tool = registry.get<tool_component>(active_tool);
		tool.active = false;
		ec::assign_render_layers(registry, active_tool, 0);
	}
	
	active_tool = entity;
	
	if (active_tool != entt::null)
	{
		auto& tool = registry.get<tool_component>(active_tool);
		tool.active = true;
		ec::assign_render_layers(registry, active_tool, 1);
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
