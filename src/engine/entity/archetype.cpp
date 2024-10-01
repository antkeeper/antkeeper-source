// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/entity/archetype.hpp>
#include <engine/entity/clone.hpp>

namespace entity {

entity::id archetype::create(entity::registry& registry) const
{
	entt::handle instance_handle(registry, registry.create());
	
	for (const auto& function: stamps)
		function(instance_handle);
	
	return instance_handle.entity();
}

void archetype::stamp(entt::handle& handle) const
{
	for (const auto& function: stamps)
		function(handle);
}

} // namespace entity
