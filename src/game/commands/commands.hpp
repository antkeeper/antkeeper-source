/*
 * Copyright (C) 2023  Christopher J. Howard
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

#ifndef ANTKEEPER_GAME_COMMANDS_HPP
#define ANTKEEPER_GAME_COMMANDS_HPP

#include <engine/entity/id.hpp>
#include <engine/entity/registry.hpp>
#include <engine/utility/fundamental-types.hpp>
#include <engine/math/transform-type.hpp>


/// Commands which operate on entity::id components
namespace command {

void translate(entity::registry& registry, entity::id eid, const float3& translation);
void rotate(entity::registry& registry, entity::id eid, float angle, const float3& axis);
void move_to(entity::registry& registry, entity::id eid, const float3& position);
void warp_to(entity::registry& registry, entity::id eid, const float3& position);
void set_scale(entity::registry& registry, entity::id eid, const float3& scale);
void set_transform(entity::registry& registry, entity::id eid, const math::transform<float>& transform, bool warp = false);
void place(entity::registry& registry, entity::id eid, entity::id celestial_body_id, double altitude, double latitude, double longitude);
void assign_render_layers(entity::registry& registry, entity::id eid, unsigned int layers);
math::transform<float> get_local_transform(entity::registry& registry, entity::id eid);
math::transform<float> get_world_transform(entity::registry& registry, entity::id eid);

} // namespace command

#endif // ANTKEEPER_GAME_COMMANDS_HPP
