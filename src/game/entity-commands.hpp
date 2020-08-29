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

#ifndef ANTKEEPER_ECS_ENTITY_COMMANDS_HPP
#define ANTKEEPER_ECS_ENTITY_COMMANDS_HPP

#include "utility/fundamental-types.hpp"
#include "math/transform-type.hpp"
#include <entt/entt.hpp>

namespace ec {

void translate(entt::registry& registry, entt::entity eid, const float3& translation);
void move_to(entt::registry& registry, entt::entity eid, const float3& position);
void warp_to(entt::registry& registry, entt::entity eid, const float3& position);
void set_scale(entt::registry& registry, entt::entity eid, const float3& scale);
void set_transform(entt::registry& registry, entt::entity eid, const math::transform<float>& transform, bool warp = false);
void place(entt::registry& registry, entt::entity eid, const float2& translation);
void assign_render_layers(entt::registry& registry, entt::entity eid, unsigned int layers);
void bind_transform(entt::registry& registry, entt::entity source_eid, entt::entity target_eid);
math::transform<float> get_local_transform(entt::registry& registry, entt::entity eid);
math::transform<float> get_world_transform(entt::registry& registry, entt::entity eid);
void parent(entt::registry& registry, entt::entity child, entt::entity parent);

} // namespace ec

#endif // ANTKEEPER_ECS_ENTITY_COMMANDS_HPP

