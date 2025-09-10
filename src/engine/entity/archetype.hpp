// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/entity/id.hpp>
#include <engine/entity/registry.hpp>
#include <entt/entt.hpp>
#include <functional>
#include <list>

namespace engine::entity
{
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

}
