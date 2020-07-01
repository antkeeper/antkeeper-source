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

#ifndef ANTKEEPER_COLLISION_SYSTEM_HPP
#define ANTKEEPER_COLLISION_SYSTEM_HPP

#include "entity-system.hpp"
#include "entity/components/collision-component.hpp"

/**
 * Maintains a spatially partitioned set of collision meshes. The set of collision meshes isnot owned by the collision system, so it can be accessed by other systems as well.
 */
class collision_system: public entity_system
{
public:
	collision_system(entt::registry& registry);
	virtual void update(double t, double dt);

private:	
	void on_collision_construct(entt::registry& registry, entt::entity entity, ecs::collision_component& collision);
	void on_collision_replace(entt::registry& registry, entt::entity entity, ecs::collision_component& collision);
	void on_collision_destroy(entt::registry& registry, entt::entity entity);
};

#endif // ANTKEEPER_COLLISION_SYSTEM_HPP

