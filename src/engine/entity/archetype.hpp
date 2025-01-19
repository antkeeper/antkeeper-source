// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ENTITY_ARCHETYPE_HPP
#define ANTKEEPER_ENTITY_ARCHETYPE_HPP

#include <engine/entity/registry.hpp>
#include <engine/entity/id.hpp>
#include <list>
#include <functional>

namespace entity {

/// Entity type template.
struct archetype
{
	/// List of stamp functions which construct instances of the archetype's components.
	std::list<std::function<void(entt::handle&)>> stamps;
	
	/// Creates an instance of this archetype.
	/// @param registry Registry in which to create an entity.
	/// @return Entity ID of the created instance.
	entity::id create(entity::registry& registry) const;
	
	void stamp(entt::handle& handle) const;
};

} // namespace entity

#endif // ANTKEEPER_ENTITY_ARCHETYPE_HPP
