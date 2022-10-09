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

#ifndef ANTKEEPER_GAME_SYSTEM_COLLISION_HPP
#define ANTKEEPER_GAME_SYSTEM_COLLISION_HPP

#include "game/system/updatable.hpp"
#include "entity/id.hpp"
#include "game/component/collision.hpp"

namespace game {
namespace system {

/**
 * Maintains a spatially partitioned set of collision meshes. The set of collision meshes isnot owned by the collision system, so it can be accessed by other systems as well.
 */
class collision: public updatable
{
public:
	collision(entity::registry& registry);
	virtual void update(double t, double dt);

private:	
	void on_collision_construct(entity::registry& registry, entity::id entity_id);
	void on_collision_update(entity::registry& registry, entity::id entity_id);
	void on_collision_destroy(entity::registry& registry, entity::id entity_id);
};

} // namespace system
} // namespace game

#endif // ANTKEEPER_GAME_SYSTEM_COLLISION_HPP

