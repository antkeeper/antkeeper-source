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

#ifndef ANTKEEPER_NEST_SYSTEM_HPP
#define ANTKEEPER_NEST_SYSTEM_HPP

#include "entity-system.hpp"
#include "game/components/nest-component.hpp"

class nest;
class resource_manager;

class nest_system: public entity_system
{
public:
	nest_system(entt::registry& registry, ::resource_manager* resource_manager);
	~nest_system();
	virtual void update(double t, double dt);

private:
	resource_manager* resource_manager;

	void on_nest_construct(entt::registry& registry, entt::entity entity, ecs::nest_component& component);
	void on_nest_destroy(entt::registry& registry, entt::entity entity);
};

#endif // ANTKEEPER_NEST_SYSTEM_HPP

