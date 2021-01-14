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

#ifndef ANTKEEPER_ECS_COMMANDS_HPP
#define ANTKEEPER_ECS_COMMANDS_HPP

#include "ecs/entity.hpp"
#include "ecs/registry.hpp"
#include "utility/fundamental-types.hpp"
#include "math/transform-type.hpp"

namespace ecs {

/// Commands which operate on entity components
namespace command {

void translate(ecs::registry& registry, ecs::entity entity, const float3& translation);
void move_to(ecs::registry& registry, ecs::entity entity, const float3& position);
void warp_to(ecs::registry& registry, ecs::entity entity, const float3& position);
void set_scale(ecs::registry& registry, ecs::entity entity, const float3& scale);
void set_transform(ecs::registry& registry, ecs::entity entity, const math::transform<float>& transform, bool warp = false);
void place(ecs::registry& registry, ecs::entity entity, const float2& translation);
void assign_render_layers(ecs::registry& registry, ecs::entity entity, unsigned int layers);
void bind_transform(ecs::registry& registry, entity source_eid, entity target_eid);
math::transform<float> get_local_transform(ecs::registry& registry, ecs::entity entity);
math::transform<float> get_world_transform(ecs::registry& registry, ecs::entity entity);
void parent(ecs::registry& registry, entity child, entity parent);

} // namespace command
} // namespace ecs

#endif // ANTKEEPER_ECS_COMMANDS_HPP

