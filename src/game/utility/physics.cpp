// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/utility/physics.hpp"
#include "game/components/time-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include <engine/physics/kinematics/colliders/mesh-collider.hpp>

std::optional<rigid_body_trace_result> trace_rigid_bodies(const entity::registry& registry, const geom::ray<float, 3>& ray, entity::id ignore_eid, std::uint32_t layer_mask)
{
	entity::id nearest_entity_id = entt::null;
	float nearest_hit_sqr_distance = std::numeric_limits<float>::infinity();
	std::uint32_t nearest_face_index = 0;
	math::fvec3 nearest_hit_normal{};

	// For each entity with a rigid body
	auto rigid_body_view = registry.view<const rigid_body_component>();
	for (const auto entity_id : rigid_body_view)
	{
		if (entity_id == ignore_eid)
		{
			// Ignore a specific entity
			continue;
		}

		// Get rigid body and collider of the entity
		const auto& rigid_body = *rigid_body_view.get<rigid_body_component>(entity_id).body;
		const auto& collider = rigid_body.get_collider();

		if (!collider)
		{
			// Ignore rigid bodies without colliders
			continue;
		}

		if (!(collider->get_layer_mask() & layer_mask))
		{
			// Ignore rigid bodies without a common collision layer
			continue;
		}

		if (collider->type() == physics::collider_type::mesh)
		{
			// Transform ray into rigid body space
			const auto& transform = rigid_body.get_transform();
			geom::ray<float, 3> bs_ray;
			bs_ray.origin = ((ray.origin - transform.translation) * transform.rotation) / transform.scale;
			bs_ray.direction = math::normalize((ray.direction * transform.rotation) / transform.scale);

			const auto& mesh = static_cast<const physics::mesh_collider&>(*collider);
			if (auto intersection = mesh.intersection(bs_ray))
			{
				const auto point = rigid_body.get_transform() * bs_ray.extrapolate(std::get<0>(*intersection));
				const auto sqr_distance = math::sqr_distance(point, ray.origin);

				if (sqr_distance < nearest_hit_sqr_distance)
				{
					nearest_hit_sqr_distance = sqr_distance;
					nearest_entity_id = entity_id;
					nearest_face_index = std::get<1>(*intersection);
					nearest_hit_normal = math::normalize(transform.rotation * (std::get<2>(*intersection) / transform.scale));
				}
			}
		}
	}

	if (nearest_entity_id == entt::null)
	{
		return std::nullopt;
	}

	return rigid_body_trace_result
	{
		nearest_entity_id,
		std::sqrt(nearest_hit_sqr_distance),
		nearest_face_index,
		nearest_hit_normal
	};
}
