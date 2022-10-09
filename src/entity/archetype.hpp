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

#ifndef ANTKEEPER_ENTITY_ARCHETYPE_HPP
#define ANTKEEPER_ENTITY_ARCHETYPE_HPP

#include "entity/registry.hpp"
#include "entity/id.hpp"
#include <list>
#include <functional>

namespace entity {

/**
 * Entity type template.
 */
struct archetype
{
	/// List of stamp functions which construct instances of the archetype's components.
	std::list<std::function<void(entt::handle&)>> stamps;
	
	/**
	 * Creates an instance of this archetype.
	 *
	 * @param registry Registry in which to create an entity.
	 *
	 * @return Entity ID of the created instance.
	 */
	entity::id create(entity::registry& registry) const;
	
	void stamp(entt::handle& handle) const;
};

} // namespace entity

#endif // ANTKEEPER_ENTITY_ARCHETYPE_HPP
