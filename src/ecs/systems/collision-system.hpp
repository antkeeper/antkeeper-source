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

#ifndef ANTKEEPER_ECS_COLLISION_SYSTEM_HPP
#define ANTKEEPER_ECS_COLLISION_SYSTEM_HPP

#include "entity-system.hpp"
#include "ecs/entity.hpp"
#include "ecs/components/collision-component.hpp"

namespace ecs {

/**
 * Maintains a spatially partitioned set of collision meshes. The set of collision meshes isnot owned by the collision system, so it can be accessed by other systems as well.
 */
class collision_system: public entity_system
{
public:
	collision_system(ecs::registry& registry);
	virtual void update(double t, double dt);

private:	
	void on_collision_construct(ecs::registry& registry, ecs::entity entity, ecs::collision_component& collision);
	void on_collision_replace(ecs::registry& registry, ecs::entity entity, ecs::collision_component& collision);
	void on_collision_destroy(ecs::registry& registry, ecs::entity entity);
};

} // namespace ecs

#endif // ANTKEEPER_ECS_COLLISION_SYSTEM_HPP

