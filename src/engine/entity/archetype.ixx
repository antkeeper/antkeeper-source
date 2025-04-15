// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.entity.archetype;
export import engine.entity.id;
export import engine.entity.registry;
export import <entt/entt.hpp>;
import <functional>;
import <list>;

export namespace engine::entity
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
