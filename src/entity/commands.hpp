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

#ifndef ANTKEEPER_ENTITY_COMMANDS_HPP
#define ANTKEEPER_ENTITY_COMMANDS_HPP

#include "entity/id.hpp"
#include "entity/registry.hpp"
#include "utility/fundamental-types.hpp"
#include "math/transform-type.hpp"

namespace entity {

/// Commands which operate on entity::id components
namespace command {

void translate(entity::registry& registry, entity::id entity_id, const float3& translation);
void move_to(entity::registry& registry, entity::id entity_id, const float3& position);
void warp_to(entity::registry& registry, entity::id entity_id, const float3& position);
void set_scale(entity::registry& registry, entity::id entity_id, const float3& scale);
void set_transform(entity::registry& registry, entity::id entity_id, const math::transform<float>& transform, bool warp = false);
void place(entity::registry& registry, entity::id entity_id, entity::id celestial_body_id, double altitude, double latitude, double longitude);
void assign_render_layers(entity::registry& registry, entity::id entity_id, unsigned int layers);
void bind_transform(entity::registry& registry, entity::id source_eid, entity::id target_eid);
math::transform<float> get_local_transform(entity::registry& registry, entity::id entity_id);
math::transform<float> get_world_transform(entity::registry& registry, entity::id entity_id);
void parent(entity::registry& registry, entity::id child, entity::id parent);

} // namespace command
} // namespace entity

#endif // ANTKEEPER_ENTITY_COMMANDS_HPP

