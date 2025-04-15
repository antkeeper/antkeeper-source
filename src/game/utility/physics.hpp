// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_UTILITY_PHYSICS_HPP
#define ANTKEEPER_GAME_UTILITY_PHYSICS_HPP

#include <entt/entt.hpp>
import engine.geom.primitives.ray;
import engine.math.vector;
import engine.entity.id;
import engine.entity.registry;
import engine.utility.sized_types;
import <optional>;

using namespace engine;

/// Result of a rigid body trace.
struct rigid_body_trace_result
{
	/// ID of the nearest intersecting entity.
	entity::id entity_id{entt::null};

	/// Distance along the ray to the point of intersection.
	float distance{0.0f};

	/// Index of the hit face.
	u32 face_index{0};

	/// Surface normal at the point of intersection.
	math::fvec3 normal{};
};

/// Traces a ray to the nearest point of intersection.
/// @param ray World-spce ray.
/// @param ignore_eid Entity ID with which to ignore intersection.
/// @param layer_mask Mask of collision layers with which the ray can intersect.
/// @return Result of the trace, or std::nullopt if no intersection occurred.
[[nodiscard]] std::optional<rigid_body_trace_result> trace_rigid_bodies(const entity::registry& registry, const geom::ray<float, 3>& ray, entity::id ignore_eid = entt::null, u32 layer_mask = ~u32{0});

#endif // ANTKEEPER_GAME_UTILITY_PHYSICS_HPP
