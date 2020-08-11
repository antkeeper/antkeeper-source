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

#include "camera-system.hpp"
#include "entity/components/collision-component.hpp"
#include "entity/components/tool-component.hpp"
#include "entity/components/transform-component.hpp"
#include "scene/camera.hpp"
#include "orbit-cam.hpp"
#include "geometry/mesh.hpp"
#include "geometry/intersection.hpp"
#include "math/math.hpp"

using namespace ecs;

camera_system::camera_system(entt::registry& registry):
	entity_system(registry),
	orbit_cam(nullptr),
	viewport{0, 0, 0, 0},
	mouse_position{0, 0}
{}

void camera_system::update(double t, double dt)
{
	if (!orbit_cam)
		return;

	const camera* camera = orbit_cam->get_camera();
	if (!camera)
		return;
	
	/*
	registry.view<transform_component, tool_component>().each(
		[&](auto entity, auto& transform, auto& tool)
		{
			if (!tool.active)
				return;

			float3 screen_position = camera->project(transform.transform.translation, viewport);
			std::cout << screen_position << std::endl;

			// Project tool position onto viewport
		});
	*/
	
	// Cast a ray straight down from infinity at the focal point's lateral coordinates.
	// std::numeric_limits<float>::infinity() actually doesn't work here
	float3 focal_point = orbit_cam->get_target_focal_point();
	float3 pick_origin = float3{focal_point.x, focal_point.y + 500.0f, focal_point.z};
	float3 pick_direction = float3{0, -1, 0};
	ray<float> picking_ray = {pick_origin, pick_direction};

	float a = std::numeric_limits<float>::infinity();
	bool intersection = false;
	float3 pick;

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
			auto mesh_result = ray_mesh_intersection(transformed_ray, *collision.mesh);
			if (std::get<0>(mesh_result))
			{
				intersection = true;
				if (std::get<1>(mesh_result) < a)
				{
					a = std::get<1>(mesh_result);
					pick = picking_ray.extrapolate(a);
				}
			}
		});


	if (intersection)
	{
		//orbit_cam->set_target_focal_point(pick);
	}
}

void camera_system::set_orbit_cam(::orbit_cam* orbit_cam)
{
	this->orbit_cam = orbit_cam;
}

void camera_system::set_viewport(const float4& viewport)
{
	this->viewport = viewport;
}

void camera_system::handle_event(const mouse_moved_event& event)
{
	mouse_position[0] = event.x;
	mouse_position[1] = event.y;
}

