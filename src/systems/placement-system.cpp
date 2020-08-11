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

#include "placement-system.hpp"
#include "entity/components/collision-component.hpp"
#include "entity/components/placement-component.hpp"
#include "entity/components/transform-component.hpp"
#include "entity/components/terrain-component.hpp"
#include "utility/fundamental-types.hpp"

using namespace ecs;

placement_system::placement_system(entt::registry& registry):
	entity_system(registry)
{}

void placement_system::update(double t, double dt)
{
	registry.view<transform_component, placement_component>().each(
		[&](auto entity, auto& transform, auto& placement)
		{
			bool intersection = false;
			float a = std::numeric_limits<float>::infinity();
			float3 pick;

			registry.view<transform_component, collision_component>().each(
				[&](auto entity, auto& transform, auto& collision)
				{
					// Transform ray into local space of collision component
					math::transform<float> inverse_transform = math::inverse(transform.transform);
					float3 origin = inverse_transform * placement.ray.origin;
					float3 direction = math::normalize(math::conjugate(transform.transform.rotation) * placement.ray.direction);
					ray<float> transformed_ray = {origin, direction};

					// Broad phase AABB test
					auto aabb_result = ray_aabb_intersection(transformed_ray, collision.bounds);
					if (!std::get<0>(aabb_result))
					{
						return;
					}

					auto mesh_result = collision.mesh_accelerator.query_nearest(transformed_ray);
					if (mesh_result)
					{
						intersection = true;
						if (mesh_result->t < a)
						{
							a = mesh_result->t;
							pick = placement.ray.extrapolate(a);
						}
					}
				});

			if (intersection)
			{
				transform.transform.translation = pick;
				transform.warp = true;
				registry.remove<placement_component>(entity);
			}
		});
}

