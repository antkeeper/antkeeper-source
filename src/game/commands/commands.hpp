// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_COMMANDS_HPP
#define ANTKEEPER_GAME_COMMANDS_HPP

import engine.math.vector;
import engine.math.transform;
import engine.entity.id;
import engine.entity.registry;

using namespace engine;

/// Commands which operate on entity::id components
namespace command
{
	void translate(entity::registry& registry, entity::id eid, const math::fvec3& translation);
	void rotate(entity::registry& registry, entity::id eid, float angle, const math::fvec3& axis);
	void move_to(entity::registry& registry, entity::id eid, const math::fvec3& position);
	void warp_to(entity::registry& registry, entity::id eid, const math::fvec3& position);
	void set_scale(entity::registry& registry, entity::id eid, const math::fvec3& scale);
	void set_transform(entity::registry& registry, entity::id eid, const math::transform<float>& transform);
	void assign_render_layers(entity::registry& registry, entity::id eid, unsigned int layers);
	math::transform<float> get_local_transform(entity::registry& registry, entity::id eid);
	math::transform<float> get_world_transform(entity::registry& registry, entity::id eid);
}

#endif // ANTKEEPER_GAME_COMMANDS_HPP

